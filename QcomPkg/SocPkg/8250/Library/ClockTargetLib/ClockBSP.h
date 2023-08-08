#ifndef __CLOCKBSP_H__
#define __CLOCKBSP_H__

/*=========================================================================

                    C L O C K   D E V I C E   D R I V E R
               
        B O A R D   S U P P O R T   P A C K A G E   D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains the BSP interface definitions for the
  MSM DAL Clock Device Driver.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

      Copyright (c) 2010-2015 by QUALCOMM Technologies, Inc.  All Rights Reserved.

==========================================================================*/

/*=========================================================================
      Includes
==========================================================================*/


#include "ClockDefs.h"
#include "HALclk.h"
#include "HALclkInternal.h"
#include "DDIChipInfo.h"
#include "voltage_level.h"
#include "npa.h"
#include "npa_resource.h"


/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/

#define SIZEOF_ARRAY(a) (sizeof((a))/sizeof((a)[0]))


/*
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define CLOCK_BSP_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF


/*
 * Macros for consolidating PLL configuration in a single mask.
 */
#define CLOCK_CONFIG_PLL_FSM_MODE_ENABLE        (1 << 0)
#define CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE      (1 << 1)
#define CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE     (1 << 2)
#define CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE    (1 << 3)
#define CLOCK_CONFIG_PLL_XO_DOUBLER_DCC         (1 << 4)
#define CLOCK_CONFIG_PLL_EVEN_OUTPUT_ENABLE     (1 << 5)
#define CLOCK_CONFIG_PLL_ODD_OUTPUT_ENABLE      (1 << 6)
#define CLOCK_CONFIG_PLL_MAIN_OUTPUT_HW_CTL     (1 << 7)
#define CLOCK_CONFIG_PLL_EVEN_OUTPUT_HW_CTL     (1 << 8)
#define CLOCK_CONFIG_PLL_ODD_OUTPUT_HW_CTL      (1 << 9)


/*
 * Common divider fields (*_CDIVR register)
 */
#define HAL_CLK_CDIVR_CLK_DIV_FMSK                                     0x0000000F
#define HAL_CLK_CDIVR_CLK_DIV_SHFT                                     0


/**
 * Unified flag bits
 *
 * NOTE: These flags can be used in any node that supports the
 * nFlags field.
 *
 *  LOG_STATE_CHANGE - Enable logging of enable/disable calls
 *    to a particular node.
 *  LOG_FREQ_CHANGE - Enable logging of frequency changes to
 *    a particular node.
 *  LOG_CONFIG_CHANGE - Enable logging of configuration changes
 *    to a particular node.
 *  LOG_RESET - Enable logging of resets to a particular node.
 *  DO_NOT_DISABLE - Ignore requests to disable a particular
 *    node.
 *  LOG_INVERT_CHANGE - Enables logging of clock inversion
 *    changes.
 *  SUPPRESSIBLE - Indicates XO shutdown will not be held off
 *    while the clock is on.
 *  DOMAIN_HOLD_VREG_REQUEST - Hold the voltage request even
 *    when the clock is off. When set this means the voltage
 *    request will be based solely on the configured clock
 *    frequency and will not be released when the clock is
 *    disabled.
 *  DOMAIN_DYNAMIC_CONFIG - Indicates if the mux configuration
 *    was set up dynamically so we free up the memory after
 *    switching to a new configuration.
 *  AUTOGATE - Allows hardware to automatically enable or
 *    disable the clock branch based on usage. When this
 *    flag is set, the CLK_ENABLE bit in the CBCR register
 *    is ignored.
 *  BIST_FAILURE_ANTICIPATED - Indicates that a clock is
 *    expected to fail the BIST. The BIST log will show
 *    that the clock failed, but that the failure is
 *    expected. The clock does not count towards the
 *    total number of failed clocks.
 *  INTERNAL_CONTROL - Indicates that this node (source/clock/etc)
 *    cannot be controlled by clients via the DAL API.
 *  INITIALIZED - Indicates that this node (source/clock/etc)
 *    has been initialized by the driver.
 *  STUB_HW_RUMI - Indicates that requests on this resource should
 *    avoid accessing HW on RUMI.
 *  CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF - Indicates that the SW should wait for 
 *    a root-off signal before disabling the clock source.
 *  EXPLICIT_ACCESS_ONLY - Indicates that this clock may not always be
 *    physically accessible and should only be access when a client explicitly
 *    requests it. Any internal functions that iterate over all clocks should
 *    skip these.
 *  READ_ONLY - Indicates that this clock is read-only and shouldn't be
 *    be configured by the driver.
 *  DFS_CONTROLLED - Indicates that this clock is being controlled by an
 *    enabled DFS instance.
 *  STUB_HW_VIRTIO - Indicates that requests on this resource should
 *    avoid accessing HW on Virtio.
 */
