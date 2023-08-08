/** @file
  Qualcomm functions for operations on capsule partition from DXE capsule library.

  Caution: This module requires additional review when modified.
  This module will have external input - capsule image.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.

  Copyright (c) 2017-2018, Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
**/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/28/18   prema   Fixes to update capsule partition using unaligned buffer
 06/08/18   prema   Fixes to enable casule partition usage for NVME and UFS devices
 03/24/17   gparam  New functions to abstract some more operations on capsule
                    persisting media + Correction of function headers +
                    Changes to do memory free in a cleaner way
 02/22/17   gparam  Initial revision

=============================================================================*/
#include <Protocol/Capsule.h>
#include <Protocol/EFICapsule.h>
#include <Protocol/SimpleFileSystem.h>

#include <Guid/CapsuleVendor.h>
#include <Guid/FmpCapsule.h>
#include <Guid/FileInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/EventGroup.h>

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/CapsuleLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SortLib.h>
#include <Library/HobLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BootConfig.h>

#include "DxeCapsuleQcom.h"
#include "DxeCapsulePartition.h"

EFI_BLOCK_IO_PROTOCOL        *CapsuleBlkIo = NULL;

extern VOID                 **mCapsulePtr;
extern UINT32                *mCapsuleOffsetArray;
extern VOID                  *CapsuleStorageHeader;
extern UINT32                 mCapsuleTotalNumber;
extern EFI_STATUS            *mCapsuleStatusArray;
extern EFI_GUID               gEfiNvme0Guid;
extern EFI_GUID               gEfiUfsLU4Guid;


/**
  Validates the capsule GUID table in capsule partition.
  
  @param[in]  CapsuleGuidTable Capsule GUID table buffer pointer to be
                               initialized and updated to the storage media.

  @retval EFI_SUCESS            Action completed successfully
  @retval EFI_INVALID_PARAMETER Invalid input parameter.

**/
EFI_STATUS
EFIAPI
ValidateCapsulePartitionGuidTable (
  IN CAPSULE_PARTITION_GUID_TABLE *CapsuleGuidTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Crc32 = 0;
  do {
    if (CapsuleGuidTable == NULL) {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    Crc32 = CapsuleGuidTable->Crc32;
    CapsuleGuidTable->Crc32  = 0;

    Status = CalculateCrc32((VOID*)&CapsuleGuidTable->Crc32,
                            sizeof(SCATTERGATHERLIST_ARRAY_HEADER));
    
    if (EFI_ERROR (Status)) {
      break;
    }

    if ((Crc32 != CapsuleGuidTable->Crc32) ||
        (CapsuleGuidTable->Signature != CAPSULE_GUID_TABLE_SIGNATURE)) {
      // restore old CRC value
      CapsuleGuidTable->Crc32 = Crc32;
      Status = EFI_NOT_FOUND;
      break;
    }
  } while (0);

  return Status;
}

/**
  Initialize the capsule GUID table in capsule partition. This accepts the
  pointer to the memory that can be used as the buffer to initialize the
  default data and updates the same to the capsule partition
  
  @param[in]  CapsuleGuidTable Capsule GUID table buffer pointer to be
                               initialized and updated to the storage media.

  @retval EFI_SUCESS            Action completed successfully
  @retval EFI_INVALID_PARAMETER Invalid input parameter.

**/
EFI_STATUS
EFIAPI
InitCapsulePartitionGuidTable (
  CAPSULE_PARTITION_GUID_TABLE *CapsuleGuidTable
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  UINTN              HeaderBlocks = EFI_SIZE_TO_BLOCKS (sizeof (*CapsuleGuidTable), CapsuleBlkIo->Media->BlockSize);

  if (NULL == CapsuleGuidTable) {
    return EFI_INVALID_PARAMETER;
  }

  if (!UpdateCapsulePartitionExists()) {
    Status = EFI_NOT_FOUND;
    DEBUG ((EFI_D_ERROR, "Cannot initialize GUID table as capsule partition does not exist\n"));
    return Status;
  }

  // This effectively deletes all capsules in the partition
  CapsuleGuidTable->Signature = CAPSULE_GUID_TABLE_SIGNATURE;
  CapsuleGuidTable->Revision = CAPSULE_PART_GUID_TABLE_REVISION;
  CapsuleGuidTable->GuidCount = 0;
  CapsuleGuidTable->BlockSize = CapsuleBlkIo->Media->BlockSize;
  CapsuleGuidTable->BlockCount = CapsuleBlkIo->Media->LastBlock + 1;
  CapsuleGuidTable->FreeBlocks = CapsuleGuidTable->BlockCount - HeaderBlocks ;

  // Clear out all Guid entries
  SetMem(&(CapsuleGuidTable->CapsuleGuidEntries[0]),
         sizeof(CapsuleGuidTable->CapsuleGuidEntries),
         0x0);

  CapsuleGuidTable->Crc32 = 0;

  Status = CalculateCrc32((VOID*)&CapsuleGuidTable->Crc32,
                          sizeof(SCATTERGATHERLIST_ARRAY_HEADER));
  
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to calculate CRC value after clearing capsule GUID table. Status: %r\n", Status));
    return Status;
  }

  // Copy back Capsule Guid Table to persisted storage
  Status = CapsuleBlkIo->WriteBlocks (CapsuleBlkIo,
                                      CapsuleBlkIo->Media->MediaId,
                                      0,
                                      EFI_BLOCKS_TO_SIZE (HeaderBlocks, CapsuleBlkIo->Media->BlockSize),
                                      CapsuleGuidTable);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to initialize capsule partition header. Status: %r\n", Status));
    return Status;
  }

  return Status;
}

