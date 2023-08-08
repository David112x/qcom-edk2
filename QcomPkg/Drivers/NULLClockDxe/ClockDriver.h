#ifndef __CLOCKDRIVER_H__
#define __CLOCKDRIVER_H__
/*
===========================================================================
*/
/**
  @file ClockDriver.h

  Internal header file for the clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010-2019 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/boot.xf/3.2/QcomPkg/Drivers/NULLClockDxe/ClockDriver.h#6 $
  $DateTime: 2019/07/22 14:57:35 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h"
#include "DALSys.h"
#ifdef CLOCK_ULOG_STUB
  #include "ClockULogStub.h"
#else
  #include "ULogFront.h"
#endif
#include "Include/ClockDefs.h"
#include "ClockBSP.h"
#include "voltage_level.h"
#ifdef CLOCK_NPA_STUB
  #include "ClockNPAStub.h"
#else
  #include "npa.h"
  #include "npa_resource.h"
#endif
#include "CoreVerify.h"


/*=========================================================================
      Warnings to ignore
==========================================================================*/

#ifndef __GNUC__
#pragma diag_suppress 188
#endif


/*=========================================================================
      Macro Definitions
==========================================================================*/

/**
 * Define __weak for non-rvct compilers.
 */
#ifdef __GNUC__
  #ifdef UEFI64
    #define __weak
  #else
    #define __weak __attribute__((weak))
  #endif
#elif !defined(__ARMCC_VERSION)
  #define __weak
#endif


/**
 * Macro lock the driver mutex.
 */
#define DALCLOCK_LOCK(drvCtxt) (drvCtxt->hLock) ? DALSYS_SyncEnter(drvCtxt->hLock) : (void)drvCtxt->hLock


/**
 * Macro try-lock the driver mutex.
 */
#define DALCLOCK_TRYLOCK(drvCtxt) (drvCtxt->hLock) ? DALSYS_SyncTryEnter(drvCtxt->hLock) : DAL_SUCCESS


/**
 * Macro to free the driver mutex.
 */
#define DALCLOCK_FREE(drvCtxt) (drvCtxt->hLock) ? DALSYS_SyncLeave(drvCtxt->hLock) : (void)drvCtxt->hLock


/**
 * Check if source is controlled by an NPA resource.
 */
#define CLOCK_SOURCE_NPA_CONTROLLED(src) \
  ((src)->szName && (src)->szName[0] == '/')


/**
 * NPA client name used for internal clock driver clients.
 */
#define CLOCK_NPA_CLIENT_NAME  "clock"


/**
 * Convert an NPA state to a frequency in Hz.
 */
#define NPA_TO_HZ(state) \
  (uint32)((state) == NPA_MAX_STATE ? (state) : (state) * 1000)


/*
 * Macros for checking, setting and clearing the unified flag bits in the
 * driver nodes.  Note these work on any structure containing an "nFlags"
 * field.
 */
#define CLOCK_FLAG_IS_SET(ptr, flag) (((ptr)->nFlags & CLOCK_FLAG_##flag) ? TRUE : FALSE)
#define CLOCK_FLAG_SET(ptr, flag)    ((ptr)->nFlags |= CLOCK_FLAG_##flag)
#define CLOCK_FLAG_CLEAR(ptr, flag)  ((ptr)->nFlags &= ~CLOCK_FLAG_##flag)


/*
 * Macros for checking, setting and clearing the global flag bits in the
 * driver context.  Note these work only on the "pBSP->nFlags" field
 * within the driver context structure.
 */
#define CLOCK_GLOBAL_FLAG_IS_SET(flag) ((pDrvCtxt->pBSP->nFlags & CLOCK_GLOBAL_FLAG_##flag) ? TRUE : FALSE)
#define CLOCK_GLOBAL_FLAG_SET(flag)    (pDrvCtxt->pBSP->nFlags |= CLOCK_GLOBAL_FLAG_##flag)
#define CLOCK_GLOBAL_FLAG_CLEAR(flag)  (pDrvCtxt->pBSP->nFlags &= ~CLOCK_GLOBAL_FLAG_##flag)

/*
 * Macro to check if a clock is stubbed on the current running platform.
 */
#define CLOCK_STUBBED(ptr) \
  ( \
    (boolean)(ptr->nFlags & CLOCK_FLAG_STUB_HW_RUMI) \
  )

/*
 * Values used for making NPA requests on clock sources.
 *  0:          Everything related to XO can be turned off.
 *  0xFFFFFFFF: Everything possible in the chain (crystal to pad) must be on.
 */
#define CLOCK_SOURCE_NOT_REQUIRED     0
#define CLOCK_SOURCE_REQUIRED         0xFFFFFFFF


/*
 * Default size of the BIST ULOG log buffer.
 *
 * NOTE: The log size should be specified in powers of 2.
 */