#define CLOCK_FLAG_LOG_STATE_CHANGE             0x00000001
#define CLOCK_FLAG_LOG_FREQ_CHANGE              0x00000002
#define CLOCK_FLAG_LOG_CONFIG_CHANGE            0x00000004
#define CLOCK_FLAG_LOG_RESET                    0x00000008
#define CLOCK_FLAG_LOG_INVERT_CHANGE            0x00000010
// Reserved for future logging                  0x00000020
// Reserved for future logging                  0x00000040
// Reserved for future logging                  0x00000080
#define CLOCK_FLAG_DO_NOT_DISABLE               0x00000100
#define CLOCK_FLAG_SUPPRESSIBLE                 0x00000200
#define CLOCK_FLAG_DOMAIN_HOLD_VREG_REQUEST     0x00000400
#define CLOCK_FLAG_DOMAIN_DYNAMIC_CONFIG        0x00000800
#define CLOCK_FLAG_AUTOGATE                     0x00001000
#define CLOCK_FLAG_BIST_FAILURE_ANTICIPATED     0x00002000
#define CLOCK_FLAG_INTERNAL_CONTROL             0x00004000
// Unused                                       0x00008000
#define CLOCK_FLAG_INITIALIZED                  0x00010000
// Unused                                       0x00020000
// Unused                                       0x00040000
#define CLOCK_FLAG_STUB_HW_RUMI                 0x00080000
#define CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF     0x00100000
#define CLOCK_FLAG_EXPLICIT_ACCESS_ONLY         0x00200000
#define CLOCK_FLAG_READ_ONLY                    0x00400000
#define CLOCK_FLAG_DFS_CONTROLLED               0x00800000
#define CLOCK_FLAG_STUB_HW_VIRTIO               0x01000000


/**
 * Global flag bits
 *
 * NOTE: These flags are to be used only in the nGlobalFlags
 * field of the driver context structure.
 *
 *  LOG_x_STATE_CHANGE - Enable logging of enable/disable
 *    calls to all clocks, sources, or power domains as
 *    specified by x.
 *  LOG_x_FREQ_CHANGE - Enable logging of frequency changes
 *    to all clocks, sources as specified by x.
 *  LOG_CLOCK_CONFIG_CHANGE - Enable logging of configuration
 *    changes to all clocks.
 *  LOG_CLOCK_RESET - Enable logging of resets to all clocks.
 *  LOG_VOLTAGE_CHANGE - Enable logging of vdd requirement changes.
 *  DO_NOT_DISABLE_x - Ignore requests to disable all clocks,
 *    sources, or power domains as specified by x.
 *  STUB_API - Stubs out entire API (DAL and NPA nodes expected).
 *  LOG_CLOCK_INVERT_CHANGE - Enables logging of clock inversion
 *    changes.
 */
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_STATE_CHANGE          0x00000001
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_FREQ_CHANGE           0x00000002
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_CONFIG_CHANGE         0x00000004
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_RESET                 0x00000008
#define CLOCK_GLOBAL_FLAG_LOG_SOURCE_STATE_CHANGE         0x00000010
#define CLOCK_GLOBAL_FLAG_LOG_POWER_DOMAIN_STATE_CHANGE   0x00000020
#define CLOCK_GLOBAL_FLAG_LOG_VOLTAGE_CHANGE              0x00000040
#define CLOCK_GLOBAL_FLAG_LOG_SOURCE_FREQ_CHANGE          0x00000080
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_CLOCKS           0x00000100
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_SOURCES          0x00000200
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_POWER_DOMAINS    0x00000400
#define CLOCK_GLOBAL_FLAG_STUB_API                        0x00000800
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_INVERT_CHANGE         0x00001000