/**
  Checks whether the Update Capsule partition exists.

  @retval TRUE                  Update Capsule partition does exist.
  @retval FALSE                 Update Capsule partition does not exist.

**/
BOOLEAN
EFIAPI
UpdateCapsulePartitionExists (
  VOID
  )
{
  if (NULL != CapsuleBlkIo) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Delete the capsule matching the input parameter from persisted capsules in capsule partition.
  
  @param[in]  CapsuleGuid      File name of the mass-storage capsule.
  @param[in]  CapsuleOffset    Offset of the capsule in storage, if 0 capsule did
                               not come from storage

  @retval EFI_SUCESS            Action completed successfully
  @retval EFI_INVALID_PARAMETER Invalid input parameter.

**/
EFI_STATUS
EFIAPI
DeletePersistedCapsuleFromCapsulePartition(
  IN EFI_GUID           *CapsuleGuid,
  IN UINT32              CapsuleOffset
  )
{
  EFI_STATUS                   Status = EFI_SUCCESS;
  UINT32                       *MemLoc = NULL;
  UINT32                       HeaderSize = 0;
  CAPSULE_PARTITION_GUID_TABLE *CapsuleGuidTable = NULL;
  UINT32                       Index = 0;
  UINTN                        HeaderBlocks = EFI_SIZE_TO_BLOCKS (sizeof (*CapsuleGuidTable), CapsuleBlkIo->Media->BlockSize);

  do
  {
    if (NULL == CapsuleGuid) {
      DEBUG ((DEBUG_ERROR, "Invalid value received for parameter CapsuleGuid\n"));
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if (!UpdateCapsulePartitionExists())
    {
      Status = EFI_UNSUPPORTED;
      break;
    }

    HeaderSize = EFI_BLOCKS_TO_SIZE (HeaderBlocks, CapsuleBlkIo->Media->BlockSize);
    Status = gBS->AllocatePool(EfiBootServicesData,
                               HeaderSize,
                               (VOID**)&MemLoc);

    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "Allocate-memory for capsule storage header returned error - %r\n", Status));
      break;
    }

    // Load the Capsule Guid Table into memory
    Status = CapsuleBlkIo->ReadBlocks (CapsuleBlkIo, 
                                       CapsuleBlkIo->Media->MediaId, 
                                       0, 
                                       HeaderSize, 
                                       MemLoc);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Capsule storage header read during capsule delete failed. Status: %r\n", Status));
      break;
    }

    CapsuleGuidTable = (CAPSULE_PARTITION_GUID_TABLE*)MemLoc;
    Status = ValidateCapsulePartitionGuidTable(CapsuleGuidTable);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Invalid capsule storage header found during capsule delete. Status: %r\n", Status));
      break;
    }

    // Find Guid entry corresponding to capsule
    for (Index = 0; Index < CapsuleGuidTable->GuidCount; Index++)
    {
      if ((CompareGuid(&(CapsuleGuidTable->CapsuleGuidEntries[Index].Guid), 
                       CapsuleGuid) == TRUE) &&
          (CapsuleOffset == CapsuleGuidTable->CapsuleGuidEntries[Index].BlockOffset))
      {
        // Check if last entry in list;
        // Don't update CapsuleGuidTable->FreeBlocks unless all entries are 
        // deleted. Once all capsules are deleted it will clear out the entire 
        // table and thus free up the partition. If a new capsule is inserted,
        // it will be allocated from the remaining FreeBlocks and not reallocate
        // from the blocks taken from the deleted capsule.
        if (Index == CapsuleGuidTable->GuidCount - 1)
        {
          // If last entry, zero out last entry
          SetMem(&(CapsuleGuidTable->CapsuleGuidEntries[Index].Guid), 
                 sizeof(CAPSULE_GUID_ENTRY), 
                 0x0);
          CapsuleGuidTable->GuidCount--;
        }
        else
        {
          // If not last entry, shift all entries up one
          CopyMem(&(CapsuleGuidTable->CapsuleGuidEntries[Index]),
                  &(CapsuleGuidTable->CapsuleGuidEntries[Index + 1]),
                  (CapsuleGuidTable->GuidCount - Index - 1)*sizeof(CAPSULE_GUID_ENTRY));
    
          // Zero out last entry
          SetMem(&(CapsuleGuidTable->CapsuleGuidEntries[CapsuleGuidTable->GuidCount - 1]), 
                 sizeof(CAPSULE_GUID_ENTRY), 
                 0x0);
          CapsuleGuidTable->GuidCount--;
        }
      }
    }

    if (CapsuleGuidTable->GuidCount == 0)
    {
      // Reinitialize Capsule Guid Table 
      // This effectively deletes all capsules in the partition
      CapsuleGuidTable->Signature = CAPSULE_GUID_TABLE_SIGNATURE;
      CapsuleGuidTable->Revision = CAPSULE_PART_GUID_TABLE_REVISION;
      CapsuleGuidTable->GuidCount = 0; 
      CapsuleGuidTable->BlockSize = CAPSULE_PART_GUID_TABLE_BLOCK_SIZE;
      CapsuleGuidTable->BlockCount = CapsuleBlkIo->Media->LastBlock;
      CapsuleGuidTable->FreeBlocks = CapsuleBlkIo->Media->LastBlock - 1;

      // Clear out all Guid entries
      SetMem(&(CapsuleGuidTable->CapsuleGuidEntries[0]), 
             sizeof(CapsuleGuidTable->CapsuleGuidEntries),
             0x0);
    }

    CapsuleGuidTable->Crc32 = 0;
    Status = CalculateCrc32((VOID*)&CapsuleGuidTable->Crc32,
                            sizeof(SCATTERGATHERLIST_ARRAY_HEADER));
    
    if (EFI_ERROR (Status))
    {
      break;
    }

    // Copy back Capsule Guid Table to persisted storage
    Status = CapsuleBlkIo->WriteBlocks (CapsuleBlkIo, 
                                        CapsuleBlkIo->Media->MediaId, 
                                        0, 
                                        HeaderSize, 
                                        MemLoc);
    if (EFI_ERROR (Status))
    {
      DEBUG ((EFI_D_ERROR, "Capsule storage header update during capsule delete failed. Status: %r\n", Status));
      break;
    }
  }while(0);

  if (NULL != MemLoc)
  {
    gBS->FreePool(MemLoc);
  }

  return Status;
}