#define CLOCK_BIST_DEFAULT_LOG_SIZE   0x200000

/*
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define CLOCK_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF

/*
 * MAX and MIN macros.
 */
#ifndef MAX
  #define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
  #define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif


/*
 * Clock ID macros.
 */
#define CLOCK_MAKE_ID(cat, idx)   (((cat) << 16) | (idx))
#define CLOCK_GET_ID_CATEGORY(id) (((id) & 0xFFFF0000) >> 16)
#define CLOCK_GET_ID_INDEX(id)    ((id) & 0xFFFF)
#define CLOCK_ID_INVALID          0

#define CLOCK_XO_NAME             "/arc/client/rail_xo"


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * ClockIdCategoryType
 *
 * Clock ID categories.
 */
typedef enum
{
  CLOCK_ID_CATEGORY_NONE = 0,
  CLOCK_ID_CATEGORY_CLOCK,
  CLOCK_ID_CATEGORY_POWERDOMAIN,
  CLOCK_ID_CATEGORY_SOURCE,
  CLOCK_ID_CATEGORY_DEBUGMUX,
  CLOCK_ID_CATEGORY_DIVIDER,
  CLOCK_ID_CATEGORY_CLOCK_DOMAIN,

  CLOCK_ID_CATEGORY_TOTAL,
} ClockIdCategoryType;


/**
 * Clock driver context.
 */
typedef struct
{
  ClockBSPType      *pBSP;                    /**< Main clock driver BSP data. */
  DALSYSSyncObj      LockObj;                 /**< Sync object storage */
  DALSYSSyncHandle   hLock;                   /**< Handle to our mutex */
  ULogHandle         hClockLog;               /**< Handle to our ULOG log */
  ULogHandle         hClockBISTLog;           /**< Handle to our BIST ULOG log */
  boolean            bBISTEnabled;            /**< Flag used to enable the BIST */
  uint32             nBISTLogSize;            /**< Size of the BIST ULOG log */
  ClockNameListType *pBISTExceptions;         /**< Pointer to the BIST Exceptions List */
  ULogHandle         hClockDebugLog;          /**< Handle to our Debug ULOG log */
  boolean            bDCVSEnabled;            /**< DCVS state */
  boolean            bNPADisabled;            /**< NPA state */
  void              *pImageCtxt;              /**< Image context */
  boolean            bSuspendVoltageRequest;  /**< Voltage suspension state */
  boolean            bQdssEnabled;            /**< QDSS enabled status */
  boolean            bColdStart;              /**< FALSE on wake up from hibernate */
  uint32             nGlobalPlatformInfo;     /**< Global Platform info for the driver */
} ClockDrvCtxt;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : Clock_DriverInit
** =========================================================================*/
/**
  Initialize the clock driver.

  This function initializes the clock driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/
DALResult Clock_DriverInit
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_DriverDeInit
** =========================================================================*/
/**
  De-initialize the clock driver.

  This function shuts-down the clock driver.  It is not expected to ever
  be called.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverDeInit
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_DeInitTarget
** =========================================================================*/
/**
  De-initialize the Target context data.

  This function clears out existing Target context data.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- DeInit was successful.

  @dependencies
  None.
*/

DALResult Clock_DeInitTarget
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitBases
** =========================================================================*/
/**
  Initialize clock register bases.

  This function initializes register bases used by the clock driver. It maps
  each address used by the driver to its virtual representation.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitBases
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitTarget
** =========================================================================*/
/**
  Initialize the target specific portion of the clock driver.

  This function initializes the portion of the clock driver that is specific
  to the image and chipset we are running on.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitTarget
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitDCVS
** =========================================================================*/
/**
  Initialize the NPA DCVS nodes for this image in the clock driver.

  This function initializes the DCVS module for the current image.  This
  will define and create the required NPA nodes.  Nodes will be be
  active until DalClock_EnableDCVS has been called.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitDCVS
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_InitNPA
** =========================================================================*/
/**
  Initialize the generic NPA resources.

  This function initializes the generic NPA resources that control domain
  frequencies.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitNPA
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_TestClock
** =========================================================================*/
/**
  Perform a non-destructive built-in self test on the specified clock node
  in the driver.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param ClockId [in] -- Clock ID of the clock to be tested.

  @return
  DAL_SUCCESS -- BIST was successful.
  DAL_ERROR -- BIST failed.

  @dependencies
  Core and image initialization must be complete prior to running the test.
*/

DALResult Clock_TestClock
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock
);


/* =========================================================================
**  Function : Clock_InitBISTLog
** =========================================================================*/
/**
  Initializes the BIST log.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- BIST log initialization was successful.
  DAL_ERROR -- BIST log initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitBISTLog
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_BIST
** =========================================================================*/
/**
  Perform a non-destructive built-in self test on all clock nodes in the
  driver.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- BIST was successful.
  DAL_ERROR -- BIST failed.

  @dependencies
  Core and image initialization must be complete prior to running the BIST.
*/