/*
 * Macro defining the total number of supported client contexts.
 * The value of this macro has a direct impact on the clock data
 * structures size.
 */
#define CLOCK_CTXT_TOTAL             3


/*
 * Macro used to terminate ClockSourceMapType arrays.
 */
#define CLOCK_SOURCE_MAP_END 0xFF


/*
 * Macro used to terminate ClockDebugMuxInputDivType arrays.
 */
#define CLOCK_DEBUG_MUX_INPUT_DIV_END 0xFFFF


/*
 * Forward prototypes.
 */
typedef struct ClockSourceNodeType ClockSourceNodeType;


/*
 * ClockHWVersionType;
 *
 * HW version information.
 *
 *  Min             - Minimum supported HW Major/Minor revision
 *  Max             - Maximum supported HW Major/Minor revision
 *                    '0xFF' indicates all versions.
 *  eChipInfoFamily - Chip this configuration is for. '0' indicates all chips.
 *  aeChipInfoId    - Array of chip id's that the configuration supports.
 *                    NULL indicates all chips.
 */
typedef struct
{
  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Min;

  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Max;

  DalChipInfoFamilyType    eChipInfoFamily;
//  const DalChipInfoIdType *aeChipInfoId;

} ClockHWVersionType;


/*
 * ClockSourceFreqConfigType
 *
 * Frequency configuration information for a PLL.
 *
 *  nFreqHz     - Frequency in Hz for this configuration.
 *  pSource     - Pointer to source used by this configuration.
 *  HALConfig   - Configuration to pass to the HAL.
 *  eVRegLevel  - Voltage level for this configuration.
 *  HWVersion   - Version of the chip HW this configuration is for.
 */
typedef struct
{
  uint32                 nFreqHz;
//  ClockSourceNodeType   *pSource;
  HAL_clk_PLLConfigType  HALConfig;
  rail_voltage_level     eVRegLevel;
  ClockHWVersionType     HWVersion;
} ClockSourceFreqConfigType;


/*
 * ClockMuxConfigType
 *
 * Configuration information for a clock mux.
 *
 *  nFreqHz           - Frequency in Hz for this configuration.
 *  pSource           - Pointer to source used by this configuration.
 *  HALConfig         - Configuration to pass to the HAL.
 *  eVRegLevel        - Voltage level for this configuration.
 *  HWVersion         - Version of the chip HW this configuration is for.
 *  pSourceFreqConfig - Optional pointer to a PLL to configure for this.
 */
typedef struct
{
  uint32                           nFreqHz;
  ClockSourceNodeType             *pSource;
  HAL_clk_ClockMuxConfigType       HALConfig;
  rail_voltage_level               eVRegLevel;
  ClockHWVersionType               HWVersion;
  const ClockSourceFreqConfigType *pSourceFreqConfig;
} ClockMuxConfigType;


/*
 * ClockLogType
 *
 * Default configuration for the clock log.
 *
 *  nLogSize         - Log size in bytes.  Must be a power of 2.
 *  nGlobalLogFlags  - Global log flags.
 */
typedef struct
{
  uint32      nLogSize;
  uint8       nGlobalLogFlags;
} ClockLogType;


/*
 * ClockXOVoterType
 *
 * Structure to hold clocks which need to explicitly hold a source.
 */
typedef struct
{
  const char *szName;
} ClockXOVoterType;


/*
 * ClockPropertyValueType
 *
 * Generic clock property data.  Actual data type will depend on the property
 * and will not even necessarily be a pointer (i.e. could be straight uint32).
 */
typedef const void *ClockPropertyValueType;


/*
 * ClockPropertyType
 *
 * Generic clock property structure, the BSP data in non-DAL environments
 * will consist of an array of these.
 *
 *  szName - Name of the property
 *  Value  - Property value
 */