/**
  This code loads capsules in boot time from the NV storage and processes them 
  in the same way as capsules in RAM.

  @param [in/out] CapsulePtrArray     Array of capsules with array length CapsuleArrayLen.
  @param [in/out] CapsuleOffsetArray  Array of capsules offset within the capsule storage.
  @param [in] CapsuleArrayLen     Length of the arrays CapsulePtrArray and CapsuleOffsetArray.

  @retval EFI_SUCCESS             UEFI Capsule Runtime Services are installed successfully. 
  @retval EFI_INVALID_PARAMETER   Invalid paramter received
  @retval EFI_VOLUME_CORRUPTED    Capsule partition corrupted

**/
EFI_STATUS
EFIAPI
LoadPersistedCapsulesFromCapsulePartition (
  IN OUT VOID **CapsulePtrArray,
  IN OUT UINT32 *CapsuleOffsetArray,
  IN UINT32 CapsuleArrayLen
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_GUID                      InvalidGuid = CAPSULE_INVALID_GUID;
  CAPSULE_PARTITION_GUID_TABLE  *CapsuleGuidTable = (CAPSULE_PARTITION_GUID_TABLE *) CapsuleStorageHeader;
  CAPSULE_GUID_ENTRY            *CapsuleGuidEntry = NULL;
  UINT32                        Index;

  UINT32                        *MemLoc = NULL;
  UINT32                        ImgSize = 0;

  do {
    if (NULL == CapsulePtrArray) {
        Status = EFI_INVALID_PARAMETER;
        DEBUG ((DEBUG_ERROR, "Invalid value received for parameter CapsulePtrArray\n"));
        break;
    }

    if (NULL == CapsuleOffsetArray) {
        Status = EFI_INVALID_PARAMETER;
        DEBUG ((DEBUG_ERROR, "Invalid value received for parameter CapsuleOffsetArray\n"));
        break;
    }

    if (CapsuleBlkIo == NULL) {
      Status = EFI_NOT_FOUND;
      DEBUG ((DEBUG_ERROR, "Capsule partition read pointers not initialized. Error in loading capsules from partition\n"));
      break;
    }

    // Calculate CRC on table, if doesn't match or is uninitialized, reinitialize
    Status = ValidateCapsulePartitionGuidTable(CapsuleGuidTable);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ValidateCapsulePartitionGuidTable returned error - %r\n", Status));
      break;
    }

    if (CapsuleArrayLen != CapsuleGuidTable->GuidCount) {
      Status = EFI_INVALID_PARAMETER;
      DEBUG ((DEBUG_ERROR, "ValidateCapsulePartitionGuidTable returned error - %r\n", Status));
      break;
    }
      
    // Load each capsule and store it to capsule pointer
    for (Index = 0; Index < CapsuleGuidTable->GuidCount; Index++) {
      CapsuleGuidEntry = &(CapsuleGuidTable->CapsuleGuidEntries[Index]);

      // Confirm GUID entry is valid
      if (CompareGuid(&(CapsuleGuidEntry->Guid), &InvalidGuid)) {
        // This should never be the case
        Status = EFI_VOLUME_CORRUPTED;
        DEBUG ((DEBUG_ERROR, "Capsule partition corrupted. Invalid capsule table GUID\n"));
        break;
      }

      // Get the Capsule size. Account for one extra block in case not a
      // multiple of block size
      ImgSize = EFI_BLOCKS_TO_SIZE (EFI_SIZE_TO_BLOCKS (CapsuleGuidEntry->ImageSize, CapsuleBlkIo->Media->BlockSize), CapsuleBlkIo->Media->BlockSize);

      if ((CapsuleGuidEntry->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) != 0) {
        Status = gBS->AllocatePool(EfiRuntimeServicesData,
                                   ImgSize,
                                   (VOID**)&MemLoc);
      } else {
        Status = gBS->AllocatePool(EfiBootServicesData,
                                   ImgSize,
                                   (VOID**)&MemLoc);
      }
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Failed to allocate memory to load persisted capsule\n"));
        break;
      }

      // Load entire capsule into memory
      Status = CapsuleBlkIo->ReadBlocks (CapsuleBlkIo,
                                         CapsuleBlkIo->Media->MediaId,
                                         CapsuleGuidEntry->BlockOffset,
                                         ImgSize,
                                         MemLoc);
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Capsule partition read for index %d returned error - %r\n", Index, Status));
        break;
      }
        
      // Copy to pointer to Capsule pointer array
      CapsulePtrArray[Index] = MemLoc;
      
      // Copy to block offset to Capsule offset array
      CapsuleOffsetArray[Index] = CapsuleGuidEntry->BlockOffset;
    }
  } while(0);

  return Status;
}