DALResult Clock_BIST
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_FindClockConfig
** =========================================================================*/
/**
  Finds a particular clock configuration in the BSP.

  This function finds a particular clock configuration in the BSP based on
  the specified frequency and match criteria.

  @param *pDomain[in] -- Pointer to a clock domain.
  @param nFreqHz [in] -- Frequency in Hz
  @param eMatch [in] -- Match criteria
  @param **pMatchingConfig [out] -- Matching configuration

  @return
  DAL_SUCCESS -- A matching configuration was found.
  DAL_ERROR -- A matching configuration was not found.

  @dependencies
  None.
*/

DALResult Clock_FindClockConfig
(
  ClockDomainNodeType  *pDomain,
  uint32                nFreqHz,
  ClockFrequencyType    eMatch,
  ClockMuxConfigType  **pMatchingConfig
);


/* =========================================================================
**  Function : Clock_SetClockConfig
** =========================================================================*/
/**
  Sets a clock configuration.

  This function configures a clock domain to a particular configuration.
  It is used internally only.

  @param *pDrvCtxt [in] -- Driver context.
  @param *pClockDomain [in] -- Clock domain to configure.
  @param *pNewConfig [in] -- New configuration to use.

  @return
  DAL_SUCCESS -- The domain was successfully configured.
  DAL_ERROR -- The domain configuration failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_SetClockConfig
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pDomain,
  ClockMuxConfigType  *pNewConfig
);


/* =========================================================================
**  Function : Clock_EnableSourceInternal
** =========================================================================*/
/**
  Requests to enable a source.

  This function enables a source, generally as a result of a call to
  Clock_EnableClockDomain().  CPU based source requests are tracked
  independently of non-CPU based as the former will be automatically
  released when the CPU goes to sleep.
  A reference count is maintained so callers must ensure that all Enables
  and Disables are matched.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param bSuppressibleRequest [in] -- TRUE if this is a source request is for a suppressible clock.

  @return
  DAL_SUCCESS -- The source was successfully enabled.
  DAL_ERROR -- The source was not enabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_EnableSourceInternal
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bSuppressibleRequest
);


/* =========================================================================
**  Function : Clock_DisableSourceInternal
** =========================================================================*/
/**
  Disables a source.

  This function disables a source, generally as a result of a call to
  Clock_DisableClockDomain().
  A reference count is maintained so callers must ensure that all Enables
  and Disables are matched.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param bSuppressibleRequest [in] -- TRUE if this is a source request is for a suppressible clock.
  @param  bDoNotDisable [in]        -- TRUE if this is a source request that should not disable the source.

  @return
  DAL_SUCCESS -- The source was successfully disabled.
  DAL_ERROR -- The source was not disabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_DisableSourceInternal
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  boolean              bSuppressibleRequest,
  boolean              bDoNotDisable
);


/* =========================================================================
**  Function : Clock_FindSourceConfig
** =========================================================================*/
/**
  Finds a particular source configuration in the BSP.

  This function finds a particular source configuration in the BSP based on
  the specified frequency and match criteria.

  @param *aBSP [in] -- Pointer to a BSP data table.
  @param nBSPLen [in] -- BSP data table len.
  @param nFreqHz [in] -- Frequency in Hz
  @param eMatch [in] -- Match criteria
  @param **pMatchingConfig [out] -- Matching configuration

  @return
  DAL_SUCCESS -- A matching configuration was found.
  DAL_ERROR -- A matching configuration was not found.

  @dependencies
  None.
*/
DALResult Clock_FindSourceConfig
(
  const ClockSourceFreqConfigType  *aBSP,
  uint32                            nBSPLen,
  uint32                            nFreqHz,
  ClockFrequencyType                eMatch,
  const ClockSourceFreqConfigType **pMatchingConfig
);