typedef struct
{
  const char            *szName;
  ClockPropertyValueType Value;
} ClockPropertyType;


/*
 * ClockStubType
 *
 * BSP data structure for stubbing out the clock driver on various platforms.
 *
 * bRUMI   - Stub boolean for RUMI.
 * bVirtio - Stub boolean for Virtio.
 */
typedef struct
{
  boolean bRUMI;
  boolean bVirtio;
} ClockStubType;


/*
 * ClockBase
 *
 * Structure for a register base mapping.
 *
 * nPhysAddr  - Physical address to be mapped.
 * nSize      - Size of region to be mapped.
 * pnVirtAddr - Mapped virtual address.
 */
typedef struct ClockBase
{
  uintnt  nPhysAddr;
  uint32  nSize;
  uintnt *pnVirtAddr;
} ClockBaseType;


/**
 * Structure to track corner reference counts.
 */
typedef struct ClockCornerRefCount
{
  rail_voltage_level eCorner;   /**< Voltage corner being voted for. */
  uint32             nRefCount; /**< Number of votes on eCorner. */
} ClockCornerRefCountType;


/**
 * Clock voltage rail configuration type.
 */
typedef struct ClockVRegConfigType
{
  npa_client_handle         NPAHandle;  /**< NPA client handle */
  ClockCornerRefCountType   aCornerRefCounts[RAIL_VOLTAGE_LEVEL_NUM_LEVELS];  /**< Number of active clients */
  rail_voltage_level        eMinLevel;  /**< Minimum vreg level */
} ClockVRegConfigType;


/**
 * Clock voltage rail type.
 */
typedef struct ClockRail
{
  const char            *szName;                  /**< Rail name */
  ClockVRegConfigType    VRegConfig;              /**< Voltage configuration */
  ClockVRegConfigType    VRegConfigSuppressible;  /**< Suppressible voltage configuration */
  rail_voltage_level     eBootCorner;             /**< Default boot corner for rail. */
  boolean                bSuspendVoltageRequest;  /**< Voltage suspension state */
} ClockRailType;


/**
 * Clock voltage request type.
 */
typedef struct ClockVRegRequestType
{
  rail_voltage_level  eVRegLevel;
  boolean             bSuppressible;
} ClockVRegRequestType;


/**
 * Static source register settings type.
 */
typedef struct ClockSourceRegSettingsType
{
  HAL_clk_SourceRegSettingsType HALSettings;  /* Register settings. */
  ClockHWVersionType            HWVersion;    /* HW version these settings apply to. */
} ClockSourceRegSettingsType;

/* A type for domain masters */
typedef enum
{
  CLOCK_MASTER_APCS,
  CLOCK_MASTER_DEFAULT = CLOCK_MASTER_APCS,
  CLOCK_MASTER_RPMH,
  CLOCK_MASTER_AOSS,
  CLOCK_MASTER_MAX_SIZE
} ClockMasterType;

/**
 * Clock source type.
 */
struct ClockSourceNodeType
{
//  ClockRailType                     *pRail;                         /**< Voltage rail powering this source. */
  const char                        *szName;                        /**< Source name. */
  const ClockSourceFreqConfigType   *aBSP;                          /**< Frequency configuration array. */
  const ClockSourceFreqConfigType   *pActiveFreqConfig;             /**< Pointer to active config */
  const ClockSourceFreqConfigType   *pCalibrationFreqConfig;        /**< Pointer to calibration config */
  const ClockSourceRegSettingsType  *pRegSettings;                  /**< Static register settings. */
  HAL_clk_SourceDescType             HALSource;                     /**< HAL source structure. */
  HAL_clk_SourceDisableModeType      eDisableMode;                  /**< Disable mode. */
//  ClockVRegRequestType               VRegRequest;                   /**< Voltage request */
  uint32                             nFlags;                        /**< Source flags. */
  uint32                             nConfigMask;                   /**< Configuration mask */
  uint8                              nReferenceCount;               /**< Number of active non-CPU domains */
  uint8                              aMasterRefCounts[CLOCK_MASTER_MAX_SIZE]; /**< Array for master specific reference counts */
  uint8                              nCalibrationFreqConfigLen;
  uint8                              nBSPLen;                       /**< Length of aBSP. */
  uint8                              nRegSettingsLen;               /**< Length of pRegSettings. */
//  uint32                             nReferenceCountSuppressible;   /**< Number of active suppressible domains */
//  uint32                             nReferenceCountExternalTotal;              /**< Total external reference count for this clock */
//  uint32                             nReferenceCountExternal[CLOCK_CTXT_TOTAL]; /**< Number of active external enable requests */
//  ClockSourceNodeType               *pSource;                       /**< Pointer to source's source node */
//  npa_client_handle                  NPAHandle;                     /**< NPA client handle for required requests. */
//  npa_client_handle                  NPAHandleSuppressible;         /**< NPA client handle for suppressible requests. */
//  uint32                             nCalibration;                  /**< Source calibration data */ 
};