/**
  Persists capsules received in the form of capsule array to capsule partition.

  @param [in] CapsuleHeaderArray Virtual pointer to an array of virtual pointers
                                 to the capsules being passed into update capsule.
  @param [in] CapsuleCount       Number of pointers to EFI_CAPSULE_HEADER in
                                 CaspuleHeaderArray.

  @retval EFI_SUCCESS           Successfully persisted given capsules.
  @retval EFI_DEVICE_ERROR      The capsule update was started, but failed
                                due to a device error.
  @retval EFI_OUT_OF_RESOURCES  When ExitBootServices() has been previously
                                called this error indicates the capsule 
                                is compatible with this platform but is not
                                capable of being submitted or processed 
                                in runtime. The caller may resubmit the capsule
                                prior to ExitBootServices().
  @retval EFI_OUT_OF_RESOURCES  When ExitBootServices() has not been previously
                                called then this error indicates the capsule is
                                compatible with this platform but there are
                                insufficient resources to process.

**/
EFI_STATUS
EFIAPI
PersistCapsulesToCapsulePartition (
  IN EFI_CAPSULE_HEADER    **CapsuleHeaderArray,
  IN UINTN                   CapsuleCount
  )
{
  UINTN                         ArrayNumber = 0;
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_CAPSULE_HEADER           *CapsuleHeader = NULL;
  EFI_CAPSULE_HEADER           *CapsuleHeaderAligned = NULL;
  UINTN                         BlockAlignedSize = 0;
  CAPSULE_PARTITION_GUID_TABLE *CapsuleGuidTable = (CAPSULE_PARTITION_GUID_TABLE *) CapsuleStorageHeader;
  EFI_GUID                      UniqueCapsuleGuid;

  //
  // Walk through all capsules, store capsules to persist
  //
  for (ArrayNumber = 0; ArrayNumber < CapsuleCount ; ArrayNumber++) {
    CapsuleHeader = CapsuleHeaderArray[ArrayNumber];
    if ((CapsuleHeader->Flags & CAPSULE_FLAGS_PERSIST_ACROSS_RESET) != 0) {
      if ((CapsuleGuidTable->GuidCount >= CAPSULE_GUID_ENTRIES_MAX) ||
          ((CapsuleGuidTable->FreeBlocks * CapsuleGuidTable->BlockSize) < CapsuleHeader->CapsuleImageSize)) {
        DEBUG ((EFI_D_ERROR, "Failed to persist capsule to capsule partition. Not enough resources.\n"));
        Status = EFI_OUT_OF_RESOURCES;
        goto PersistCapsulesToCapsulePartitionExit;
      }

      Status = GetUniqueCapsuleGuid (CapsuleHeader, &UniqueCapsuleGuid);
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Failed to get unique GUID of capsule to store in capsule partition. Returning error %r\n", Status));
        goto PersistCapsulesToCapsulePartitionExit;
      }

      CopyMem (&CapsuleGuidTable->CapsuleGuidEntries[CapsuleGuidTable->GuidCount].Guid, &UniqueCapsuleGuid, sizeof(EFI_GUID));
      CapsuleGuidTable->CapsuleGuidEntries[CapsuleGuidTable->GuidCount].BlockOffset = CapsuleGuidTable->BlockCount - CapsuleGuidTable->FreeBlocks;
      CapsuleGuidTable->CapsuleGuidEntries[CapsuleGuidTable->GuidCount].ImageSize = CapsuleHeader->CapsuleImageSize;
      ALIGN_TO_BLOCK (CapsuleHeader->CapsuleImageSize, CapsuleGuidTable->BlockSize, BlockAlignedSize);
      CapsuleGuidTable->FreeBlocks -= EFI_SIZE_TO_BLOCKS (BlockAlignedSize, CapsuleGuidTable->BlockSize);;
      CapsuleGuidTable->CapsuleGuidEntries[CapsuleGuidTable->GuidCount].Flags = CapsuleHeader->Flags;

      // Free the buffer from last iteration of the loop
      if (NULL != CapsuleHeaderAligned)
      {
        FreePool (CapsuleHeaderAligned);
        CapsuleHeaderAligned = NULL;
      }

      CapsuleHeaderAligned = AllocateZeroPool (BlockAlignedSize);

      if (CapsuleHeaderAligned == NULL)
      {
        DEBUG ((EFI_D_ERROR, "Failed to allocate buffer of size %d to copy capsule to block aligned buffer.\n", BlockAlignedSize));
        Status = EFI_OUT_OF_RESOURCES;
        goto PersistCapsulesToCapsulePartitionExit;
      }
      
      CopyMem (
        CapsuleHeaderAligned,
        CapsuleHeader,
        CapsuleHeader->CapsuleImageSize
        );

      Status = CapsuleBlkIo->WriteBlocks (CapsuleBlkIo,
                                          CapsuleBlkIo->Media->MediaId,
                                          CapsuleGuidTable->CapsuleGuidEntries[CapsuleGuidTable->GuidCount].BlockOffset,
                                          BlockAlignedSize,
                                          CapsuleHeaderAligned);
      CapsuleGuidTable->GuidCount++;
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Failed to persist capsule to capsule partition. Status: %r. Returning alternative error %r\n", Status, EFI_DEVICE_ERROR));
        Status = EFI_DEVICE_ERROR;
        goto PersistCapsulesToCapsulePartitionExit;
      }
    }
  }

  //Update CRC and store header
  CapsuleGuidTable->Crc32 = 0;
  Status = CalculateCrc32((VOID*)&CapsuleGuidTable->Crc32,
                          sizeof(SCATTERGATHERLIST_ARRAY_HEADER));

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to calculate CRC after persisting capsules. Status: %r. Returning alternative error %r\n", Status, EFI_DEVICE_ERROR));
    Status = EFI_DEVICE_ERROR;
    goto PersistCapsulesToCapsulePartitionExit;
  }

  ALIGN_TO_BLOCK (sizeof(CAPSULE_PARTITION_GUID_TABLE), CapsuleGuidTable->BlockSize, BlockAlignedSize);

  Status = CapsuleBlkIo->WriteBlocks (CapsuleBlkIo,
                                      CapsuleBlkIo->Media->MediaId,
                                      0,
                                      BlockAlignedSize,
                                      CapsuleGuidTable);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to add storage header to capsule partition. Status: %r. Returning alternative error %r\n", Status, EFI_DEVICE_ERROR));
    Status = EFI_DEVICE_ERROR;
    goto PersistCapsulesToCapsulePartitionExit;
  }