/* =========================================================================
**  Function : Clock_ConfigSource
** =========================================================================*/
/**
  Configures a source.

  This function configures a source, generally a PLL.  This can only be
  done if there are no users of the source and we own the PLL.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.
  @param *pConfig [in] -- New configuration.

  @return
  DAL_SUCCESS -- The source was successfully configures.
  DAL_ERROR -- The source was not configured, either due to invalid
               parameter or (more likely) the source was in use.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_ConfigSource
(
  ClockDrvCtxt                     *pDrvCtxt,
  ClockSourceNodeType              *pSource,
  const ClockSourceFreqConfigType  *pConfig
);


/* =========================================================================
**  Function : Clock_DetectSourceConfig
** =========================================================================*/
/**
  Detect a source configuration.

  This function detects a source configuration by reading the HW state
  and matching it with the BSP data. It will set the various bookkeeping
  fields to match the detected configuration.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pSource [in] -- Pointer to a source node.

  @return
  DAL_SUCCESS -- The source configuration was successfully detected.
  DAL_ERROR -- The source configuration was not found.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_DetectSourceConfig
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource
);


/* =========================================================================
**  Function : Clock_InitSource
** =========================================================================*/
/**
  Configures a source.

  This function initializes a source, generally a PLL, from the current BSP
  data.

  @param *pDrvCtxt          [in] -- Handle to the DAL driver context.
  @param *pSource           [in] -- Pointer to a source node.
  @param *pSourceFreqConfig [in] -- Optional frequency to init this source to.
                                    The first source frequency config is used if
                                    this argument is NULL.

  @return
  DAL_SUCCESS -- The source was successfully initialized.
  DAL_ERROR -- The source was not initialized, generally because the source
               has no valid BSP data.

  @dependencies
  The clock mutex should be locked prior to calling this function or
  we should be a single execution thread environment (like boot).
*/

DALResult Clock_InitSource
(
  ClockDrvCtxt                    *pDrvCtxt,
  ClockSourceNodeType             *pSource,
  const ClockSourceFreqConfigType *pSourceFreqConfig
);


/* =========================================================================
**  Function : Clock_MapHWIORegion
** =========================================================================*/
/**
  Map a HWIO region into memory for the clock driver.

  This function maps a physical HWIO region into memory for the clock driver
  using environment appropriate APIs.  If the mapping is not available
  the virtual address will be set to the provided physical address.

  @param nPhysAddr [in] -- Physical address.
  @param nSizeBytes [in] -- Size of the region in bytes.
  @param *pnVirtAddr [out] -- Location to store the mapped address.

  @return
  DAL_SUCCESS if mapping was ok, otherwise error code based on failure.

  @dependencies
  None.
*/

DALResult Clock_MapHWIORegion
(
  uintnt  nPhysAddr,
  uint32  nSizeBytes,
  uintnt *pnVirtAddr
);


/* =========================================================================
**  Function : Clock_MapAddress
** =========================================================================*/
/**
  Map a single address to its virtual representation.

  This function maps a single address to its virtual representation.

  @param *pDrvCtxt [in]   -- Handle to the DAL driver context.
  @param *pnAddr [in/out] -- Pointer to physical address to map. Updated
                             to mapped virtual address.

  @return
  DAL_SUCCESS if mapping was ok, otherwise error code based on failure.

  @dependencies
  None.
*/

DALResult Clock_MapAddress
(
  ClockDrvCtxt *pDrvCtxt,
  uintnt       *pnAddr
);


/* =========================================================================
**  Function : Clock_InitVoltage
** =========================================================================*/
/**
  Initialize voltage tracking subsystem.

  This function prepares the voltage tracking module for managing voltage
  requests.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_InitVoltage
(
  ClockDrvCtxt *pDrvCtxt
);


/* =========================================================================
**  Function : Clock_VoltageRequest
** =========================================================================*/
/**
  Make a voltage regulator request.

  This function is used internally for a clock domain to make a voltage
  regulator level request.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pRail [in] -- Pointer to the desired voltage rail.
  @param *pCurrentRequest [in/out] -- Pointer to currently requested voltage config.
                                      Updated to pNewRequest.
  @param *pNewRequest [in] -- New voltage requirement.

  @return
  DAL_SUCCESS -- Request was successful.
  DAL_ERROR -- Request failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_VoltageRequest
(
  ClockDrvCtxt          *pDrvCtxt,
  ClockRailType         *pRail,
  ClockVRegRequestType  *pCurrentRequest,
  ClockVRegRequestType  *pNewRequest
);


/* =========================================================================
**  Function : Clock_VoltageSuspend
** =========================================================================*/
/**
  Make a request to suspend voltage requests.

  This function is used internally to suspend voltage requests.

  @param *pRail [in] -- Specifies the voltage rail.

  @return
  DAL_SUCCESS -- Request was successful.
  DAL_ERROR   -- Request failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_VoltageSuspend
(
  ClockRailType *pRail
);


/* =========================================================================
**  Function : Clock_VoltageResume
** =========================================================================*/
/**
  Make a request to resume voltage requests.

  This function is used internally to resume voltage requests.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param *pRail  [in] -- Specifies the voltage rail.
  @param bUpdate [in] -- Flag to update voltage request immediately upon resume.

  @return
  DAL_SUCCESS -- Request was successful.
  DAL_ERROR   -- Request failed.

  @dependencies
  The clock mutex must be locked.
*/

DALResult Clock_VoltageResume
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockRailType *pRail,
  boolean        bUpdate
);


/*=========================================================================
**  Function : Clock_IsBSPSupported
** =========================================================================*/
/**

  This function validates the specified BSP config is supported on the
  executing target chip and version.

  @param pHWVersion [in] - HW Version

  @return
  TRUE - HW version is supported.
  FALSE - HW version is not supported.

  @dependencies
  None.
*/