/**
 * ClockFMaxType
 */
typedef struct
{
  uint32              nMaxFreqHz; /**< Max frequency. */
  rail_voltage_level  eVRegLevel; /**< Corner required by nMaxFreqHz */
  ClockHWVersionType  HWVersion;  /**< HW version this max frequency applies to. */
} ClockFMaxType;


/**
 * Clock source map type.
 */
typedef struct ClockSourceMap
{
  ClockSourceNodeType *pSource;   /**< Source node associated with nMuxSel. */
  ClockHWVersionType   HWVersion; /**< HW version this source map applies to. */
  uint32               nMuxSel;   /**< Mux select value. */
  uint32               nFreqHz;   /**< Frequency of clock selected by nMuxSel. */
} ClockSourceMapType;


/**
 * Structure to track input-specific divide values.
 */
typedef struct ClockDebugMuxInputDiv
{
  uint16 nMuxSel;   /**< Input mux select value. */
  uint16 nDivider;  /**< Divider for input specified by nMuxSel. */
} ClockDebugMuxInputDivType;


/**
 * Clock debug mux type.
 */
typedef struct ClockDebugMux ClockDebugMuxType;
struct ClockDebugMux
{
  const char                *szName;      /**< Debug mux name */
  ClockDebugMuxType         *pParent;     /**< Pointer to parent mux. */
  uint32                     nParentSel;  /**< Value to select this mux in pParent. */
  uint32                     nDivider;    /**< Global divider applied to all inputs on this mux. */
  ClockDebugMuxInputDivType *anInputDivs; /**< Additional dividers that apply only to specific inputs. */
  HAL_clk_DebugMuxDescType   HALDebugMux; /**< HAL debug mux structure. */
};


/**
 * Clock domain type.
 */
typedef struct
{
  ClockMuxConfigType            *aBSP;                          /**< Frequency configuration array. */
//  ClockRailType                 *pRail;                         /**< Voltage rail powering this domain. */
//  ClockSourceNodeType           *pSource;                       /**< Pointer to source node */
  const ClockMuxConfigType      *pActiveMuxConfig;              /**< Pointer to active config */
//  const ClockFMaxType           *aFMax;                         /**< Max frequency array. */
//  const ClockSourceMapType      *aSourceMap;                    /**< Source map array. */
//  ClockVRegRequestType           VRegRequest;                   /**< Voltage request */
  HAL_clk_ClockDomainDescType    HALDomain;                     /**< HAL domain structure. */
  uint32                         nFlags;                        /**< Domain flags. */
  uint8                          nBSPLen;                       /**< Length of aBSP. */
  uint8                          nReferenceCount;               /**< Number of active clocks */
  ClockMasterType                eMaster;                       /**< Domain master. */
//  uint32                         nReferenceCountSuppressible;   /**< Number of active suppressible domains */
} ClockDomainNodeType;


/**
 * Clock type.
 */
