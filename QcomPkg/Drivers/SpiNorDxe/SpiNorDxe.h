/**
@file     SpiNor.h
@brief   SPI NOR DXE Header File

This file contains the definitions of the constants, data structures,
and interfaces for the SPINOR driver in UEFI.

 Copyright (c) 2017-2018, Qualcomm Technologies, Inc. All rights reserved.
 Portions Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED
**/

/*=============================================================================
                              EDIT HISTORY


when            who   what, where, why
----------      ---   -----------------------------------------------------------
2018-03-23      wek   Fix indentation
2018-03-20      md    Updates/Fixes to handle SCM calls
2017-11-14      jt    Initial version

=============================================================================*/

#ifndef _SPINOR_H_
#define _SPINOR_H_

/** @name SPINOR Device Signature
      Signature as per UEFI requirement.
  */
/*@{*/
#define SPINOR_DEV_SIGNATURE SIGNATURE_32 ('s', 'n', 'o', 'r')
/*@}*/


/**
  SPINOR Device Structure
  */
typedef struct {
  UINT32                     Signature;
  EFI_HANDLE                 ClientHandle;  /**< -- Client Handle */
  EFI_BLOCK_IO_PROTOCOL      BlkIo;         /**< -- Block I/O */
  EFI_ERASE_BLOCK_PROTOCOL   EraseBlk;      /**< -- Erase block */
} SPINOR_DEV;


/**
   Reset the Block Device.

   @param  This                 Indicates a pointer to the calling context.
   @param  ExtendedVerification Driver may perform diagnostics on reset.

   @retval EFI_SUCCESS          The device was reset.
   @retval EFI_DEVICE_ERROR     The device is not functioning properly
                                and could not be reset.

**/
EFI_STATUS EFIAPI SpiNorReset (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN BOOLEAN                 ExtendedVerification
   );


/**
   Read BufferSize bytes from Lba into Buffer.

   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    Id of the media, changes every time the media is replaced.
   @param  Lba        The starting Logical Block Address to read from
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the destination buffer for the data.
                      The caller is responsible for either having implicit or
                      explicit ownership of the buffer.

   @retval EFI_SUCCESS           The data was read correctly from the device.
   @retval EFI_DEVICE_ERROR      The device reported an error while
                                 performing the read.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHANGED     The MediaId does not matched the
                                 current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block
                                 size of the device.
   @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not
                                 valid,or the buffer is not on proper alignment.
EFI_STATUS
**/
EFI_STATUS EFIAPI SpiNorReadBlocks (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN UINT32                  MediaId,
   IN EFI_LBA                 Lba,
   IN UINTN                   BufferSize,
   OUT VOID                   *Buffer
   );

/**
   Write BufferSize bytes from Lba into Buffer.

   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    The media ID that the write request is for.
   @param  Lba        The starting logical block address to be
                      written. The   caller is responsible for
                      writing to only legitimate locations.
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the source buffer for the data.

   @retval EFI_SUCCESS           The data was written correctly to the device.
   @retval EFI_WRITE_PROTECTED   The device can not be written to.
   @retval EFI_DEVICE_ERROR      The device reported an error
                                 while performing the write.
   @retval EFI_NO_MEDIA          There is no media in the
           device.
   @retval EFI_MEDIA_CHNAGED     The MediaId does not matched
                                 the current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple
                                 of the block size of the device.
   @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not
                                 valid or the buffer is not on proper
                                 alignment.
**/
EFI_STATUS EFIAPI SpiNorWriteBlocks (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN UINT32                  MediaId,
   IN EFI_LBA                 Lba,
   IN UINTN                   BufferSize,
   IN VOID                    *Buffer
   );

/**
   Flush the Block Device.

   @param  This              Indicates a pointer to the calling context.

   @retval EFI_SUCCESS       All outstanding data was written to the device
   @retval EFI_DEVICE_ERROR  The device reported an error while
                             writting back the data
   @retval EFI_NO_MEDIA      There is no media in the device.

**/
EFI_STATUS EFIAPI SpiNorFlushBlocks (
   IN EFI_BLOCK_IO_PROTOCOL  *This
   );

/**
  Erase blocks in the Block Device

  If EFI_DEVICE_ERROR, EFI_NO_MEDIA,_EFI_WRITE_PROTECTED or EFI_MEDIA_CHANGED
  is returned and non-blocking I/O is being used, the Event associated with
  this request will not be signaled.

  @param[in]      This     Indicates a pointer to the calling context.
  @param[in,out]  Token    A pointer to the token associated with the transaction

  @retval EFI_SUCCESS          The flush request was queued if Event is not NULL.
                               All outstanding data was written correctly to the
                               device if the Event is NULL.
  @retval EFI_DEVICE_ERROR     The device reported an error while writting back
                               the data.
  @retval EFI_WRITE_PROTECTED  The device cannot be written to.
  @retval EFI_NO_MEDIA         There is no media in the device.
  @retval EFI_MEDIA_CHANGED    The MediaId is not for the current media.
  @retval EFI_OUT_OF_RESOURCES The request could not be completed due to a lack
                               of resources.
**/
EFI_STATUS EFIAPI SpiNorEraseBlocks (
   IN     EFI_BLOCK_IO_PROTOCOL *This,
   IN     UINT32 MediaId,
   IN     EFI_LBA LBA,
   IN OUT EFI_ERASE_BLOCK_TOKEN *Token,
   IN     UINTN Size
   );


#define SPINOR_DEV_FROM_BLOCKIO(a)   CR (a, SPINOR_DEV, BlkIo, SPINOR_DEV_SIGNATURE)
#define SPINOR_DEV_FROM_ERASE_BLOCK(a) CR (a, SPINOR_DEV, EraseBlk, SPINOR_DEV_SIGNATURE)

#define EFI_SPINOR_GUID  \
    /* gEfiSpiNorGuid */ \
    { 0x7cce9c94, 0x983f, 0x4d0a, { 0x81, 0x43, 0xb6, 0xc0, 0x55, 0x45, 0xb2, 0x23 } }

#endif /* _SPINOR_H_ */

