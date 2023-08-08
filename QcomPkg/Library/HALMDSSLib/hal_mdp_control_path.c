/*! \file */

/*
===========================================================================

FILE:         hal_mdp_control_path.c

DESCRIPTION: Connects the blocks of the MDP (Source, Mixer and Display Interface) 
  

===========================================================================
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_ControlPath_BaseOffset()
*/
/*!
* \brief
*     Returns offset of the layer mixer register
*
* \param [in] eControlPath         - Control Path ID (0, 1, 2, 3, 4)
*
* \retval uintPtr
*
****************************************************************************/
static uintPtr HAL_MDP_Get_ControlPath_BaseOffset(HAL_MDP_ControlPathId  eControlPath)
{
   uintPtr      uRegOffset     = 0x0;

   if((HAL_MDP_CONTROL_PATH_NONE           == eControlPath)         || 
      (HAL_MDP_GET_CONTROL_PATHS_COUNT()   <  (uint32)eControlPath) ||
      (HAL_MDP_CONTROL_PATH_MAX            <=  eControlPath))
   {
      HAL_MDP_ASSERT();
   }
   else
   {
      uRegOffset = uMDPControlPathRegBaseOffset[eControlPath];
   }
   return uRegOffset;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_MixerConfig()
*/
/*!
* \brief
*     Control path configuration
*
* \param [in] eControlPath                - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pControlPathMixerConfig     - Control path mixer configuration information
* \param [in] uFlags                      - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void  HAL_MDP_ControlPath_MixerConfig(HAL_MDP_ControlPathId                 eControlPath,
                                             HAL_MDP_ControlPath_MixerConfigType  *pControlPathMixerConfig,
                                             uint32                                uFlags)
{
   uintPtr                 uRegOffset              = 0x00;
   uint32                  uRegValue               = 0x00;
   uint32                  uRegValueEx             = 0x00;
   uint32                  uRegValueEx2            = 0x00;
   uint32                  uBlendOrderIndex        = 0x00;
   uint32                  uLayerBlendOrder        = 0x00;
   bool32                  bLayerBlendOrderEx      = FALSE;
   uintPtr                 uLayerMixerRegAddr      = 0x00;
   uintPtr                 uLayerMixerExtRegAddr   = 0x00;
   uintPtr                 uLayerMixerExt2RegAddr  = 0x00;
   uint32                  uMixerId                = (pControlPathMixerConfig->eMixerId<HAL_MDP_LAYER_MIXER_MAX)?(uint32)pControlPathMixerConfig->eMixerId:(uint32)HAL_MDP_LAYER_MIXER_NONE;
   uint32                  uLayerMixerRegOffsets[HAL_MDP_LAYER_MIXER_MAX]    = { 0x00, 0x00, 0x04, 0x08, 0x0C, 0x10, 0x24, 0x00 };
   uint32                  uLayerMixerExtRegOffsets[HAL_MDP_LAYER_MIXER_MAX] = { 0x00, 0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x00 };

   uRegOffset  = HAL_MDP_Get_ControlPath_BaseOffset(eControlPath);

   uLayerMixerRegAddr    = HWIO_MDP_CTL_0_LAYER_0_ADDR     + uRegOffset + uLayerMixerRegOffsets[uMixerId];
   uLayerMixerExtRegAddr = HWIO_MDP_CTL_0_LAYER_0_EXT_ADDR + uRegOffset + uLayerMixerExtRegOffsets[uMixerId];
   uLayerMixerExt2RegAddr = HWIO_MDP_CTL_0_LAYER_0_EXT2_ADDR + uRegOffset + uLayerMixerExtRegOffsets[uMixerId];

   uRegValue      = in_dword(uLayerMixerRegAddr);
   uRegValueEx    = in_dword(uLayerMixerExtRegAddr);
   uRegValueEx2   = in_dword(uLayerMixerExt2RegAddr);

   // Border Color Configuration
   if(NULL != pControlPathMixerConfig->psMixerBorderColorConfig)
   {
      uRegValue = HWIO_OUT_FLD(uRegValue,
                               MDP_CTL_0_LAYER_0, 
                               BORDER_OUT,
                               pControlPathMixerConfig->psMixerBorderColorConfig->bBorderColorEnable);
   }

   // Blend Stage Z-ORDER configuration
   if (NULL != pControlPathMixerConfig->psMixerZOrderConfig)
   {
       // Clear all layer attach information
       uRegValue = uRegValue & HWIO_MDP_CTL_0_LAYER_0_BORDER_OUT_BMSK;

       // Clear all layer attach information
       uRegValueEx2 = 0; 

       for (uBlendOrderIndex = HAL_MDP_BLEND_ORDER_STAGE_0_BASE_LAYER;
           uBlendOrderIndex < HAL_MDP_BLEND_ORDER_STAGE_MAX;
           uBlendOrderIndex++)
       {
           uint32 uI;

           uLayerBlendOrder   = ((uBlendOrderIndex >= HAL_MDP_BLEND_ORDER_STAGE_6_FG_LAYER) ? (uBlendOrderIndex - HAL_MDP_BLEND_ORDER_STAGE_6_FG_LAYER) : (uBlendOrderIndex));
           bLayerBlendOrderEx = ((uBlendOrderIndex >= HAL_MDP_BLEND_ORDER_STAGE_6_FG_LAYER) ? (TRUE) : (FALSE));

           for (uI = 0; uI < HAL_MDP_MAX_SSPP_IN_DUALPIPE_MODE; uI++)
           {
               switch (pControlPathMixerConfig->psMixerZOrderConfig->eSourcePipe[uBlendOrderIndex][uI])
               {
               case HAL_MDP_SOURCE_PIPE_VIG_0:
                   uRegValue = HWIO_OUT_FLD(uRegValue, MDP_CTL_0_LAYER_0, VIG_0_OUT, uLayerBlendOrder);
                   uRegValueEx = HWIO_OUT_FLD(uRegValueEx, MDP_CTL_0_LAYER_0_EXT, VIG_0_OUT_BIT3, bLayerBlendOrderEx);
                   break;

               case HAL_MDP_SOURCE_PIPE_VIG_1:
                   uRegValue = HWIO_OUT_FLD(uRegValue, MDP_CTL_0_LAYER_0, VIG_1_OUT, uLayerBlendOrder);
                   uRegValueEx = HWIO_OUT_FLD(uRegValueEx, MDP_CTL_0_LAYER_0_EXT, VIG_1_OUT_BIT3, bLayerBlendOrderEx);
                   break;

               case HAL_MDP_SOURCE_PIPE_VIG_2:
                   uRegValue = HWIO_OUT_FLD(uRegValue, MDP_CTL_0_LAYER_0, VIG_2_OUT, uLayerBlendOrder);
                   uRegValueEx = HWIO_OUT_FLD(uRegValueEx, MDP_CTL_0_LAYER_0_EXT, VIG_2_OUT_BIT3, bLayerBlendOrderEx);
                   break;

               case HAL_MDP_SOURCE_PIPE_VIG_3:
                   uRegValue = HWIO_OUT_FLD(uRegValue, MDP_CTL_0_LAYER_0, VIG_3_OUT, uLayerBlendOrder);
                   uRegValueEx = HWIO_OUT_FLD(uRegValueEx, MDP_CTL_0_LAYER_0_EXT, VIG_3_OUT_BIT3, bLayerBlendOrderEx);
                   break;

               case HAL_MDP_SOURCE_PIPE_DMA_0:
                   uRegValue = HWIO_OUT_FLD(uRegValue, MDP_CTL_0_LAYER_0, DMA_0_OUT, uLayerBlendOrder);
                   uRegValueEx = HWIO_OUT_FLD(uRegValueEx, MDP_CTL_0_LAYER_0_EXT, DMA_0_OUT_BIT3, bLayerBlendOrderEx);
                   break;

               case HAL_MDP_SOURCE_PIPE_DMA_1:
                   uRegValue = HWIO_OUT_FLD(uRegValue, MDP_CTL_0_LAYER_0, DMA_1_OUT, uLayerBlendOrder);
                   uRegValueEx = HWIO_OUT_FLD(uRegValueEx, MDP_CTL_0_LAYER_0_EXT, DMA_1_OUT_BIT3, bLayerBlendOrderEx);
                   break;

               case HAL_MDP_SOURCE_PIPE_DMA_2:
                   uRegValueEx2 = HWIO_OUT_FLD(uRegValueEx2, MDP_CTL_0_LAYER_0_EXT2, DMA_2_OUT, uBlendOrderIndex);
                   break;

               case HAL_MDP_SOURCE_PIPE_DMA_3:
                   uRegValueEx2 = HWIO_OUT_FLD(uRegValueEx2, MDP_CTL_0_LAYER_0_EXT2, DMA_3_OUT, uBlendOrderIndex);
                   break;

               case HAL_MDP_SOURCE_PIPE_DMA_0_REC1:
                   uRegValueEx2 = HWIO_OUT_FLD(uRegValueEx2, MDP_CTL_0_LAYER_0_EXT2, DMA_0_REC1_OUT, uBlendOrderIndex);
                   break;

               case HAL_MDP_SOURCE_PIPE_DMA_1_REC1:
                   uRegValueEx2 = HWIO_OUT_FLD(uRegValueEx2, MDP_CTL_0_LAYER_0_EXT2, DMA_1_REC1_OUT, uBlendOrderIndex);
                   break;

               case HAL_MDP_SOURCE_PIPE_DMA_2_REC1:
                   uRegValueEx2 = HWIO_OUT_FLD(uRegValueEx2, MDP_CTL_0_LAYER_0_EXT2, DMA_2_REC1_OUT, uBlendOrderIndex);
                   break;

               case HAL_MDP_SOURCE_PIPE_DMA_3_REC1:
                   uRegValueEx2 = HWIO_OUT_FLD(uRegValueEx2, MDP_CTL_0_LAYER_0_EXT2, DMA_3_REC1_OUT, uBlendOrderIndex);
                   break;

               case HAL_MDP_SOURCE_PIPE_VIG_4:
               case HAL_MDP_SOURCE_PIPE_RGB_0:
               case HAL_MDP_SOURCE_PIPE_RGB_1:
               case HAL_MDP_SOURCE_PIPE_RGB_2:
               case HAL_MDP_SOURCE_PIPE_RGB_3:
               case HAL_MDP_SOURCE_PIPE_RGB_4:
               case HAL_MDP_SOURCE_PIPE_DMA_4:
               case HAL_MDP_SOURCE_PIPE_NONE:
               case HAL_MDP_SOURCE_PIPE_MAX:
               case HAL_MDP_SOURCE_PIPE_FORCE_32BIT:
               default:
                   break;
               }
           }
       }
   }
  out_dword(uLayerMixerRegAddr, uRegValue);
  out_dword(uLayerMixerExtRegAddr, uRegValueEx);
  out_dword(uLayerMixerExt2RegAddr, uRegValueEx2);
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_InterfaceConfig()
*/
/*!
* \brief
*     Control path configuration
*
* \param [in] eControlPath                - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pInterfaceConfig            - Control path interface configuration
* \param [in] uFlags                      - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void  HAL_MDP_ControlPath_InterfaceConfig(HAL_MDP_ControlPathId                     eControlPath,
                                                 HAL_MDP_ControlPath_InterfaceConfigType  *pInterfaceConfig,
                                                 uint32                                    uFlags)
{
   uintPtr                 uRegOffset        = 0x0;
   uint32                  uRegValue         = 0x00;

  uRegOffset  = HAL_MDP_Get_ControlPath_BaseOffset(eControlPath);

  if(HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE > pInterfaceConfig->eInterfaceId) // Physical interface connection
  {
    uint32    uInterfaceId       = pInterfaceConfig->eInterfaceId;

    if(HAL_MDP_FLAGS_MIRROR_INTERFACE & uFlags)
    {
      uInterfaceId = (HAL_MDP_INTERFACE_1 == uInterfaceId)? HAL_MDP_INTERFACE_2 : HAL_MDP_INTERFACE_1;
    }

    uRegValue = HWIO_OUT_FLD(uRegValue, MDP_CTL_0_TOP, MODE_SEL, pInterfaceConfig->eInterfaceModeType);
    out_dword(HWIO_MDP_CTL_0_TOP_ADDR + uRegOffset, uRegValue);

    // Set INTF active - if master, overwrite and if not, then OR it with existing value
    uRegValue = in_word(HWIO_MDP_CTL_0_INTF_ACTIVE_ADDR + uRegOffset);

    if (HAL_MDP_FLAGS_SLAVE_CONTROL_PATH & uFlags)
    {
      // Second interface on the same control path - Read-Modify-Write
      uRegValue |= (1 << (uInterfaceId - HAL_MDP_INTERFACE_0));
    }
    else
    {
      uRegValue = (1 << (uInterfaceId - HAL_MDP_INTERFACE_0));

      // Set master interface
      out_dword(HWIO_MDP_CTL_0_INTF_MASTER_ADDR + uRegOffset, uRegValue);
    }

    out_dword(HWIO_MDP_CTL_0_INTF_ACTIVE_ADDR + uRegOffset, uRegValue);
  }
  else if (HAL_MDP_INTERFACE_MEMORY_WB_LINEAR_MODE == pInterfaceConfig->eInterfaceId) // Write-Back Path
  {
    out_dword(HWIO_MDP_CTL_0_WB_ACTIVE_ADDR + uRegOffset, HWIO_OUT_FLD(0, MDP_CTL_0_WB_ACTIVE, WB_2, 1));
  }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_Interface_Status()
*/
/*!
* \brief
*     Control path configuration
*
* \param [in] eControlPath                - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pInterfaceInfoData          - Interface data configuration
* \param [in] uFlags                      - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void HAL_MDP_ControlPath_Interface_Status( HAL_MDP_ControlPathId                     eControlPath,
                                                  HAL_MDP_ControlPath_InterfaceConfigType  *pInterfaceInfoData,
                                                  uint32                                    uFlags)
{
#if 0 // Poipu - #TODO#IMPLEMENT  
  if ((eControlPath > HAL_MDP_CONTROL_PATH_NONE) &&  (eControlPath < HAL_MDP_CONTROL_PATH_MAX))
  {
    uintPtr uRegOffset         = 0x0;
    uint32  uControlPathTopCfg = 0x00;
    uint32  uMemSel;

    uRegOffset  = HAL_MDP_Get_ControlPath_BaseOffset(eControlPath) ;

    // Read in register value
    uControlPathTopCfg = in_dword(HWIO_MDP_CTL_0_TOP_ADDR + uRegOffset); 
    pInterfaceInfoData->eInterfaceId         = (HAL_MDP_InterfaceId)HWIO_GETVAL(MDP_CTL_0_TOP, uControlPathTopCfg, OUT_SEL);
    uMemSel                                  = HWIO_GETVAL(MDP_CTL_0_TOP, uControlPathTopCfg, MEM_SEL);
    if ((HAL_MDP_INTERFACE_NONE == pInterfaceInfoData->eInterfaceId) &&
        (uMemSel > 0))
    {
        // If interface is not selected decode the writeback configuration          
      pInterfaceInfoData->eInterfaceId = (HAL_MDP_InterfaceId)(HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE + uMemSel);
    }
    pInterfaceInfoData->eInterfaceModeType   = (HAL_MDP_InterfaceModeType)HWIO_GETVAL(MDP_CTL_0_TOP, uControlPathTopCfg, MODE_SEL);
    pInterfaceInfoData->b3DMuxEnable         = (bool32)HWIO_GETVAL(MDP_CTL_0_TOP, uControlPathTopCfg, PACK_3D_OUT);
    pInterfaceInfoData->eMuxPackerModeConfig = (HAL_MDP_3DMUXPackerModeType)HWIO_GETVAL(MDP_CTL_0_TOP, uControlPathTopCfg, PACK_3D_MODE);

  }
  else
  {
    pInterfaceInfoData->eInterfaceId       = HAL_MDP_INTERFACE_NONE;
    pInterfaceInfoData->eInterfaceModeType = HAL_MDP_INTERFACE_MODE_NONE;
  }
#endif  
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_ActiveModulesConfig()
*/
/*!
* \brief
*     Control path Active modules configuration
*
* \param [in] eControlPath                - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pActiveModulesConfig        - IDs of active modules for this Control
* \param [in] uFlags                      - Reserved flags
*
* \retval NONE
*
****************************************************************************/
static void  HAL_MDP_ControlPath_ActiveModulesConfig(HAL_MDP_ControlPathId            eControlPath,
                                                     HAL_MDP_ControlPath_ModulesType *pActiveModulesConfig,
                                                     uint32                           uFlags)
{
  uintPtr                 uRegOffset        = 0x0;
  uint32                  uRegValue         = 0x00;

  uRegOffset  = HAL_MDP_Get_ControlPath_BaseOffset(eControlPath);

  // Activate DSC module
  if (HAL_MDP_DSC_NONE != pActiveModulesConfig->uDSCId)
  {
    uint32 uDSCCtlOffset  = 0x0;
    
    uRegValue = (1 << (pActiveModulesConfig->uDSCId - HAL_MDP_DSC_0));

    if (HAL_MDP_FLAGS_SLAVE_CONTROL_PATH & uFlags)
    {
      // Second interface on the same control path - Read-Modify-Write
      uRegValue |= (uint32) in_word(HWIO_MDP_CTL_0_DSC_ACTIVE_ADDR + uRegOffset);
    }

    out_dword(HWIO_MDP_CTL_0_DSC_ACTIVE_ADDR + uRegOffset, uRegValue);

    // Get offset for the DSC input xbar register
    uDSCCtlOffset = 0x4 * (pActiveModulesConfig->uDSCId - HAL_MDP_DSC_0);

    // Configure DSC input xbar
    out_dword(HWIO_MDP_DSC_CTL_DSC_0_CTL_ADDR + uDSCCtlOffset, (pActiveModulesConfig->uPingPongId - HAL_MDP_PINGPONG_0));
  }

  // Activate 3d Mux module
  if ((HAL_MDP_3DMUX_NONE != pActiveModulesConfig->u3DMuxId) &&
      (HAL_MDP_3DMUX_MAX > pActiveModulesConfig->u3DMuxId))
  {
    uRegValue = (1 << (pActiveModulesConfig->u3DMuxId - HAL_MDP_3DMUX_0));

    if (HAL_MDP_FLAGS_SLAVE_CONTROL_PATH & uFlags)
    {
      // Second interface on the same control path - Read-Modify-Write
      uRegValue |= (uint32) in_word(HWIO_MDP_CTL_0_MERGE_3D_ACTIVE_ADDR + uRegOffset);
    }

    out_dword(HWIO_MDP_CTL_0_MERGE_3D_ACTIVE_ADDR + uRegOffset, uRegValue);

    // Enable 3D Muxer
    uRegOffset = uMDP3DMuxRegBaseOffset[pActiveModulesConfig->u3DMuxId];
    uRegValue  = HWIO_OUT_FLD(0, MDP_MERGE_3D_0_MODE, ENABLE, 1);

    // Setting 3D Mux mode to Horizontal row interleave, by default.
    uRegValue  = HWIO_OUT_FLD(uRegValue, MDP_MERGE_3D_0_MODE, PACK, 1);

    out_dword(HWIO_MDP_MERGE_3D_0_MODE_ADDR + uRegOffset, uRegValue); 
  }
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_Setup()
*/
/*!
* \brief
*     Set up control path configuration
*
* \param [in] eControlPath          - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pControlPathConfig    - Control path configuration information
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ControlPath_Setup(HAL_MDP_ControlPathId           eControlPath,
                                             HAL_MDP_ControlPathConfigType  *pControlPathConfig,
                                             uint32                          uFlags)
{
   HAL_MDSS_ErrorType      eStatus  = HAL_MDSS_STATUS_SUCCESS;

   // Active modules configuration
   if(NULL != pControlPathConfig->psModulesConfig)
   {
      HAL_MDP_ControlPath_ActiveModulesConfig(eControlPath, pControlPathConfig->psModulesConfig, uFlags);
   }

   // Control Path Layer Mixer Z-Order Configuration
   if(NULL != pControlPathConfig->psMixerConfig)
   {
      HAL_MDP_ControlPath_MixerConfig(eControlPath, pControlPathConfig->psMixerConfig, 0x00);
   }

   // Control Path Interface Cross Bar (or) Memory Type Selection
   if(NULL != pControlPathConfig->psInterfaceConfig)
   {
      HAL_MDP_ControlPath_InterfaceConfig(eControlPath, pControlPathConfig->psInterfaceConfig, uFlags);
   }

   // Mixer Configuration
   if(NULL != pControlPathConfig->psLayerMixerConfig)
   {
      HAL_MDP_LayerMixer_Config(pControlPathConfig->psLayerMixerConfig->eLayerMixerId,
                                pControlPathConfig->psLayerMixerConfig,
                                uFlags);
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_Commit()
*/
/*!
* \brief
*     Control path flush and triggers the write-back path
*
* \param [in] eControlPath          - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pControlPathFlush     - Modules to Flush
* \param [in] eInterfaceConnectType - Mainly for write-back paths
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ControlPath_Commit(HAL_MDP_ControlPathId         eControlPath,
                                              HAL_MDP_ControlPathFlushType  *pControlPathFlush,
                                              HAL_MDP_InterfaceConnectType  eInterfaceConnectType,
                                              uint32                        uFlags)
{
  HAL_MDSS_ErrorType      eStatus     = HAL_MDSS_STATUS_SUCCESS;
  uintPtr                 uRegOffset  = 0x0;
  uint32                  uRegValue   = 0x00;

  uRegOffset  = HAL_MDP_Get_ControlPath_BaseOffset(eControlPath);

  // Flush the modules
  uRegValue  = pControlPathFlush->uFlushModules;

  /* 
   * Setup submodules for flush  
   * Hardware has more granularity for flush, but that is not exposed yet.
   * So DSC, 3d merge & PERIPH modules will be flushed on INTF change
   */
  if (uRegValue & (HAL_CONTROL_PATH_FLUSH_INTF | HAL_CONTROL_PATH_FLUSH_WB))
  {
    uint32                  uActive     = 0x0;

    // DSC
    uActive = in_dword(HWIO_MDP_CTL_0_DSC_ACTIVE_ADDR + uRegOffset);
    if (0 < uActive)
    {
      out_dword(HWIO_MDP_CTL_0_DSC_FLUSH_ADDR + uRegOffset, uActive);
      uRegValue |= HAL_CONTROL_PATH_FLUSH_DSC;
    }

    // 3D Mux
    uActive = in_dword(HWIO_MDP_CTL_0_MERGE_3D_ACTIVE_ADDR + uRegOffset);
    if (0 < uActive)
    {
      out_dword(HWIO_MDP_CTL_0_MERGE_3D_FLUSH_ADDR + uRegOffset, uActive);
      uRegValue |= HAL_CONTROL_PATH_FLUSH_MERGE_3D;
    }

    // Physical interfaces & Peripherals
    uActive = in_dword(HWIO_MDP_CTL_0_INTF_ACTIVE_ADDR + uRegOffset);
    if (0 < uActive)
    {
      out_dword(HWIO_MDP_CTL_0_INTF_FLUSH_ADDR + uRegOffset, uActive);

      // There is no active register for peripherals, they are linked to INTF
      out_dword(HWIO_MDP_CTL_0_PERIPH_FLUSH_ADDR + uRegOffset, uActive);
      uRegValue |= HAL_CONTROL_PATH_FLUSH_PERIPH;
    }

    // Writeback interfaces
    uActive = in_dword(HWIO_MDP_CTL_0_WB_ACTIVE_ADDR + uRegOffset);
    if (0 < uActive)
    {
      out_dword(HWIO_MDP_CTL_0_WB_FLUSH_ADDR + uRegOffset, uActive);
    }
  }

  // Write top level Flush
  out_dword(HWIO_MDP_CTL_0_FLUSH_ADDR + uRegOffset, uRegValue);

  // Only if write-back path or dsi command mode. Trigger the write back operation only after flush
  if((HAL_MDP_INTERFACE_TYPE_WRITE_BACK == eInterfaceConnectType) ||
     (HAL_MDP_FLAGS_COMMIT_FORCE_UPDATE & uFlags))
  {
    out_dword(HWIO_MDP_CTL_0_CTL_START_ADDR + uRegOffset, 0x01);
  }
  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_GetProperty()
*/
/*!
* \brief
*     Retrieve information about an existing control path
*
* \param [in] eControlPath          - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pControlInfoData      - Configuration for the current control path
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ControlPath_GetProperty(HAL_MDP_ControlPathId           eControlPath,
                                                   HAL_MDP_ControlPathConfigType  *pControlInfoData,
                                                   uint32                          uFlags)
{
   HAL_MDSS_ErrorType      eStatus     = HAL_MDSS_STATUS_SUCCESS;

   // Interface configuration
   if (NULL != pControlInfoData->psInterfaceConfig)
   {
      HAL_MDP_ControlPath_Interface_Status(eControlPath, pControlInfoData->psInterfaceConfig, 0x00);
   }


   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_Reset()
*/
/*!
* \brief
*     Triggers a soft reset for a given control path
*
* \param [in] eControlPath          - Control Path ID (0, 1, 2, 3, 4)
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ControlPath_Reset(HAL_MDP_ControlPathId           eControlPath,
                                             uint32                          uFlags)
{
   HAL_MDSS_ErrorType      eStatus     = HAL_MDSS_MDP_STATUS_FAILED;
   uintPtr                 uRegOffset  = 0x0;
   uint32                  uResetVal   = 0x00;
   uint32                  uCnt        = 0x00;

   uRegOffset  = HAL_MDP_Get_ControlPath_BaseOffset(eControlPath);

   // Trigger soft reset
   out_dword(HWIO_MDP_CTL_0_SW_RESET_ADDR + uRegOffset, 0x01);

   // Poll for reset completion
   for(uCnt = 0; uCnt < HAL_MDP_MAX_RETRIES_FOR_CTRL_PATH_RESET_COMPLETE; uCnt++)
   {
      uResetVal = in_dword(HWIO_MDP_CTL_0_SW_RESET_ADDR + uRegOffset);
      if(0x00 == uResetVal)
      {
         eStatus = HAL_MDSS_STATUS_SUCCESS;
         break;
      }
   }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_SetupCommitMode()
*/
/*!
* \brief
*     Configure specific commit on split display. For ex for single commit
*     writing commit bits on  master pipe will also commit slave pipe. 
*     This applicable for video mode only.
*
* \param [in]     eCommitMode  - Requested commit mode configuration
* \param [in]     uFlags       - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_SetupCommitMode(HAL_MDP_CommitModeType eCommitMode,
                                                  uint32                 uFlags)
{
  HAL_MDSS_ErrorType   eStatus        = HAL_MDSS_STATUS_SUCCESS;
  uint32               uRegValue      = 0x00000000;

  // Check the commit mode type
  if (HAL_MDP_COMMITMODE_NONE == eCommitMode)
  {
    // Get the current register setting
    uRegValue = HWIO_MDP_MDP_SSPP_SPARE_0_IN;
    uRegValue = HWIO_OUT_FLD(uRegValue, MDP_MDP_SSPP_SPARE_0, MDP_SSPP_SPARE0, FALSE);

    // Update the register setting with new value
    HWIO_MDP_MDP_SSPP_SPARE_0_OUT(uRegValue);
  }
  else if (HAL_MDP_COMMITMODE_SINGLE == eCommitMode)
  {
    // Get the current register setting
    uRegValue = HWIO_MDP_MDP_SSPP_SPARE_0_IN;
    uRegValue = HWIO_OUT_FLD(uRegValue, MDP_MDP_SSPP_SPARE_0, MDP_SSPP_SPARE0, TRUE);

    // Update the register setting with new value
    HWIO_MDP_MDP_SSPP_SPARE_0_OUT(uRegValue);
  }
  else
  {
    eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
  }
  
  return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_SetProperty()
*/
/*!
* \brief
*     Configuration paramaters for setting up a control path
*
* \param [in] eControlPath          - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pControlInfoData      - Configuration for the current control path
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ControlPath_SetProperty(HAL_MDP_ControlPathId           eControlPath,
                                                   HAL_MDP_ControlPathConfigType  *pControlInfoData,
                                                   uint32                          uFlags)
{
   HAL_MDSS_ErrorType      eStatus     = HAL_MDSS_STATUS_SUCCESS;

   // Interface configuration
   if (NULL != pControlInfoData->psCommitModeConfig)
   {
      eStatus = HAL_MDP_SetupCommitMode(pControlInfoData->psCommitModeConfig->eCommitMode, uFlags);
   }

   return eStatus;
}

#ifdef __cplusplus
}
#endif