boolean Clock_IsBSPSupported
(
  const ClockHWVersionType *pHWVersion
);


/*=========================================================================
**  Function : Clock_GetDrvCtxt
** =========================================================================*/
/**
  Returns driver context.

  This function is used to get the driver context when called from the lite
  API call path.  The mutex shared with the lite API is taken.


  @return
  Pointer to clock driver context.

  @dependencies
  None.
*/

ClockDrvCtxt *Clock_GetDrvCtxt
(
  void
);


/*=========================================================================
**  Function : Clock_GetMappedSource
** =========================================================================*/
/**
  Map mux select value to internal source node.

  @return
  DAL_SUCCESS -- Mapped source was found.
  DAL_ERROR -- No mapped source found.

  @dependencies
  None.
*/

DALResult Clock_GetMappedSource
(
  const ClockSourceMapType   *aSourceMap,
  uint32                      nMuxSel,
  const ClockSourceMapType  **pMatchingSourceMap
);


/*=========================================================================
**  Function : Clock_GetRequiredCorner
** =========================================================================*/
/**
  Get required corner for frequency.

  @return
  DAL_SUCCESS -- Required corner found.
  DAL_ERROR -- No suitable corner found.

  @dependencies
  None.
*/

DALResult Clock_GetRequiredCorner
(
  const ClockFMaxType *aFMax,
  uint32               nFreqHz,
  rail_voltage_level  *peVRegLevel
);


/*=========================================================================
**  Function : Clock_GetMaxDomainCorner
** =========================================================================*/
/**
  Get maximum corner supported by domain.

  @return
  DAL_SUCCESS -- Max corner found.
  DAL_ERROR -- No suitable corner found.

  @dependencies
  None.
*/
DALResult Clock_GetMaxDomainCorner
(
  ClockDomainNodeType *pDomain,
  rail_voltage_level  *peVRegLevel
);


/*=========================================================================
**  Function : Clock_AssignNPAState
** =========================================================================*/
/**
  Invoke an NPA driver function and assign the resource state.

  This function calls an NPA node driver function with nRequest, and
  assigns the resource state to the return value. This function bypasses
  the NPA aggregation layer and can be used to override the current request
  state of a node.

  @param *pResource [in] -- Pointer to NPA resource to assign.
  @param nRequest [in] -- New request state.

  @return
  npa_resource_state -- Active state of the resource.

  @dependencies
  The NPA node pointed to by pResource must have already been created.
*/

npa_resource_state Clock_AssignNPAState
(
  npa_resource        *pResource,
  npa_resource_state   nRequest
);


/*=========================================================================
**  Function : Clock_NormalizeChipInfo
** =========================================================================*/
/**

  This is an optional function used to normalize ChipInfo when the official
  chip info data makes no sense due to marketing considerations.

  @param HALClkCtxt [in/out] - Pointer to HAL context containing the official
                               Chip Info data

  @return
  None.

  @dependencies
  None.
*/

void Clock_NormalizeChipInfo
(
  HAL_clk_ContextType *HALClkCtxt
);


/*=========================================================================
**  Function : Clock_GetClockNode
** =========================================================================*/
/**
  Return pointer to clock node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Clock identifier to look up.

  @return
  ClockNodeType* if found; NULL otherwise.
*/

ClockNodeType *Clock_GetClockNode
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nId
);


/*=========================================================================
**  Function : Clock_GetSourceNode
** =========================================================================*/
/**
  Return pointer to source node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Source identifier to look up.

  @return
  ClockSourceNodeType* if found; NULL otherwise.
*/

ClockSourceNodeType *Clock_GetSourceNode
(
  ClockDrvCtxt  *pDrvCtxt,
  SourceIdType   nId
);


/*=========================================================================
**  Function : Clock_GetPowerDomainNode
** =========================================================================*/
/**
  Return pointer to power domain node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Power domain identifier to look up.

  @return
  ClockPowerDomainNodeType* if found; NULL otherwise.
*/

ClockPowerDomainNodeType *Clock_GetPowerDomainNode
(
  ClockDrvCtxt            *pDrvCtxt,
  ClockPowerDomainIdType   nId
);


/*=========================================================================
**  Function : Clock_GetDebugMuxNode
** =========================================================================*/
/**
  Return pointer to debug mux node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Debug mux identifier to look up.

  @return
  ClockDebugMuxType* if found; NULL otherwise.
*/

ClockDebugMuxType *Clock_GetDebugMuxNode
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDebugMuxIdType  nId
);


/*=========================================================================
**  Function : Clock_GetDividerNode
** =========================================================================*/
/**
  Return pointer to clock node for a given ID.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param nId[in] -- Divider identifier to look up.

  @return
  ClockDividerNodeType* if found; NULL otherwise.
*/