PersistCapsulesToCapsulePartitionExit:

  if (NULL != CapsuleHeaderAligned)
  {
    FreePool (CapsuleHeaderAligned);
    CapsuleHeaderAligned = NULL;
  }

  return Status;
}

/**
  Get the maximum capsule size supported in the update capsule partition.
  
  @param  MaxiumCapsuleSize     Maximum capsule size supported in the update
                                capsule partition.
   
  @retval EFI_SUCESS            Action completed successfully
  @retval EFI_INVALID_PARAMETER Invalid input parameter.

**/
EFI_STATUS
EFIAPI
GetMaxCapsuleSizeInCapsulePartition (
  OUT UINT64             *MaxiumCapsuleSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (NULL != MaxiumCapsuleSize) {
    CAPSULE_PARTITION_GUID_TABLE *CapsuleGuidTable = (CAPSULE_PARTITION_GUID_TABLE *)CapsuleStorageHeader;

    *MaxiumCapsuleSize = EFI_BLOCKS_TO_SIZE (CapsuleGuidTable->FreeBlocks,
                                             CapsuleBlkIo->Media->BlockSize);    
  } else {
    Status = EFI_INVALID_PARAMETER;
  }
  
  return Status;
}

/**
  Get the space needed to store the capsule in the current partition
  
  @param  CapsuleImageSize      Size of the capsule eceived in the capsule
                                header array.
   
  @retval Size needed to store the capsule in the current storage media

**/
UINTN
EFIAPI
GetCapsuleSizeInCapsulePartition (
  IN UINT32             CapsuleImageSize
  )
{
  return EFI_BLOCKS_TO_SIZE (EFI_SIZE_TO_BLOCKS (CapsuleImageSize,
                                                 CapsuleBlkIo->Media->BlockSize),
                             CapsuleBlkIo->Media->BlockSize);
}