typedef struct
{
  const char              *szName;                            /**< Clock name. */
  ClockDomainNodeType     *pDomain;                           /**< Domain this clock belongs to. */
  uint32                   nFlags;                            /**< Clock flags. */
  HAL_clk_ClockDescType    HALClock;                          /**< HAL clock structure. */
  uint8                    nReferenceCountTotal;              /**< Total number of reference counts for this clock */
//  uint32                   nReferenceCount[CLOCK_CTXT_TOTAL]; /**< Number of active client requests */
//  ClockDebugMuxType       *pDebugMux;                         /**< Pointer to debug mux this clock is connected to. */
//  uint32                   nUniqueId;                         /**< Unique number used to identify this clock in logs. */
} ClockNodeType;


/**
 * Power domain type.
 */
typedef struct
{
  const char                    *szName;                            /**< Power domain name. */
  ClockRailType                 *pRail;                             /**< Voltage rail powering this domain. */
  HAL_clk_PowerDomainDescType    HALPowerDomain;                    /**< HAL power domain structure. */
  ClockVRegRequestType           VRegRequest;                       /**< Voltage request */
  uint32                         nFlags;                            /**< Power domain flags. */
  uint8                          nReferenceCountTotal;              /**< Total number of reference counts */
//  uint32                         nReferenceCount[CLOCK_CTXT_TOTAL]; /**< Number of active client requests */
} ClockPowerDomainNodeType;


/**
 * Clock divider type.
 */
typedef struct ClockDivider
{
  const char                *szName;      /**< Clock divider name. */
  uint32                     nFlags;      /**< Divider flags. */
  HAL_clk_DividerDescType    HALDivider;  /**< HAL clock divider structure. */
} ClockDividerNodeType;


/**
 * Clock NPA resource context.
 */
typedef struct ClockNPACtxt
{
  const char    *szClockName; /**< Name of clock to control. */
  ClockNodeType *pClock;      /**< Clock node to control. */
} ClockNPACtxtType;


/**
 * Clock NPA node definition.
 */
typedef struct ClockNPANode
{
  npa_node_definition  Node;          /**< NPA node. */
  npa_resource_state  *anInitStates;  /**< Initial resource states. */
} ClockNPANodeType;


/**
 * Main clock BSP type.
 */
typedef struct ClockBSP
{
  ClockRailType             **aRails;           /**< Array of voltage rails. */
  ClockNodeType              *aClocks;          /**< Array of clocks. */
  ClockDomainNodeType       **aDomains;         /**< Array of clock domains. */
  ClockSourceNodeType       **aSources;         /**< Array of clock sources. */
  ClockDividerNodeType       *aDividers;        /**< Array of clock dividers .*/
  ClockPowerDomainNodeType   *aPowerDomains;    /**< Array of power domains. */
  ClockNPANodeType           *pGenericNPANode;  /**< Generic NPA node. */
  uintptr_t                  *aPDEnable;
  uintptr_t                  *aBranchEnable;
  HAL_clk_RegisterMaskType   *aBranchVote;
  HAL_clk_ClockMuxConfigType  SafeMuxConfig;    /**< Default safe mux configuration. */
  uint32                      nFlags;           /**< Clock driver flags. */
  uint8                      nNumRails;        /**< Length of aRails. */
  uint8                      nNumClocks;       /**< Length of aClocks. */
  uint8                      nNumDomains;      /**< Length of aDomains. */
  uint8                      nNumSources;      /**< Length of aSources. */
  uint8                      nNumDividers;     /**< Length of aDividers. */
  uint8                      nNumPowerDomains; /**< Length of aPowerDomains. */
  uint8                      nNumPDEnables; /**< Length of aPowerDomains. */
  uint8                      nNumBranchEnables; /**< Length of aPowerDomains. */
  uint8                      nNumBranchVotes; /**< Length of aPowerDomains. */
//  uint32                      nLogFlags;        /**< Clock driver log flags. */
//  uint32                      nNumDebugMuxes;   /**< Length of aDebugMuxes. */
//  ClockDebugMuxType         **aDebugMuxes;      /**< Array of debug muxes. */
//  uint32                      nNumBases;        /**< Length of aBases. */
//  ClockBaseType              *aBases;           /**< Array of clock bases. */
} ClockBSPType;


#endif  /* __CLOCKBSP_H__ */ 