ClockDividerNodeType *Clock_GetDividerNode
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDividerIdType   nId
);


/*=========================================================================
      NPA wrapper functions.
==========================================================================*/

void clock_npa_issue_scalar_request
(
  npa_client_handle   client,
  npa_resource_state  state
);

/*=========================================================================
**  Function : Clock_GetMaxPerfLevel
** =========================================================================*/
/*
  Returns the maximum defined performance level for the processor clock.

  This function returns the maximum defined processor performance level.
  Each performance level corresponds to a unique clock/voltage pair.

  Parameters Description:

  @param nCPU        [in]  -- Specifies the CPU core/cluster of interest.
  @param pnPerfLevel [out] -- Pointer to fill in with the maximum
                              performance level.

  Return Value:

  @return
  DAL_SUCCESS -- Valid configuration is available, performance level
                 filled in.
  DAL_ERROR   -- Valid configuration is not available.

*/

DALResult
Clock_GetMaxPerfLevel (
  uint32 nCPU,
  uint32 *pnPerfLevel
);


/* =========================================================================
**  Function : Clock_GetMinPerfLevel
** =========================================================================*/
/*
  Returns the minimum defined performance level for the processor clock.

  This function returns the minimum defined processor performance level.
  Each performance level corresponds to a unique clock/voltage pair.

  Parameters Description:

  @param nCPU        [in]  -- Specifies the CPU core/cluster of interest.
  @param pnPerfLevel [out] -- Pointer to fill in with the minimum
                              performance level.

  Return Value:

  @return
  DAL_SUCCESS -- Valid configuration is available, performance level
                 filled in.
  DAL_ERROR   -- Valid configuration is not available.

*/

DALResult Clock_GetMinPerfLevel
(
  uint32 nCPU,
  uint32 *pnPerfLevel
);


/* =========================================================================
**  Function : Clock_GetCPUFrequencyLevels
** =========================================================================*/
/*
  Returns the number of defined performance level for the specified CPU.

  This function returns the number of defined processor performance level.
  Each performance level corresponds to a unique clock/voltage pair.

  Parameters Description:

  @param nCPU              [in]  -- Specifies the CPU core/cluster of interest.
  @param pnFrequencyLevels [out] -- Pointer to the number of frequency levels.

  Return Value:

  @return
  DAL_SUCCESS -- Valid configuration is available, performance level
                 filled in.
  DAL_ERROR   -- Valid configuration is not available.

*/
DALResult
Clock_GetCPUFrequencyLevels (
  uint32 nCPU,
  uint32 *pnFrequencyLevels 
);


/* =========================================================================
**  Function : Clock_GetCPUFrequencyPlan
** =========================================================================*/
/*
  Dumps the frequency plan for the specified CPU in the Table passed as pointer.

  This function dumps the frequency plan in the table pointer passed for the specified CPU.

  Parameters Description:

  @param nCPU              [in]  -- Specifies the CPU core/cluster of interest.
  @param pCPUFreqPlan      [in]  -- Pointer to the table storing the frequency plan of the corresponding CPU.
  @param nSize             [in]  -- Size of the Frequency plan table, expected value to be: (No. of Perf Levels * sizeof(ClockFreqPlanType))

  Return Value:

  @return
  DAL_SUCCESS -- Valid configuration is available, performance level
                 filled in.
  DAL_ERROR   -- Valid configuration is not available.

*/
DALResult
Clock_GetCPUFrequencyPlan (
  uint32             nCPU,
  ClockFreqPlanType  *pCPUFreqPlan,
  uint32             nSize
);


/* =========================================================================
**  Function : Clock_SetCPUFrequency
** =========================================================================*/
/*
  Changes the processor clock speed and voltage.

  This function changes the performance level of the specified CPU to the
  specified frequency and the corresponding voltage level.

  Parameters Description:

  @param nCPU             [in]  -- Specifies the CPU core/cluster of interest.
  @param nFrequencyHz     [in]  -- User requested frequency.
  @param pnResultFreqHz   [out]  -- Pointer to the actual frequency achieved.(It is an optional parameter)

  Return Value:

  @return
  DAL_SUCCESS -- Valid configuration is available, performance level
                 filled in.
  DAL_ERROR   -- Valid configuration is not available.

*/
DALResult
Clock_SetCPUFrequency (
  uint32 nCPU,
  uint32 nFrequencyHz,
  uint32 *pnResultFreqHz
);

/* =========================================================================
**  Function : Clock_GetCPUFrequency
** =========================================================================*/
/*
  Returns the current clock speed in Hz of the given processor.

  This function returns the current CPU frequency.

  Parameters Description:

  @param nCPU             [in]  -- Specifies the CPU core/cluster of interest.
  @param pnFrequencyHz    [out] -- Pointer to fill in with the corresponding
                                    frequency in Hz.

  Return Value:

  @return
  DAL_SUCCESS -- Valid configuration is available, performance level
                 filled in.
  DAL_ERROR   -- Valid configuration is not available.

*/
DALResult
Clock_GetCPUFrequency (
  uint32 nCPU,
  uint32 *pnFrequencyHz
);