/**
  Checks whether the Update Capsule partition exists.
  This function also loads and validates the header of the capsule partition

  @retval EFI_SUCESS            Action completed successfully
  @retval EFI_INVALID_PARAMETER Invalid input parameter.
**/
EFI_STATUS
EFIAPI
InitStoragePointersForCapsulePartition (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN       HeaderSize = 0;
  UINT32      Attribs = 0;
  PartiSelectFilter HandleFilter;
  // Use size as 2 to detect existance of more than one capsule partition while query
  HandleInfo  HandleInfoList [2];
  UINT32      NoHandles = sizeof(HandleInfoList) / sizeof(*HandleInfoList);

  if (NULL != CapsuleBlkIo) {
    return EFI_SUCCESS;
  }

  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  HandleFilter.VolumeName = 0;
  HandleFilter.PartitionType = &gUpdateCapsulePartitionGuid;

  if (boot_from_nvme())
  {
    Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE;
    HandleFilter.RootDeviceType = &gEfiNvme0Guid;
  }
  else if (boot_from_ufs())
  {
    Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE;
    HandleFilter.RootDeviceType = &gEfiUfsLU4Guid;
  }
  
  Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoList, &NoHandles);

  if (EFI_ERROR (Status))
  {
    DEBUG ((EFI_D_ERROR, "GetBlkIOHandles returned Status: %r\n", Status));
    return Status;
  }
  
  if (0 == NoHandles) {
    Status = EFI_NOT_FOUND;
    DEBUG ((EFI_D_ERROR, "Update Capsule partition not found. Status: %r\n", Status));
    return Status;
  }

  if (NoHandles != 1) {
    DEBUG ((EFI_D_ERROR, "More than 1 update Capsule partition found\n"));
    return EFI_DEVICE_ERROR;
  }

  Status = gBS->HandleProtocol (HandleInfoList[0].Handle, &gEfiBlockIoProtocolGuid, (VOID **)&CapsuleBlkIo);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Update Capsule partition does not support block io. Status: %r\n", Status));
    return Status;
  }

  if (NULL == CapsuleBlkIo) {
    DEBUG ((EFI_D_ERROR, "Capsule partition block io is NULL\n"));
    return EFI_DEVICE_ERROR;
  }

  HeaderSize = EFI_BLOCKS_TO_SIZE (EFI_SIZE_TO_BLOCKS (sizeof (CAPSULE_PARTITION_GUID_TABLE), CapsuleBlkIo->Media->BlockSize), CapsuleBlkIo->Media->BlockSize);

  Status = gBS->AllocatePool (EfiRuntimeServicesData, HeaderSize, &CapsuleStorageHeader);
  if (EFI_ERROR (Status) || (CapsuleStorageHeader == NULL)) {
    DEBUG ((EFI_D_ERROR, "Failed to allocate capsule partition storage header. Status: %r\n", Status));
    return (EFI_ERROR (Status) ? Status : EFI_OUT_OF_RESOURCES);
  }

  Status = CapsuleBlkIo->ReadBlocks (CapsuleBlkIo,
                                     CapsuleBlkIo->Media->MediaId,
                                     0,
                                     HeaderSize,
                                     CapsuleStorageHeader);
  if (EFI_ERROR (Status)) {
    gBS->FreePool (CapsuleStorageHeader);
    DEBUG ((EFI_D_ERROR, "Failed to read capsule partition header. Status: %r\n", Status));
    return Status;
  }

  Status = ValidateCapsulePartitionGuidTable ((CAPSULE_PARTITION_GUID_TABLE *)CapsuleStorageHeader);
  if (EFI_ERROR (Status)) {
    Status = InitCapsulePartitionGuidTable ((CAPSULE_PARTITION_GUID_TABLE *)CapsuleStorageHeader);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Failed to initilize capsule partition storage header. Status: %r\n", Status));
      return Status;
    }
  }

  return Status;
}

/**
  De-initialize the capsule pointer memory for the capsule loaded from
  capsule partition. This function clears the pointer after de-initialization

  @param[in]  CapsulePtr       Points to the pointer to the capsule to be
                               deinitialized. 

  @retval EFI_SUCESS            Action completed successfully
  @retval EFI_INVALID_PARAMETER Invalid parameter.
  @retval EFI_UNSUPPORTED       Capsule image is not supported by the firmware.

**/
EFI_STATUS
EFIAPI
DeInitCapsulePtrMemoryForCapsulePartition (
  IN OUT VOID               **CapsulePtr
  )
{
  return DeInitCapsulePtrMemoryGeneric (CapsulePtr);
}