/* =========================================================================
**  Function : Clock_GetCpuPerfLevelFrequency
** =========================================================================*/
/*
  Returns clock speed in Hz and required voltage in mV that corresponds to
  the given perfomance level.

  This function returns clock speed in Hz and required voltage in mV that
  corresponds to the given perfomance level.

  Parameters Description:

  @param nCPU              [in]  -- Specifies the CPU core of interest.
  @param nPerfLevel        [in]  -- Specifies the performance level of interest.
  @param pnFrequencyHz     [out] -- Pointer to fill in with the corresponding
                                    frequency in Hz.
  @param pnRequiredVoltage [out] -- Pointer to fill in with the corresponding
                                    required voltage in mV.

  Return Value:

  @return
  DAL_SUCCESS -- Valid configuration is available, frequency and voltage filled in.
  DAL_ERROR   -- Valid configuration is not available.

*/

DALResult
Clock_GetCpuPerfLevelFrequency (
  uint32 nCPU,
  uint32 nPerfLevel,
  uint32 *pnFrequencyHz,
  uint32 *pnRequiredVoltage
);


/* =========================================================================
**  Function : Clock_SetCpuPerfLevel
** =========================================================================*/
/*
  Changes the processor clock speed and voltage.

  This function changes the performance level of the specified CPU to the
  specified frequency and the corresponding voltage level.

  Parameters Description:

  @param nCPU          [in]  -- Specifies the CPU core of interest.
  @param nPerfLevel    [in]  -- Specifies the desired performance level.
  @param pnFrequencyHz [out] -- Pointer to the actual CPU frequency in Hz.

  Return Value:

  @return
  DAL_SUCCESS -- The desired performance level has been successfully set.
  DAL_ERROR   -- Invalid parameters, or configuration.

*/

DALResult
Clock_SetCpuPerfLevel (
  uint32 nCPU,
  uint32 nPerfLevel,
  uint32 *pnFrequencyHz
);


/* =========================================================================
**  Function : Clock_GetCpuPerfLevel
** =========================================================================*/
/*
  Returns the current performance level of the given processor.

  This function retrieves the performance level of the specified CPU and
  returns that value to the caller.

  Parameters Description:

  @param nCPU        [in]  -- Specifies the CPU core of interest.
  @param pnPerfLevel [out] -- Pointer to the current performance level.

  Return Value:

  @return
  DAL_SUCCESS -- The current performance level value has been
                 successfully retrieved.
  DAL_ERROR   -- Unable to retrieve the current performance level
                 value.
*/

DALResult
Clock_GetCpuPerfLevel (
  uint32 nCPU,
  uint32 *pnPerfLevel
);


/*=========================================================================
      Function Definitions From DDIClock.h
==========================================================================*/

DALResult Clock_GetClockId( ClockDrvCtxt *pDrvCtxt,  const char * szClock,  ClockIdType * pnId);
DALResult Clock_EnableClock( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock);
DALResult Clock_DisableClock( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock);
DALResult Clock_SetClockFrequency( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  uint32  nFreq,  ClockFrequencyType  eMatch, uint32 *pnResultFreq );
DALResult Clock_SetClockDivider( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  uint32  nDivider);
DALResult Clock_InvertClock( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  boolean  bInvert);
DALResult Clock_ResetClock( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  ClockResetType  eReset);
DALResult Clock_ResetClock_STM( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  ClockResetType  eReset);
DALResult Clock_ResetClockBranch(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockResetType eReset);
DALResult Clock_ConfigClock(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockConfigType eConfig);
DALResult Clock_IsClockEnabled( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  boolean * pbIsOn);
DALResult Clock_IsClockOn( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  boolean * pbIsOn);
DALResult Clock_GetClockFrequency( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  uint32 * pnFrequencyHz);
DALResult Clock_GetClockData( ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, uint32 nFreqLevel, ClockRequestDataType eRequestType, uint32 * pnData);
DALResult Clock_ShowClockFrequencyAndVoltage( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  uint32  nFreq,  ClockFrequencyType eMatch, uint32 *pnResultFreq, uint32 *pnResultVoltage );
DALResult Clock_IsClockXOVote( ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, boolean bCurrently, boolean * pbIsXOVote);
DALResult Clock_GetClockRefCount(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, uint32 *pnRefCount);
DALResult Clock_GetPowerDomainRefCounts(ClockDrvCtxt *pDrvCtxt, ClockPowerDomainNodeType *pPowerDomain, uint32 *pnRefCount, uint32 *pnRefCountHW);
DALResult Clock_SelectExternalSource( ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, uint32 nFreqHz, uint32 nSource, uint32 nDiv2x, uint32 nM, uint32 nN, uint32 n2D);
DALResult Clock_CalcClockFrequency( ClockDrvCtxt *pDrvCtxt,  ClockNodeType *pClock,  uint32 * pnFrequencyHz);
DALResult Clock_LogState ( ClockDrvCtxt *pDrvCtxt, ULogHandle hULog, uint32 nFlags );
DALResult Clock_GetFrequencyPlan ( ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockFreqPlanType **aFreqPlan, uint32 *nSize );
DALResult Clock_GetClockInfo ( ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockInfoType *pClockInfo );

DALResult Clock_GetDebugMuxId(ClockDrvCtxt *pDrvCtxt, const char *szDebugMux, ClockDebugMuxIdType *pnId);
DALResult Clock_CalcFrequency(ClockDrvCtxt *pDrvCtxt, ClockDebugMuxType *pDebugMux, uint32 nMuxSel, uint32 *pnFrequencyHz);

DALResult Clock_GetSourceId( ClockDrvCtxt *pDrvCtxt,  const char * szSource,  SourceIdType * pnId);
DALResult Clock_EnableSource( ClockDrvCtxt *pDrvCtxt,  ClockSourceNodeType *pSource);
DALResult Clock_DisableSource( ClockDrvCtxt *pDrvCtxt,  ClockSourceNodeType *pSource);
DALResult Clock_SetSourceFrequency( ClockDrvCtxt *pDrvCtxt,  ClockSourceNodeType *pSource,  uint32  nFreq,  ClockFrequencyType  eMatch, uint32 *pnResultFreq );
DALResult Clock_GetSourceFrequency( ClockDrvCtxt *pDrvCtxt,  ClockSourceNodeType *pSource,  uint32 * pnFrequencyHz);
DALResult Clock_EnableDFS(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock);
DALResult Clock_IsDFSEnabled(ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, boolean *pbIsEnabled);

DALResult Clock_GetPowerDomainId( ClockDrvCtxt *pDrvCtxt,  const char * szPowerDomain,  ClockPowerDomainIdType * pnPowerDomainId);
DALResult Clock_EnablePowerDomain( ClockDrvCtxt *pDrvCtxt,  ClockPowerDomainNodeType *pPowerDomain);
DALResult Clock_DisablePowerDomain( ClockDrvCtxt *pDrvCtxt,  ClockPowerDomainNodeType *pPowerDomain);
DALResult Clock_EnablePowerDomainHW( ClockDrvCtxt *pDrvCtxt,  ClockPowerDomainNodeType *pPowerDomain);
DALResult Clock_DisablePowerDomainHW( ClockDrvCtxt *pDrvCtxt,  ClockPowerDomainNodeType *pPowerDomain);

DALResult Clock_GetDividerId(ClockDrvCtxt *pDrvCtxt, const char *szDivider,  ClockDividerIdType *pnId);
DALResult Clock_SetDivider(ClockDrvCtxt *pDrvCtxt, ClockDividerNodeType *pDivider, uint32 nDivider);

DALResult Clock_GetClockDebugId( ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, uint32 * pnDebugId );
DALResult Clock_GetClockDomainId( ClockDrvCtxt *pDrvCtxt, ClockNodeType *pClock, ClockDomainIdType * pnClockDomainId );

DALResult Clock_EnterLowPowerMode( ClockDrvCtxt *pDrvCtxt);
DALResult Clock_ExitLowPowerMode( ClockDrvCtxt *pDrvCtxt);

DALResult Clock_LogClocksState ( ClockDrvCtxt *pDrvCtxt, uint32 nFlags );
DALResult Clock_DumpClocksState ( ClockDrvCtxt *pDrvCtxt, uint32 nFlags );

DALResult Clock_GetAPCSPLLStatus ( ClockDrvCtxt *pDrvCtxt, ClockAppsClusterIdType eClusterId, boolean *bPLLEnable );

/*=========================================================================
      Stubbed out functions
==========================================================================*/

static inline DALResult Clock_LoadNV( ClockDrvCtxt *pDrvCtxt){return DAL_SUCCESS;}
static inline DALResult Clock_ProcessorSleep( ClockDrvCtxt *pDrvCtxt,  ClockSleepModeType  eMode,  uint32  nFlags){return DAL_SUCCESS;}
static inline DALResult Clock_ProcessorRestore( ClockDrvCtxt *pDrvCtxt,  ClockSleepModeType  eMode,  uint32  nFlags){return DAL_SUCCESS;}

void      __weak Clock_NormalizeChipInfo ( HAL_clk_ContextType *HALClkCtxt );

#endif /* !__CLOCK_DRIVER_H__ */


