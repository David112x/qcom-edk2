/*=============================================================================
 
  File: MDPConfig.c
 
  Source file for MDP configuration functions
  
 
  Copyright (c) 2011-2020 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#include <Library/PcdLib.h>
#include "MDPLib.h"
#include "MDPLib_i.h"
#include "MDPPlatformLib.h"
#include "MDPSystem.h"
#include "hal_mdp.h"


/*=========================================================================
     Default Defines
==========================================================================*/



/*=========================================================================
      MDP Configuration Tables
==========================================================================*/

/* Access macro for gMDP_DisplayControlPathMap */
#define MDP_GET_CTRLPATHMAP(_displayId_, _mixerId_)    (&(gMDP_DisplayControlPathMap[((_displayId_)<MDP_DISPLAY_MAX)? \
                                                       (_displayId_):MDP_DISPLAY_PRIMARY][((_mixerId_)<MDP_QUADPIPE_NUM_MIXERS)?(_mixerId_):0]))

/* Default blend order. HLOS composes bottom up layer(z-order 1), using higher z-order in UEFI enable seamless transition to the OS owned layers. */
#define HAL_MDP_BLEND_ORDER_ID                         HAL_MDP_BLEND_ORDER_STAGE_3_FG_LAYER

/* Macro to get _HAL_MDP_InterfaceConnectType from HAL_MDP_InterfaceId */
#define HAL_GET_MDP_INTERFACE_TYPE(_intfId_) ((HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE>(_intfId_))?HAL_MDP_INTERFACE_TYPE_PHYSICAL_CONNECT:HAL_MDP_INTERFACE_TYPE_WRITE_BACK)

/* Map to find the control path associated with each display */
MDP_DisplayCtrlPathMapType gMDP_DisplayControlPathMap[MDP_DISPLAY_MAX][MDP_QUADPIPE_NUM_MIXERS] =
{
  // MDP_DISPLAY_PRIMARY
  {
    {HAL_MDP_SOURCE_PIPE_VIG_0, HAL_MDP_CONTROL_PATH_0,    HAL_MDP_LAYER_MIXER_0,    HAL_MDP_DESTINATION_PIPE_0,    HAL_MDP_PINGPONG_0},    // Main control Path in single pipe or Index 0 path in dual pipe
    {HAL_MDP_SOURCE_PIPE_VIG_1, HAL_MDP_CONTROL_PATH_0,    HAL_MDP_LAYER_MIXER_1,    HAL_MDP_DESTINATION_PIPE_1,    HAL_MDP_PINGPONG_1},    // Index 1 path in dual/quad pipe
    {HAL_MDP_SOURCE_PIPE_VIG_2, HAL_MDP_CONTROL_PATH_0,    HAL_MDP_LAYER_MIXER_2,    HAL_MDP_DESTINATION_PIPE_2,    HAL_MDP_PINGPONG_2},    // Index 2 path in quad pipe
    {HAL_MDP_SOURCE_PIPE_VIG_3, HAL_MDP_CONTROL_PATH_0,    HAL_MDP_LAYER_MIXER_3,    HAL_MDP_DESTINATION_PIPE_3,    HAL_MDP_PINGPONG_3},    // Index 3 path in quad pipe
  },
  // MDP_DISPLAY_SECONDARY
  {
    {HAL_MDP_SOURCE_PIPE_VIG_1, HAL_MDP_CONTROL_PATH_1,    HAL_MDP_LAYER_MIXER_1,    HAL_MDP_DESTINATION_PIPE_1,    HAL_MDP_PINGPONG_1},    // Main control Path in single pipe or Index 0 path in dual pipe
    {HAL_MDP_SOURCE_PIPE_VIG_2, HAL_MDP_CONTROL_PATH_1,    HAL_MDP_LAYER_MIXER_2,    HAL_MDP_DESTINATION_PIPE_2,    HAL_MDP_PINGPONG_2},    // Index 1 path in dual pipe
  },
  // MDP_DISPLAY_EXTERNAL
  {
    {HAL_MDP_SOURCE_PIPE_VIG_2, HAL_MDP_CONTROL_PATH_2,    HAL_MDP_LAYER_MIXER_2,    HAL_MDP_DESTINATION_PIPE_2,    HAL_MDP_PINGPONG_2},    // Main control Path in single pipe or Index 0 path in dual pipe
    {HAL_MDP_SOURCE_PIPE_VIG_3, HAL_MDP_CONTROL_PATH_2,    HAL_MDP_LAYER_MIXER_3,    HAL_MDP_DESTINATION_PIPE_3,    HAL_MDP_PINGPONG_3},    // Index 1 path in dual pipe
  },
  // MDP_DISPLAY_EXTERNAL2
  {
    {HAL_MDP_SOURCE_PIPE_VIG_2, HAL_MDP_CONTROL_PATH_3,    HAL_MDP_LAYER_MIXER_4,    HAL_MDP_DESTINATION_PIPE_NONE, HAL_MDP_PINGPONG_4},    // Main control Path in single pipe or Index 0 path in dual pipe
    {HAL_MDP_SOURCE_PIPE_VIG_3, HAL_MDP_CONTROL_PATH_3,    HAL_MDP_LAYER_MIXER_5,    HAL_MDP_DESTINATION_PIPE_NONE, HAL_MDP_PINGPONG_5},    // Index 1 path in dual pipe
  },
  // MDP_DISPLAY_EXTERNAL3
  {
    {HAL_MDP_SOURCE_PIPE_NONE,  HAL_MDP_CONTROL_PATH_NONE, HAL_MDP_LAYER_MIXER_NONE, HAL_MDP_DESTINATION_PIPE_NONE, HAL_MDP_PINGPONG_NONE}, // Main control Path in single pipe or Index 0 path in dual pipe
    {HAL_MDP_SOURCE_PIPE_NONE,  HAL_MDP_CONTROL_PATH_NONE, HAL_MDP_LAYER_MIXER_NONE, HAL_MDP_DESTINATION_PIPE_NONE, HAL_MDP_PINGPONG_NONE}, // Index 1 path in dual pipe
  },
};

/* Access macro for gMDP_InterfaceModeMap or gMDP_InterfaceModeMapController1Primary */
/* If the configuration is DSI 1 single and primary, then we need to swap b/w gMDP_InterfaceModeMap and gMDP_InterfaceModeMapController1Primary*/
#define MDP_GET_INTFMODEHMAP(_displayConnect_, _mixerId_, _dsi1prim_)   (_dsi1prim_ ? \
                                                                        (&(gMDP_InterfaceModeMapController1Primary[((_displayConnect_)<MDP_DISPLAY_CONNECT_MAX)?(_displayConnect_):MDP_DISPLAY_CONNECT_NONE][((_mixerId_)<MDP_QUADPIPE_NUM_MIXERS)?(_mixerId_):0])) : \
                                                                        (&(gMDP_InterfaceModeMap[((_displayConnect_)<MDP_DISPLAY_CONNECT_MAX)?(_displayConnect_):MDP_DISPLAY_CONNECT_NONE][((_mixerId_)<MDP_QUADPIPE_NUM_MIXERS)?(_mixerId_):0])))

/* Map to find the interface mode and type associated with each display connection type */
MDP_DisplayInterfaceMapType gMDP_InterfaceModeMap[MDP_DISPLAY_CONNECT_MAX][MDP_QUADPIPE_NUM_MIXERS] =
{
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_NONE
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_NONE
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_EBI2
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_EBI2
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_LCDC},  // MDP_DISPLAY_CONNECT_LCDC
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_LCDC},  // MDP_DISPLAY_CONNECT_LCDC
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_PRIMARY
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_PRIMARY
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_EXTERNAL
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_EXTERNAL
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_ANALOG_TV (QDI_DISPLAY_CONNECT_TV)
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_ANALOG_TV (QDI_DISPLAY_CONNECT_TV)
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_LCDC
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_LCDC
  },
  {
    {HAL_MDP_INTERFACE_3,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_HDMI},  // MDP_DISPLAY_CONNECT_DTV (QDI_DISPLAY_CONNECT_HDMI)
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_HDMI},  // MDP_DISPLAY_CONNECT_DTV (QDI_DISPLAY_CONNECT_HDMI)
  },
  {
    {HAL_MDP_INTERFACE_1,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO
  },
  {
    {HAL_MDP_INTERFACE_1,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD
  },
  {
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO
    {HAL_MDP_INTERFACE_1,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO
  },
  {
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD
    {HAL_MDP_INTERFACE_1,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_LVDS},  // MDP_DISPLAY_CONNECT_LVDS
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_LVDS},  // MDP_DISPLAY_CONNECT_LVDS
  },
  {
    {HAL_MDP_INTERFACE_MEMORY_WB_LINEAR_MODE, HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_FRAMEBUFFER_WRITEBACK    
    {HAL_MDP_INTERFACE_NONE,                  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_FRAMEBUFFER_WRITEBACK
  },
  {
    {HAL_MDP_INTERFACE_0,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DP},    // MDP_DISPLAY_CONNECT_DP_0
    {HAL_MDP_INTERFACE_3,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DP},    // MDP_DISPLAY_CONNECT_DP_0
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_DBI
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_DBI
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MHL
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MHL
  },
  {
    {HAL_MDP_INTERFACE_4,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DP},    // MDP_DISPLAY_CONNECT_DP_1
    {HAL_MDP_INTERFACE_3,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DP},    // MDP_DISPLAY_CONNECT_DP_1
  },
  {
    {HAL_MDP_INTERFACE_5,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_eDP},    // MDP_DISPLAY_CONNECT_EDP
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},   // MDP_DISPLAY_CONNECT_EDP
  },
};

/* Map to find the interface mode and type associated with each display connection type */
/* This handles the case where DSI1 is in single DSI mode and is primary. Correctly setting up the interface id */
MDP_DisplayInterfaceMapType gMDP_InterfaceModeMapController1Primary[MDP_DISPLAY_CONNECT_MAX][MDP_QUADPIPE_NUM_MIXERS] =
{
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_NONE
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_NONE
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_EBI2
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_EBI2
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_LCDC},  // MDP_DISPLAY_CONNECT_LCDC
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_LCDC},  // MDP_DISPLAY_CONNECT_LCDC
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_PRIMARY
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_PRIMARY
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_EXTERNAL
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_EXTERNAL
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_ANALOG_TV (QDI_DISPLAY_CONNECT_TV)
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_ANALOG_TV (QDI_DISPLAY_CONNECT_TV)
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_LCDC
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MDDI_LCDC
  },
  {
    {HAL_MDP_INTERFACE_3,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_HDMI},  // MDP_DISPLAY_CONNECT_DTV (QDI_DISPLAY_CONNECT_HDMI)
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_HDMI},  // MDP_DISPLAY_CONNECT_DTV (QDI_DISPLAY_CONNECT_HDMI)
  },
  {// If DSI1 is primary then we need to configure the interface 2 instead of interface 1
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO
  },
  {// If DSI1 is primary then we need to configure the interface 2 instead of interface 1
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD
  },
  {
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO
  },
  {
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD
    {HAL_MDP_INTERFACE_2,     HAL_MDP_INTERFACE_MODE_COMMAND, HAL_MDP_INTERFACE_STANDARD_DSI},   // MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_LVDS},  // MDP_DISPLAY_CONNECT_LVDS
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_LVDS},  // MDP_DISPLAY_CONNECT_LVDS
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_FRAMEBUFFER_WRITEBACK    
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_FRAMEBUFFER_WRITEBACK
  },
  {
    {HAL_MDP_INTERFACE_0,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DP},    // MDP_DISPLAY_CONNECT_DP_0
    {HAL_MDP_INTERFACE_3,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DP},    // MDP_DISPLAY_CONNECT_DP_0
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_DBI
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_DBI
  },
  {
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MHL
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},  // MDP_DISPLAY_CONNECT_MHL
  },
  {
    {HAL_MDP_INTERFACE_4,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DP},    // MDP_DISPLAY_CONNECT_DP_1
    {HAL_MDP_INTERFACE_3,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_DP},    // MDP_DISPLAY_CONNECT_DP_1
  },
  {
    {HAL_MDP_INTERFACE_5,     HAL_MDP_INTERFACE_MODE_VIDEO,   HAL_MDP_INTERFACE_STANDARD_eDP},    // MDP_DISPLAY_CONNECT_EDP
    {HAL_MDP_INTERFACE_NONE,  HAL_MDP_INTERFACE_MODE_NONE,    HAL_MDP_INTERFACE_STANDARD_NONE},   // MDP_DISPLAY_CONNECT_EDP
  },
};


/* Pixel format mapping */
const MDP_PixelFormatMapType gMDP_PixelFormatMap[MDP_PIXEL_FORMAT_MAX] = 
{
   {0,   0, FALSE,  HAL_MDP_PIXEL_FORMAT_NONE},                    // MDP_PIXEL_FORMAT_NONE
   {16,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_RGB_565_16BPP},           // MDP_PIXEL_FORMAT_RGB_565_16BPP
   {18,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_RGB_666_18BPP},           // MDP_PIXEL_FORMAT_RGB_666_18BPP
   {24,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_RGB_888_24BPP},           // MDP_PIXEL_FORMAT_RGB_888_24BPP
   {16,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_ARGB_1555_16BPP},         // MDP_PIXEL_FORMAT_ARGB_1555_16BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_XRGB_8888_32BPP},         // MDP_PIXEL_FORMAT_XRGB_8888_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_ARGB_8888_32BPP},         // MDP_PIXEL_FORMAT_ARGB_8888_32BPP
   {16,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_BGR_565_16BPP},           // MDP_PIXEL_FORMAT_BGR_565_16BPP
   {24,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_BGR_888_24BPP},           // MDP_PIXEL_FORMAT_BGR_888_24BPP
   {16,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_ABGR_1555_16BPP},         // MDP_PIXEL_FORMAT_ABGR_1555_16BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_XBGR_8888_32BPP},         // MDP_PIXEL_FORMAT_XBGR_8888_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_ABGR_8888_32BPP},         // MDP_PIXEL_FORMAT_ABGR_8888_32BPP
   {12,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP},       // MDP_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP
   {12,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP},       // MDP_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP
   {12,  1, TRUE,   HAL_MDP_PIXEL_FORMAT_YCRYCB_H2V1_16BPP},       // MDP_PIXEL_FORMAT_YCRYCB_H2V1_16BPP
   {16,  1, TRUE,   HAL_MDP_PIXEL_FORMAT_YCBYCR_H2V1_16BPP},       // MDP_PIXEL_FORMAT_YCBYCR_H2V1_16BPP
   {16,  1, TRUE,   HAL_MDP_PIXEL_FORMAT_CRYCBY_H2V1_16BPP},       // MDP_PIXEL_FORMAT_CRYCBY_H2V1_16BPP
   {16,  1, TRUE,   HAL_MDP_PIXEL_FORMAT_CBYCRY_H2V1_16BPP},       // MDP_PIXEL_FORMAT_CBYCRY_H2V1_16BPP
   {16,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP},       // MDP_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP
   {16,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP},       // MDP_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP
   {12,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP},   // MDP_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP
   {12,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP},   // MDP_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP
   {12,  3, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP},      // MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP
   {16,  3, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP},      // MDP_PIXEL_FORMAT_Y_CR_CB_H2V2_16BPP
   {0,   0, FALSE,  HAL_MDP_PIXEL_FORMAT_NONE},                    // MDP_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP
   {16,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_RGBA_5551_16BPP},         // MDP_PIXEL_FORMAT_RGBA_5551_16BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_RGBA_8888_32BPP},         // MDP_PIXEL_FORMAT_RGBA_8888_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_XRGB_2101010_32BPP},      // MDP_PIXEL_FORMAT_XRGB_2101010_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_ARGB_2101010_32BPP},      // MDP_PIXEL_FORMAT_ARGB_2101010_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_XBGR_2101010_32BPP},      // MDP_PIXEL_FORMAT_XBGR_2101010_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_ABGR_2101010_32BPP},      // MDP_PIXEL_FORMAT_ABGR_2101010_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_RGBX_1010102_32BPP},      // MDP_PIXEL_FORMAT_RGBX_1010102_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_RGBA_1010102_32BPP},      // MDP_PIXEL_FORMAT_RGBA_1010102_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_BGRX_1010102_32BPP},      // MDP_PIXEL_FORMAT_BGRX_1010102_32BPP
   {32,  1, FALSE,  HAL_MDP_PIXEL_FORMAT_BGRA_1010102_32BPP},      // MDP_PIXEL_FORMAT_BGRA_1010102_32BPP
   {24,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_P010_24BPP},  // MDP_PIXEL_FORMAT_Y_CRCB_H2V2_P010_24BPP
   {24,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_P010_24BPP},  // MDP_PIXEL_FORMAT_Y_CBCR_H2V2_P010_24BPP
   {16,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CRCB_H2V2_TP10_16BPP},  // MDP_PIXEL_FORMAT_Y_CRCB_H2V2_TP10_16BPP
   {16,  2, TRUE,   HAL_MDP_PIXEL_FORMAT_Y_CBCR_H2V2_TP10_16BPP},  // MDP_PIXEL_FORMAT_Y_CBCR_H2V2_TP10_16BPP
};

/* Access macro for gMDP_SourcePipeMap */
#define MDP_GET_SOURCEPIPE_TYPE(_sourcePipe_)   (gMDP_SourcePipeMap[((_sourcePipe_)<HAL_MDP_SOURCE_PIPE_MAX)?(_sourcePipe_):HAL_MDP_SOURCE_PIPE_NONE])

/* Source pipe mapping table */
const MDP_SourcePipeInfoType gMDP_SourcePipeMap[HAL_MDP_SOURCE_PIPE_MAX] = 
{
  {HAL_MDP_LAYER_TYPE_NONE},        // HAL_MDP_SOURCE_PIPE_NONE  
  {HAL_MDP_LAYER_TYPE_VIG},         // HAL_MDP_SOURCE_PIPE_VIG_0
  {HAL_MDP_LAYER_TYPE_VIG},         // HAL_MDP_SOURCE_PIPE_VIG_1
  {HAL_MDP_LAYER_TYPE_VIG},         // HAL_MDP_SOURCE_PIPE_VIG_2
  {HAL_MDP_LAYER_TYPE_VIG},         // HAL_MDP_SOURCE_PIPE_VIG_3
  {HAL_MDP_LAYER_TYPE_VIG},         // HAL_MDP_SOURCE_PIPE_VIG_4
  {HAL_MDP_LAYER_TYPE_RGB},         // HAL_MDP_SOURCE_PIPE_RGB_0
  {HAL_MDP_LAYER_TYPE_RGB},         // HAL_MDP_SOURCE_PIPE_RGB_1
  {HAL_MDP_LAYER_TYPE_RGB},         // HAL_MDP_SOURCE_PIPE_RGB_2
  {HAL_MDP_LAYER_TYPE_RGB},         // HAL_MDP_SOURCE_PIPE_RGB_3
  {HAL_MDP_LAYER_TYPE_RGB},         // HAL_MDP_SOURCE_PIPE_RGB_4
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_0_REC0
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_1_REC0
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_2_REC0
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_3_REC0
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_4_REC0
  {HAL_MDP_LAYER_TYPE_SSPP_CURSOR}, // HAL_MDP_SOURCE_PIPE_CURSOR_0
  {HAL_MDP_LAYER_TYPE_SSPP_CURSOR}, // HAL_MDP_SOURCE_PIPE_CURSOR_1
  {HAL_MDP_LAYER_TYPE_SSPP_CURSOR}, // HAL_MDP_SOURCE_PIPE_CURSOR_2
  {HAL_MDP_LAYER_TYPE_SSPP_CURSOR}, // HAL_MDP_SOURCE_PIPE_CURSOR_3
  {HAL_MDP_LAYER_TYPE_SSPP_CURSOR}, // HAL_MDP_SOURCE_PIPE_CURSOR_4
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_0_REC1
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_1_REC1
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_2_REC1
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_3_REC1
  {HAL_MDP_LAYER_TYPE_DMA},         // HAL_MDP_SOURCE_PIPE_DMA_4_REC1
};

/* Control Path modules map */
const MDP_ModulesMapType gMDP_ModulesMap[HAL_MDP_PINGPONG_MAX] =
{
  {HAL_MDP_DSC_NONE, HAL_MDP_3DMUX_NONE},  //HAL_MDP_PINGPONG_NONE
  {HAL_MDP_DSC_0,    HAL_MDP_3DMUX_0},     //HAL_MDP_PINGPONG_0
  {HAL_MDP_DSC_1,    HAL_MDP_3DMUX_0},     //HAL_MDP_PINGPONG_1
  {HAL_MDP_DSC_2,    HAL_MDP_3DMUX_1},     //HAL_MDP_PINGPONG_2
  {HAL_MDP_DSC_3,    HAL_MDP_3DMUX_1},     //HAL_MDP_PINGPONG_3
  {HAL_MDP_DSC_4,    HAL_MDP_3DMUX_2},     //HAL_MDP_PINGPONG_4
  {HAL_MDP_DSC_5,    HAL_MDP_3DMUX_2},     //HAL_MDP_PINGPONG_5
  {HAL_MDP_DSC_1,    HAL_MDP_3DMUX_NONE},  //HAL_MDP_PINGPONG_6
};

/* Default CXO input frequency */
#define MDP_DEFAULT_CXO_CLK                              19200000

/* Default constants for QSEED3 */
#define MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_BITS             (21)
#define MDP_QSEED3_FIR_PIXEL_UNIT_SCALE                  (1 << MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_BITS)
#define MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_MASK             (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE - 1)
#define MDP_QSEED3_FIR_PHASE_UNIT_SCALE_BITS             (15)
#define MDP_QSEED3_FIR_PHASE_UNIT_SCALE                  (1 << MDP_QSEED3_FIR_PHASE_UNIT_SCALE_BITS)
#define MDP_QSEED3_FIR_PHASE_RESIDUAL                    (1 << (MDP_QSEED3_FIR_PHASE_UNIT_SCALE_BITS - 1))

#define MDP_QSEED3_LUT_INDEX_0                           (2) 

#define MDP_QSEED3_TAPS_X                                (2)
#define MDP_QSEED3_TAPS_Y                                (1)
#define MDP_QSEED3_FILTER_TAPS                           (4)

#define MDP_QSEED3_SCALE_METHOD_SEPARABLE                (2)
#define MDP_QSEED3_SCALE_METHOD_CIRCULAR                 (1)
#define MDP_QSEED3_SCALE_METHOD_EDGE                     (0)

#define MDP_QSEED3_BLEND_FILTER_SEPARABLE                (1)
#define MDP_QSEED3_BLEND_FILTER_CIRCULAR                 (0)

#define MDP_QSEED3_ALPHA_MODE_BILINEAR                   (1)


/* Default values for TE */
// default vsync continue threshold for smart panel, same as default in KMD
// by default, the difference between read pointer and write pointer is 5 lines
#define MDP_DEFAULT_VSYNC_CONTINUE_LINES                 (5)

// default line divisor for smart panel to calculate vsync start threshold, same as default in KMD
// by default, TE start_threshold will be 4 lines
#define MDP_DEFAULT_VSYNC_START_LINES                    (4)

// Desired number of Prefill lines for programmable fetch start
#define MDP_PROG_FETCH_PREFILL_LINES                     (35)

// by default, TE height will be set to 0xfff0
#define MDP_DEFAULT_TE_HEIGHT_MAX                        (0xFFFFFFFF)

/*
 * 1) uCompressioRatio is used at host side to calculate frame timing since BMP logo is 8 bits per component
 * 2) (uBitsPerComponent * 3 / uBitsPerPixel) is the real dsc compression ratio at panel side
 */
const MDP_DscProfileMode gDscProfileModes[MDP_DSC_PROFILEID_MAX] =
{
  /* uEncodersNum    bLMSplitEnable   bBlockPredEnable  uBitsPerComponent   uBitsPerPixel  uCompressionRatio */
  {  1,              FALSE,           1,                8,                  12,            2 },                  /* ProfileID = 0x00*/
  {  1,              TRUE,            1,                8,                  12,            2 },                  /* ProfileID = 0x01*/
  {  2,              FALSE,           1,                8,                  12,            2 },                  /* ProfileID = 0x02*/
  {  2,              TRUE,            1,                8,                  12,            2 },                  /* ProfileID = 0x03*/
  {  1,              FALSE,           1,                8,                  8,             3 },                  /* ProfileID = 0x04, 1 pipe -> 1 mixer-> 1 dsc  -> 1 dsi => per intf */
  {  1,              TRUE,            1,                8,                  8,             3 },                  /* ProfileID = 0x05, 2 pipe -> 2 mixer-> 3d mux -> 1 dsc -> 1 ds1 -> 1 intf */
  {  2,              FALSE,           1,                8,                  8,             3 },                  /* ProfileID = 0x06, 1 pipe -> 1 mixer -> 1 dsc -> 1 dsi => per intf */
  {  2,              TRUE,            1,                8,                  8,             3 },                  /* ProfileID = 0x07, 2 pipe -> 2 mixer -> 2 dsc -> 1 dsi -> 1 intf */
  {  1,              FALSE,           1,                10,                 8,             3 },                  /* ProfileID = 0x08*/
  {  1,              TRUE,            1,                10,                 8,             3 },                  /* ProfileID = 0x09*/
  {  2,              FALSE,           1,                10,                 8,             3 },                  /* ProfileID = 0x0A*/
  {  2,              TRUE,            1,                10,                 8,             3 },                  /* ProfileID = 0x0B*/
  {  1,              FALSE,           1,                10,                 10,            3 },                  /* ProfileID = 0x0C*/
  {  1,              TRUE,            1,                10,                 10,            3 },                  /* ProfileID = 0x0D*/
  {  2,              FALSE,           1,                10,                 10,            3 },                  /* ProfileID = 0x0E*/
  {  2,              TRUE,            1,                10,                 10,            3 },                  /* ProfileID = 0x0F*/
  {  4,              TRUE,            1,                8,                  8,             3 },                  /* ProfileID = 0x10*/
};

/* Access macro for gMDP_DualPipe3DMuxFlag */
#define MDP_GET_DUALPIPEMUX(_displayConnect_)   (gMDP_DualPipe3DMuxFlag[((_displayConnect_)<MDP_DISPLAY_CONNECT_MAX)?(_displayConnect_):MDP_DISPLAY_CONNECT_NONE])

const bool32 gMDP_DualPipe3DMuxFlag[MDP_DISPLAY_CONNECT_MAX] = 
{
  FALSE,      //MDP_DISPLAY_CONNECT_NONE
  FALSE,      //MDP_DISPLAY_CONNECT_EBI2
  FALSE,      //MDP_DISPLAY_CONNECT_LCDC
  FALSE,      //MDP_DISPLAY_CONNECT_MDDI_PRIMARY
  FALSE,      //MDP_DISPLAY_CONNECT_MDDI_EXTERNAL
  FALSE,      //MDP_DISPLAY_CONNECT_ANALOG_TV = MDP_DISPLAY_CONNECT_TV
  FALSE,      //MDP_DISPLAY_CONNECT_MDDI_LCDC
  TRUE,       //MDP_DISPLAY_CONNECT_HDMI = MDP_DISPLAY_CONNECT_DTV
  FALSE,      //MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO
  FALSE,      //MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD
  FALSE,      //MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO
  FALSE,      //MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD
  FALSE,      //MDP_DISPLAY_CONNECT_LVDS
  FALSE,      //MDP_DISPLAY_CONNECT_FRAMEBUFFER_WRITEBACK
  TRUE,       //MDP_DISPLAY_CONNECT_DP_0 = MDP_DISPLAY_CONNECT_DP
  FALSE,      //MDP_DISPLAY_CONNECT_DBI
  FALSE,      //MDP_DISPLAY_CONNECT_MHL
  TRUE,       //MDP_DISPLAY_CONNECT_DP_1
  TRUE,       //MDP_DISPLAY_CONNECT_EDP
};

/*=========================================================================
     Local MDP Lib Helper functions
==========================================================================*/
/* displayCommandModeTearCheckConfig */
static void displayCommandModeTearCheckConfig(MDP_Panel_AttrType *pPanelAttr, HAL_MDP_Interface_TEConfigType *pTEConfig, uint32 uPathIndex);

/* displayCommandModeVsyncConfig */
static void displayCommandModeVsyncConfig(MDP_Panel_AttrType *pPanelAttr, HAL_MDP_Interface_VsyncConfigType *pVsyncConfig, uint32 uPathIndex);



/* -----------------------------------------------------------------------
*
** FUNCTION: GetPingPongCaps()
*/
/*!
** DESCRIPTION:
**      Retrieve the ping-pong caps based on ePingPongID
**
** INPUT:
**      ePingPongID       PP ID
**
**
** ----------------------------------------------------------------------- */
const MDP_PingPongCaps *GetPingPongCaps(HAL_MDP_PingPongId ePingPongID)
{
    const MDP_PingPongCaps *pPPCaps            = NULL;
    MDP_HwPrivateInfo      *psMDPHwPrivateInfo = MDP_GETPRIVATEINFO();

    if (ePingPongID < HAL_MDP_PINGPONG_MAX)
    {
        pPPCaps = (const MDP_PingPongCaps *)&psMDPHwPrivateInfo->pDeviceCaps->pPingPongCaps[ePingPongID];
    }

    return pPPCaps;
}

/* -----------------------------------------------------------------------
*
** FUNCTION: SplitDisplayInit()
*/
/*!
** DESCRIPTION:
**      This function configures params for pingpong buffer split
**
** ----------------------------------------------------------------------- */
static MDP_Status SplitDisplayInit(MDP_Panel_AttrType *pPanelConfig)
{
    MDP_Status           eStatus = MDP_STATUS_OK;

    if ((pPanelConfig->uNumInterfaces) &&
        (pPanelConfig->uNumMixers))
    {
        // one mixer, one ping-pong buffer but two interfaces
        if ((MDP_SINGLEPIPE_NUM_MIXERS == pPanelConfig->uNumMixers) &&
            (MDP_INTERFACE_DUAL        == pPanelConfig->uNumInterfaces))
        {
            // This scenario can be supported by PP Split and should be applied only if PP SPlit is supported by the HW
            HAL_MDP_PingPongId         ePingPongId   = pPanelConfig->sDisplayControlPath[MDP_MASTER_PATH_INDEX].ePingPongId;
            MDP_PingPongCaps          *pPingPongCaps = (MDP_PingPongCaps *)GetPingPongCaps(ePingPongId);
            HAL_MDP_SplitMUXId         eSplitMuxId   = (ePingPongId <= HAL_MDP_PINGPONG_1)?HAL_MDP_SPLITMUX_0:HAL_MDP_SPLITMUX_1;

            // Check whether ping-pong split in supported in the chip
            if ((NULL != pPingPongCaps)                                                 &&
                (MDP_PINGPONG_CAP_PINGPONG_SPLIT      & pPingPongCaps->uPPFeatureFlags) &&
                (MDP_PANEL_FLAG_ENABLE_PINGPONG_SPLIT & pPanelConfig->eDisplayFlags  ))
            {
                HAL_MDP_SplitMUXConfigType            sSplitMuxConfig;
                HAL_MDP_SplitMUX_BufferConfigType     sSplitMuxBufferConfig;

                MDP_OSAL_MEMZERO(&sSplitMuxConfig, sizeof(HAL_MDP_SplitMUXConfigType));
                MDP_OSAL_MEMZERO(&sSplitMuxBufferConfig, sizeof(HAL_MDP_SplitMUX_BufferConfigType));

                // Enable PP Split
                // sDisplayControlPath[1] has the secondary interface info
                sSplitMuxBufferConfig.bSplitFifoEnable = TRUE;
                sSplitMuxBufferConfig.eSecondaryIntfId = pPanelConfig->sDisplayControlPath[MDP_SLAVE_PATH_INDEX].eInterfaceId;
                sSplitMuxConfig.pSplitMUXBufferConfig  = &sSplitMuxBufferConfig;

                if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_SplitMUX_Setup(eSplitMuxId, &sSplitMuxConfig, 0x00))
                {
                    MDP_Log_Message(MDP_LOGLEVEL_WARN, "SplitDisplayInit() failed to configure SplitMux ! (SplitMux ID = %d)\n", eSplitMuxId);
                    eStatus = MDP_STATUS_FAILED;
                }
            }
            else
            {
                MDP_Log_Message(MDP_LOGLEVEL_WARN, "SplitDisplayInit() failed: HW does not support ping-pong split (SplitMux ID = %d)\n", eSplitMuxId);
                eStatus = MDP_STATUS_NOT_SUPPORTED;
            }
        }
    }

    return eStatus;
}

/* -----------------------------------------------------------------------
**
** FUNCTION: UpdateMixerConfiguration()
**
** DESCRIPTION:
**      Helper function to update required number of mixers based on panel configuration.
** ----------------------------------------------------------------------- */
static MDP_Status UpdateMixerConfiguration(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status          eStatusRet         = MDP_STATUS_OK;
  MDP_HwPrivateInfo  *psMDPHwPrivateInfo = MDP_GETPRIVATEINFO();

  /* Initialization for single control path */
  pPanelConfig->uNumMixers = MDP_SINGLEPIPE_NUM_MIXERS;

  /* check if ping pong split is available */
  if ((TRUE == pPanelConfig->bDetected)         &&
      (NULL != psMDPHwPrivateInfo->pDeviceCaps) &&
      (NULL != psMDPHwPrivateInfo->pDeviceCaps->pPingPongCaps))
  {
    MDP_PingPongCaps            *pPingPongCaps[MDP_QUADPIPE_NUM_MIXERS];
    MDP_DisplayCtrlPathMapType  *pControlPathMap[MDP_QUADPIPE_NUM_MIXERS];
    uint32                       uMaxPPWidth;

    /* get control path mapping */
    pControlPathMap[MDP_MASTER_PATH_INDEX]   = (MDP_DisplayCtrlPathMapType *)MDP_GET_CTRLPATHMAP(pPanelConfig->eDisplayId, MDP_MASTER_PATH_INDEX);
    pControlPathMap[MDP_SLAVE_PATH_INDEX]    = (MDP_DisplayCtrlPathMapType *)MDP_GET_CTRLPATHMAP(pPanelConfig->eDisplayId, MDP_SLAVE_PATH_INDEX);

    /* get the pingpong cap for each path */
    pPingPongCaps[MDP_MASTER_PATH_INDEX]     = (MDP_PingPongCaps *)GetPingPongCaps(pControlPathMap[MDP_MASTER_PATH_INDEX]->ePingPongId);
    pPingPongCaps[MDP_SLAVE_PATH_INDEX]      = (MDP_PingPongCaps *)GetPingPongCaps(pControlPathMap[MDP_SLAVE_PATH_INDEX]->ePingPongId);

    /* max pingpong buffer width for the pingpong on master path */
    uMaxPPWidth = pPingPongCaps[MDP_MASTER_PATH_INDEX]->uMaxPPWidth;

    /* Set control path map and pingpong caps for slave path2 and path3 in quad pipe mode */
    if (pPanelConfig->eDisplayFlags & MDP_PANEL_FLAG_ENABLE_QUAD_PIPE)
    {
      pControlPathMap[MDP_SLAVE_PATH2_INDEX]  = (MDP_DisplayCtrlPathMapType *)MDP_GET_CTRLPATHMAP(pPanelConfig->eDisplayId, MDP_SLAVE_PATH2_INDEX);
      pControlPathMap[MDP_SLAVE_PATH3_INDEX]  = (MDP_DisplayCtrlPathMapType *)MDP_GET_CTRLPATHMAP(pPanelConfig->eDisplayId, MDP_SLAVE_PATH3_INDEX);
      pPingPongCaps[MDP_SLAVE_PATH2_INDEX]    = (MDP_PingPongCaps *)GetPingPongCaps(pControlPathMap[MDP_SLAVE_PATH2_INDEX]->ePingPongId);
      pPingPongCaps[MDP_SLAVE_PATH3_INDEX]    = (MDP_PingPongCaps *)GetPingPongCaps(pControlPathMap[MDP_SLAVE_PATH3_INDEX]->ePingPongId);

      pPanelConfig->uNumMixers                = MDP_QUADPIPE_NUM_MIXERS;
    }
    /* if panel width is greater than the pingpong buffer width*/
    else if (pPanelConfig->uDisplayWidth > uMaxPPWidth)
    {
      /* in this case, we will need to use two pingpong buffers, check if pingpong buffer is supported on slave path */
      if (FALSE == pPingPongCaps[MDP_SLAVE_PATH_INDEX]->bSupported)
      {
        eStatusRet = MDP_STATUS_NO_RESOURCES;
      }
      else
      {
        /* if two pingpong buffers are supported, two mixers will be used */
        pPanelConfig->uNumMixers = MDP_DUALPIPE_NUM_MIXERS;
      }
    }
    else
    {
      /* for case that panel width is less than pingpong buffer width */
      if (MDP_INTERFACE_SINGLE == pPanelConfig->uNumInterfaces)
      {
        if (pPanelConfig->uAttrs.sDsi.bDSCEnable && pPanelConfig->uAttrs.sDsi.bDSCLayerMixSplit)
        {
          /* If panel has DSC mode and supports layer mixer split then we can have uNumMixers = Dual */
          pPanelConfig->uNumMixers = MDP_DUALPIPE_NUM_MIXERS;
        }
        else
        {
          /* if panel only requires single interface, then only use single control path */
          pPanelConfig->uNumMixers = MDP_SINGLEPIPE_NUM_MIXERS;
        }
      }
      else
      {
        /* if panel requires dual interfaces */
        if ((MDP_PINGPONG_CAP_PINGPONG_SPLIT      & pPingPongCaps[MDP_MASTER_PATH_INDEX]->uPPFeatureFlags) &&
            (MDP_PANEL_FLAG_ENABLE_PINGPONG_SPLIT & pPanelConfig->eDisplayFlags))
        {
          /* if pingpong buffer split is supported, then use single control path with pingpong buffer split */
          pPanelConfig->uNumMixers = MDP_SINGLEPIPE_NUM_MIXERS;
        }
        else
        {
          /* since pingpong buffer split is not supported, we will need to use two control paths */
          if (FALSE == pPingPongCaps[MDP_SLAVE_PATH_INDEX]->bSupported)
          {
            eStatusRet = MDP_STATUS_NO_RESOURCES;
          }
          else
          {
            pPanelConfig->uNumMixers = MDP_DUALPIPE_NUM_MIXERS;
          }
        }
      }
    }
  }

  return eStatusRet;
}

/* -----------------------------------------------------------------------
**
** FUNCTION: UpdateControlPathAllocation()
**
** DESCRIPTION:
**      Update display control path allocation according to the num of mixers for primary and secondary display.
** ----------------------------------------------------------------------- */
static void UpdateControlPathAllocation(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Panel_AttrType  *pPriPanelConfig = MDP_GET_DISPLAYINFO(MDP_DISPLAY_PRIMARY);

  if ((MDP_DISPLAY_SECONDARY   == pPanelConfig->eDisplayId)    &&
     ((MDP_DUALPIPE_NUM_MIXERS == pPriPanelConfig->uNumMixers) ||
      (MDP_DUALPIPE_NUM_MIXERS == pPanelConfig->uNumMixers)))
  {
    uint32 uPathIndex;

    // Secondary display should use LM2/LM3 if primary or secondary display use dual pipe.
    for (uPathIndex = 0; uPathIndex < MDP_DUALPIPE_NUM_MIXERS; uPathIndex++)
    {
      gMDP_DisplayControlPathMap[MDP_DISPLAY_SECONDARY][uPathIndex].eSourcePipeId++;
      gMDP_DisplayControlPathMap[MDP_DISPLAY_SECONDARY][uPathIndex].eLayerMixerId++;
      gMDP_DisplayControlPathMap[MDP_DISPLAY_SECONDARY][uPathIndex].eDestinationPipeId++;
      gMDP_DisplayControlPathMap[MDP_DISPLAY_SECONDARY][uPathIndex].ePingPongId ++;
    }
  }
}

/* -----------------------------------------------------------------------
**
** FUNCTION: UpdateDisplayControlpathInfo()
**
** DESCRIPTION:
**      1. Finds number of mixers required for this display
**      2. Assigns control path information
**      3. Configures ping-pong split if required
** ----------------------------------------------------------------------- */
static MDP_Status UpdateDisplayControlpathInfo(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status          eStatusRet         = MDP_STATUS_OK;
  MDP_HwPrivateInfo  *psMDPHwPrivateInfo = MDP_GETPRIVATEINFO();

  if (MDP_STATUS_OK != (eStatusRet = UpdateMixerConfiguration(pPanelConfig)))
  {
    MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: Update mixer configuration failed with status (%d)!\n", eStatusRet);
  }
  else if (MDP_DISPLAY_SECONDARY == pPanelConfig->eDisplayId)
  {
    MDP_Panel_AttrType      *pPriPanelConfig  = MDP_GET_DISPLAYINFO(MDP_DISPLAY_PRIMARY);

    // Primary display is in disabled state, then update the number of mixers reserved for it. 
    if (0 == pPriPanelConfig->uNumMixers)
    {
      eStatusRet = UpdateMixerConfiguration(pPriPanelConfig);
    }

    if (MDP_STATUS_OK == eStatusRet)
    {
      UpdateControlPathAllocation(pPanelConfig);
    }
    else
    {
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: Update mixer configuration failed with status (%d)!\n", eStatusRet);
    }
  }

  if (MDP_STATUS_OK == eStatusRet)
  {
    uint32  uI = 0;


    if (MDP_QUADPIPE_NUM_MIXERS == pPanelConfig->uNumMixers)
    {
      MDP_DisplayInterfaceMapType    *pMasterInterfaceMap = (MDP_DisplayInterfaceMapType *)MDP_GET_INTFMODEHMAP(pPanelConfig->ePhysConnect,
                                                                                                                MDP_MASTER_PATH_INDEX,
                                                                                                                pPanelConfig->uAttrs.sDsi.uControllerSinglePrimary);
      MDP_DisplayInterfaceMapType    *pSlave1InterfaceMap = (MDP_DisplayInterfaceMapType *)MDP_GET_INTFMODEHMAP(pPanelConfig->ePhysConnect,
                                                                                                                MDP_SLAVE_PATH_INDEX,
                                                                                                                pPanelConfig->uAttrs.sDsi.uControllerSinglePrimary);

      // Slave path 1 uses same interface as master path in quad pipe mode
      pSlave1InterfaceMap->eInterfaceId = pMasterInterfaceMap->eInterfaceId;
    }

    // Populate the control path info
    // uNumMixers need not be equal to uNumInterfaces
    // Ensure that control path info is updated to take into account for both mixers and interfaces
    for (uI = 0; uI < pPanelConfig->uNumMixers; uI++)
    {
      MDP_DisplayCtrlPathMapType     *pDisplayControlPath = (MDP_DisplayCtrlPathMapType *)MDP_GET_CTRLPATHMAP(pPanelConfig->eDisplayId, uI);
      MDP_DisplayInterfaceMapType    *pInterfaceMap       = (MDP_DisplayInterfaceMapType *)MDP_GET_INTFMODEHMAP(pPanelConfig->ePhysConnect,
                                                                                                                uI,
                                                                                                                pPanelConfig->uAttrs.sDsi.uControllerSinglePrimary);

      pPanelConfig->sDisplayControlPath[uI].eControlPathId     = pDisplayControlPath->eControlPathId;
      pPanelConfig->sDisplayControlPath[uI].eSourcePipeId      = pDisplayControlPath->eSourcePipeId;
      pPanelConfig->sDisplayControlPath[uI].eLayerMixerId      = pDisplayControlPath->eLayerMixerId;
      pPanelConfig->sDisplayControlPath[uI].eDestinationPipeId = pDisplayControlPath->eDestinationPipeId;
      pPanelConfig->sDisplayControlPath[uI].eInterfaceId       = pInterfaceMap->eInterfaceId;
      pPanelConfig->sDisplayControlPath[uI].ePingPongId        = pDisplayControlPath->ePingPongId;
    }


    // Scenario requires ping pong split 
    // Identify the slave ping-pong
    if ((MDP_SINGLEPIPE_NUM_MIXERS == pPanelConfig->uNumMixers) &&
        (MDP_INTERFACE_DUAL        == pPanelConfig->uNumInterfaces))
    {
      if (NULL != psMDPHwPrivateInfo->pPPSplitSlaveMap)
      {
        HAL_MDP_PingPongId eSlavePingPongID = psMDPHwPrivateInfo->pPPSplitSlaveMap[pPanelConfig->sDisplayControlPath[MDP_MASTER_PATH_INDEX].ePingPongId];
        pPanelConfig->sDisplayControlPath[MDP_SLAVE_PATH_INDEX].ePingPongId = eSlavePingPongID;
      }
    }

    // Scenario requires DSC merge, 2 encoders (2 mixers single interface)
    if ((MDP_DUALPIPE_NUM_MIXERS == pPanelConfig->uNumMixers) &&
        (MDP_INTERFACE_SINGLE    == pPanelConfig->uNumInterfaces))
    {
      /* slave ctrl path's interface should be same as master ctrl path at dsc merge case */
      pPanelConfig->sDisplayControlPath[MDP_SLAVE_PATH_INDEX].eInterfaceId = pPanelConfig->sDisplayControlPath[MDP_MASTER_PATH_INDEX].eInterfaceId;
    }

    // Initialize any split display (source split or PP split) configuration if required 
    eStatusRet = SplitDisplayInit(pPanelConfig);
  }

  return eStatusRet;
}

/****************************************************************************
*
** FUNCTION: SetupCroppingRectangle()
*/
/*!
* \brief
*   Local helper function to update the cropping rectangle information in Dual pipe/Single pipe scenario
*
* \param  [in] pPanelConfig             - Panel configuration
* \param  [in] pSrcRect                 - The source surface information
* \param  [in\Out] pCropRectInfo        - Cropping rectangle information updated in this function
* \param  [in] uPipeIndex               - Pipe index (only used in case of Dual path)
*
*
* \retval void
*
****************************************************************************/
static void SetupCroppingRectangle(MDP_Panel_AttrType        *pPanelConfig, 
                                   MDPRectType                  *pSrcRect, 
                                   HAL_MDP_CropRectangleType    *pCropRectInfo, 
                                   uint32                        uPipeIndex)
{
  uint32                            uSrcWidth           = 0;
  uint32                            uSrcHeight          = 0;
  uint32                            uDestPosX           = 0;
  uint32                            uDestPosY           = 0;
  uint32                            uDestWidth          = 0;
  uint32                            uDestHeight         = 0;
  MDP_PresentationMode_Type         eMode               = (PcdGet32(PcdPresentationMode)<MDP_PRESENTATIONMODE_MAX)?(MDP_PresentationMode_Type)PcdGet32(PcdPresentationMode):MDP_PRESENTATIONMODE_LETTERBOX;
  const MDP_DisplayCtrlPathMapType *pDisplayControlPath = MDP_GET_CTRLPATHMAP(pPanelConfig->eDisplayId, uPipeIndex);
  
  // Fallback to letterbox if the pipe cannot support scaling.
  if ((MDP_PRESENTATIONMODE_FIT == eMode) &&
      (HAL_MDP_LAYER_TYPE_VIG != MDP_GET_SOURCEPIPE_TYPE(pDisplayControlPath->eSourcePipeId).eLayerType))
  {
    eMode = MDP_PRESENTATIONMODE_LETTERBOX;
  }

  // Calculate source / destination rectangles based on the presentation mode.
  switch (eMode)
  {
    case MDP_PRESENTATIONMODE_FIT:
      {
        // Expand to the panel width/height
        uSrcWidth   = pSrcRect->uWidth;
        uSrcHeight  = pSrcRect->uHeight;
        uDestWidth  = pPanelConfig->uDisplayWidth;
        uDestHeight = pPanelConfig->uDisplayHeight;        
      }
      break;
    case MDP_PRESENTATIONMODE_LETTERBOX:  
    default:
      {
        // Calcuate the position of the rectangle, assume no scaling so we center the image within the display if there is a mismatch
        if (pPanelConfig->uDisplayWidth > pSrcRect->uWidth)
        {
          // If panel is wider then surface, only consider the surface and center it within the screen.
          uSrcWidth  = pSrcRect->uWidth;
          uDestWidth = uSrcWidth;
          uDestPosX  = (pPanelConfig->uDisplayWidth - pSrcRect->uWidth) / 2;  
        }
        else 
        {
          // Surface is equal to the panel width
          uSrcWidth  = pPanelConfig->uDisplayWidth;
          uDestWidth = uSrcWidth;          
          uDestPosX  = 0;
        }

        // Center the vertical plane as well 
        if (pPanelConfig->uDisplayHeight > pSrcRect->uHeight)
        {
          // If panel is wider then surface, only consider the surface and center it within the screen.
          uSrcHeight  = pSrcRect->uHeight;
          uDestHeight = uSrcHeight;
          uDestPosY   = (pPanelConfig->uDisplayHeight - pSrcRect->uHeight) / 2;  
        }
        else 
        {
          // Surface is equal to the panel height
          uSrcHeight  = pPanelConfig->uDisplayHeight;
          uDestHeight = uSrcHeight;          
          uDestPosY   = 0;
        }
    }
    break;
  }  
  
  if (MDP_QUADPIPE_NUM_MIXERS == pPanelConfig->uNumMixers)
  {
    // Source fetch. If panel is inverted we need to flip the left and right rectangle in source split mode.
    pCropRectInfo->sSrcRectConfig.uPosX = (MDP_ROTATE_180 == pPanelConfig->ePhysPanelOrientation ) ?
                                          ((MDP_QUADPIPE_NUM_MIXERS-uPipeIndex-1) * (uSrcWidth / 4) ):
                                          ((uPipeIndex) * (uSrcWidth / 4) );

    pCropRectInfo->sSrcRectConfig.uPosY           = 0;
    pCropRectInfo->sSrcRectConfig.uWidthInPixels  = (uSrcWidth/4);
    pCropRectInfo->sSrcRectConfig.uHeightInPixels = uSrcHeight;
    pCropRectInfo->sDstRectConfig.uPosX           = uDestPosX + ((uPipeIndex%2) * (uSrcWidth / 4));
    pCropRectInfo->sDstRectConfig.uPosY           = uDestPosY;
    pCropRectInfo->sDstRectConfig.uWidthInPixels  = (uDestWidth/4);
    pCropRectInfo->sDstRectConfig.uHeightInPixels = uDestHeight;
  } 
  else if (MDP_DUALPIPE_NUM_MIXERS == pPanelConfig->uNumMixers)
  {
    // We split the source surface into two equal halves
    // Left layer displays the left half and the Right layer displays right half 
    // We need to find the Layer at  the current index is left layer or right layer
    HAL_MDP_LayerMixerId              eLayerMixerId        = pPanelConfig->sDisplayControlPath[uPipeIndex].eLayerMixerId;
    HAL_MDP_LayerMixerId              eOtherLayerMixerId   = pPanelConfig->sDisplayControlPath[(uPipeIndex == 0) ? 1 : 0].eLayerMixerId;
    bool32                            bLeft                = (eLayerMixerId < eOtherLayerMixerId)? TRUE : FALSE;
    
    // Source fetch. If panel is inverted we need to flip the left and right rectangle in source split mode.
    pCropRectInfo->sSrcRectConfig.uPosX = (MDP_ROTATE_180 == pPanelConfig->ePhysPanelOrientation ) ?
                                          ((bLeft == TRUE) ? (uSrcWidth / 2) : 0) :
                                          ((bLeft == TRUE) ? 0 : (uSrcWidth / 2)) ;
    pCropRectInfo->sSrcRectConfig.uPosY           = 0;
    pCropRectInfo->sSrcRectConfig.uWidthInPixels  = (uSrcWidth/2);
    pCropRectInfo->sSrcRectConfig.uHeightInPixels = uSrcHeight;

    // Destination rect
    pCropRectInfo->sDstRectConfig.uPosX           = (bLeft == TRUE)? uDestPosX: (pPanelConfig->uDisplayWidth/2);
    pCropRectInfo->sDstRectConfig.uPosY           = uDestPosY;
    pCropRectInfo->sDstRectConfig.uWidthInPixels  = (uDestWidth/2);
    pCropRectInfo->sDstRectConfig.uHeightInPixels = uDestHeight;

  }
  else
  {
    // Source fetch  
    pCropRectInfo->sSrcRectConfig.uPosX           = 0;
    pCropRectInfo->sSrcRectConfig.uPosY           = 0;
    pCropRectInfo->sSrcRectConfig.uWidthInPixels  = uSrcWidth;
    pCropRectInfo->sSrcRectConfig.uHeightInPixels = uSrcHeight;

    // Destination rect    
    pCropRectInfo->sDstRectConfig.uPosX           = uDestPosX;
    pCropRectInfo->sDstRectConfig.uPosY           = uDestPosY;
    pCropRectInfo->sDstRectConfig.uWidthInPixels  = uDestWidth;
    pCropRectInfo->sDstRectConfig.uHeightInPixels = uDestHeight;
  }

}

/****************************************************************************
*
** FUNCTION: InterfaceFetchStartConfig()
*/
/*!
* \brief
*   Local helper function to setup the Programmable Fetch Configuration for the interface
*
* \param  [in]  pPanelConfig            - Panel configuration
* \param  [out] pProgFetchStartConfig   - Prog Fetch config to be updated
*
* \retval None
*
****************************************************************************/
static void InterfaceFetchStartConfig(MDP_Panel_AttrType *pPanelConfig, HAL_MDP_Interface_ProgFetchStartConfigType *pProgFetchStartConfig)
{
  if ((NULL != pProgFetchStartConfig) && (NULL != pPanelConfig))
  {
    bool32	 bEnablePFS        = TRUE;
    uint32	 uVFrontPorch      = pPanelConfig->sActiveTiming.uVsyncFrontPorchLns;
    uint32	 uVFPNotInPrefill  = 0;  // lines in VFP which are not used for prefetch
    uint32	 uPrefillLines     = pPanelConfig->sActiveTiming.uVsyncBackPorchLns +
                                 pPanelConfig->sActiveTiming.uVsyncPulseWidthLns;  // Default PrefillLines = VBP+VSP

    /* Check if PFS needs to be enabed */
    if (uPrefillLines >= MDP_PROG_FETCH_PREFILL_LINES)
    {
      // no need to enable Programmable fetch
      bEnablePFS = FALSE;
    }
    else if (uVFrontPorch + uPrefillLines <= MDP_PROG_FETCH_PREFILL_LINES)
    {
      // use the full VFP for pre-fill
      uVFPNotInPrefill = 0;
    }
    else
    {
      // use part of the VFP 
      uVFPNotInPrefill = uVFrontPorch + uPrefillLines - MDP_PROG_FETCH_PREFILL_LINES;
    }

    MDP_OSAL_MEMZERO((void *)pProgFetchStartConfig, sizeof(HAL_MDP_Interface_ProgFetchStartConfigType));

    if (bEnablePFS)
    {
      uint32 uHorzActive     = (MDP_INTERFACE_DUAL == pPanelConfig->uNumInterfaces) ? pPanelConfig->uDisplayWidth >> 1 :
                                                                                      pPanelConfig->uDisplayWidth;
      uint32 uStartFetchLine = uPrefillLines + pPanelConfig->uDisplayHeight + uVFPNotInPrefill;
      uint32 uHSyncPeriod    = 0;

      if ((pPanelConfig->uAttrs.sDsi.bDSCEnable) &&
         (pPanelConfig->uAttrs.sDsi.uDSCCompressionRatio))
      {
        // In DSC mode, the interface is only a pushing a compressed frame
        uHorzActive /= pPanelConfig->uAttrs.sDsi.uDSCCompressionRatio;
	  }

      uHSyncPeriod = pPanelConfig->sActiveTiming.uHsyncBackPorchDclk  +
                     pPanelConfig->sActiveTiming.uHsyncFrontPorchDclk +
                     pPanelConfig->sActiveTiming.uHsyncPulseWidthDclk +
                     uHorzActive;

      // Programmable Fetch lines is measured in #lines * hSyncPeriod, one more uHSyncPeriod delay is required for Hana.
      // + 1 is to make sure we are not in active period 
      pProgFetchStartConfig->uProgFetchStartValue  = (uStartFetchLine * uHSyncPeriod) + uHSyncPeriod + 1;
      pProgFetchStartConfig->bProgFetchStartEnable = TRUE;
    }
  }
}

/****************************************************************************
*
** FUNCTION: SetupInterface()
*/
/*!
* \brief
*   Local helper function to setup the MDP interface
*
* \param  [in] pPanelConfig          - Panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
static MDP_Status SetupInterface(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status                                 eStatus = MDP_STATUS_OK;
  MDP_DisplayInterfaceMapType               *pInterfaceMap;
  HAL_MDP_InterfaceConfigType                sInterfaceConfig;
  HAL_MDP_PhyIntf_TimingGenConfigType        sInterfaceTiming;
  HAL_MDP_Interface_DualInferfaceConfigType  sDualInterface;
  HAL_MDP_Interface_VsyncConfigType          sVsyncConfig;
  HAL_MDP_Interface_TEConfigType             sTEConfig;
  HAL_MDP_Interface_ProgFetchStartConfigType sProgFetchStartConfig;
  uint32                                     uI;

  InterfaceFetchStartConfig(pPanelConfig, &sProgFetchStartConfig);

  for (uI = 0; uI < pPanelConfig->uNumMixers; uI++)
  {
    uint32 uMDPSetupFlags = 0;

    // Ensure same interface will not be configured twice.
    if ((uI                                                    > MDP_MASTER_PATH_INDEX)  &&
        (pPanelConfig->sDisplayControlPath[uI-1].eInterfaceId == pPanelConfig->sDisplayControlPath[uI].eInterfaceId))
    {
      continue;
    }

    pInterfaceMap =  (MDP_DisplayInterfaceMapType*)MDP_GET_INTFMODEHMAP(pPanelConfig->ePhysConnect,
                                                                        uI,
                                                                        pPanelConfig->uAttrs.sDsi.uControllerSinglePrimary);

    if (HAL_MDP_INTERFACE_NONE == pPanelConfig->sDisplayControlPath[uI].eInterfaceId)
    {
      continue;
    }

    // 1. Setup interface configuration
    MDP_OSAL_MEMZERO(&sInterfaceConfig, sizeof(HAL_MDP_InterfaceConfigType));
    sInterfaceConfig.pIntrTimingGenConfig = &sInterfaceTiming;
 
    // 2 Setup interface timing
    MDP_OSAL_MEMZERO(&sInterfaceTiming, sizeof(HAL_MDP_PhyIntf_TimingGenConfigType));
    sInterfaceTiming.eInterfaceMode           = pInterfaceMap->eInterfaceMode;
    sInterfaceTiming.eInterfaceStandard       = pInterfaceMap->eInterfaceStandard;
  
    sInterfaceTiming.eInterfacePixelFormat    = MDP_GET_PIXELFMTMAP(pPanelConfig->eColorFormat).eHALPixelFormat;
    sInterfaceTiming.bInterlacedMode          = pPanelConfig->sActiveTiming.bInterlaced;
    sInterfaceTiming.uVisibleWidthInPixels    = pPanelConfig->uDisplayWidth;
    sInterfaceTiming.uVisibleHeightInPixels   = pPanelConfig->uDisplayHeight;
    sInterfaceTiming.uHsyncFrontPorchInPixels = pPanelConfig->sActiveTiming.uHsyncFrontPorchDclk;
    sInterfaceTiming.uHsyncBackPorchInPixels  = pPanelConfig->sActiveTiming.uHsyncBackPorchDclk;
    sInterfaceTiming.uHsyncPulseInPixels      = pPanelConfig->sActiveTiming.uHsyncPulseWidthDclk;
    sInterfaceTiming.uHysncSkewInPixels       = pPanelConfig->sActiveTiming.uHsyncSkewDclk;
    sInterfaceTiming.uVsyncFrontPorchInLines  = pPanelConfig->sActiveTiming.uVsyncFrontPorchLns;
    sInterfaceTiming.uVsyncBackPorchInLines   = pPanelConfig->sActiveTiming.uVsyncBackPorchLns;
    sInterfaceTiming.uVsyncPulseInLines       = pPanelConfig->sActiveTiming.uVsyncPulseWidthLns;
    sInterfaceTiming.uHLeftBorderInPixels     = pPanelConfig->sActiveTiming.uHLeftBorderDClk;
    sInterfaceTiming.uHRightBorderInPixels    = pPanelConfig->sActiveTiming.uHRightBorderDClk;
    sInterfaceTiming.uVTopBorderInLines       = pPanelConfig->sActiveTiming.uVTopBorderLines;
    sInterfaceTiming.uVBottomBorderInLines    = pPanelConfig->sActiveTiming.uVBottomBorderLines;
    sInterfaceTiming.uBorderColorInRGB888     = pPanelConfig->sActiveTiming.uBorderColorRgb888;
    sInterfaceTiming.uUnderflowColorInRGB888  = (0 != PcdGet32(PcdUnderflowColor))? PcdGet32(PcdUnderflowColor) : pPanelConfig->uUnderflowColor;
    sInterfaceTiming.eDataEnableSignal        = (0==pPanelConfig->sActiveTiming.uDataEnInvertSignal)?HAL_MDP_SIGNAL_POLARITY_LOW:HAL_MDP_SIGNAL_POLARITY_HIGH;
    sInterfaceTiming.eVsyncSignal             = (0==pPanelConfig->sActiveTiming.uVsyncInvertSignal) ?HAL_MDP_SIGNAL_POLARITY_LOW:HAL_MDP_SIGNAL_POLARITY_HIGH;
    sInterfaceTiming.eHsyncSignal             = (0==pPanelConfig->sActiveTiming.uHsyncInvertSignal) ?HAL_MDP_SIGNAL_POLARITY_LOW:HAL_MDP_SIGNAL_POLARITY_HIGH;

    sInterfaceConfig.ePingPongId = pPanelConfig->sDisplayControlPath[uI].ePingPongId;

    /* Handle interface specific configuration */
    switch (pPanelConfig->ePhysConnect)
    {
      case MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO:
      case MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD:
      case MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO:
      case MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD:
        {
          /* Split DSI display */
          if (MDP_INTERFACE_DUAL == pPanelConfig->uNumInterfaces)
          {
            sInterfaceTiming.uVisibleWidthInPixels >>= 1;
          }

          /* Check for DSI DSC */
          if ( TRUE == pPanelConfig->uAttrs.sDsi.bDSCEnable)
          {
            if (pPanelConfig->uAttrs.sDsi.uDSCCompressionRatio)
            {
              //If DSC is Enabled, the timing could be reduced by the factor of the compression ratio.
              sInterfaceTiming.uVisibleWidthInPixels    /= pPanelConfig->uAttrs.sDsi.uDSCCompressionRatio;
            }
            else
            {
              eStatus = MDP_STATUS_BAD_PARAM;    
            }
          }
          break;
        }
      case MDP_DISPLAY_CONNECT_FRAMEBUFFER_WRITEBACK:
        {
          /* Setup data path */
          HAL_MDP_DataPathConfigType         sDataPath;
          HAL_MDP_DataPath_SurfaceConfigType sSurfaceConfig;
          HAL_MDP_RectType                   sOutRectConfig;
          HAL_MDP_SurfaceAttrType            sOutSurfaceConfig;
          uint32                             uBitsPerPixel;
          void                              *pOutputBuffer;

          MDP_OSAL_MEMZERO(&sDataPath, sizeof(HAL_MDP_DataPathConfigType));
          MDP_OSAL_MEMZERO(&sSurfaceConfig, sizeof(HAL_MDP_DataPath_SurfaceConfigType));
          MDP_OSAL_MEMZERO(&sOutRectConfig, sizeof(HAL_MDP_RectType));
          MDP_OSAL_MEMZERO(&sOutSurfaceConfig, sizeof(HAL_MDP_SurfaceAttrType));

          sDataPath.eDataPathMode                       = HAL_MDP_DATA_PATH_LINEAR_MODE;
          sOutRectConfig.uPosX                          = sInterfaceTiming.uHLeftBorderInPixels;
          sOutRectConfig.uPosY                          = sInterfaceTiming.uVTopBorderInLines;
          sOutRectConfig.uWidthInPixels                 = sInterfaceTiming.uVisibleWidthInPixels;
          sOutRectConfig.uHeightInPixels                = sInterfaceTiming.uVisibleHeightInPixels;

          uBitsPerPixel                                 = MDP_GET_PIXELFMTMAP(pPanelConfig->eColorFormat).uBitsPerPixel;
          sOutSurfaceConfig.ePixelFormat                = sInterfaceTiming.eInterfacePixelFormat;
          sOutSurfaceConfig.uWidthInPixel               = sInterfaceTiming.uVisibleWidthInPixels;
          sOutSurfaceConfig.uHeightInPixel              = sInterfaceTiming.uVisibleHeightInPixels;
          sOutSurfaceConfig.uPlanes.sRgb.uStrideInBytes = ((sOutSurfaceConfig.uWidthInPixel * uBitsPerPixel) + 7) / 8;

          if (NULL == (pOutputBuffer = MDP_OSAL_CALLOC(sOutSurfaceConfig.uPlanes.sRgb.uStrideInBytes *
                                                    sOutSurfaceConfig.uHeightInPixel)))
          {
            // Cannot allocate a writeback buffer
            eStatus = MDP_STATUS_NO_RESOURCES;
          }
          else
          {
            sOutSurfaceConfig.uPlanes.sRgb.sDeviceAddress.iQuadPart = (int64) pOutputBuffer;
            sSurfaceConfig.psOutputRectConfig                       = &sOutRectConfig;
            sSurfaceConfig.psOutSurfaceConfig                       = &sOutSurfaceConfig;
            sDataPath.pSurfaceConfig                                = &sSurfaceConfig;

            //Cache the buffer address, so it can be freed later
            pPanelConfig->sWritebackAddress.iQuadPart               = (int64) pOutputBuffer;

            if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_DataPath_Setup(HAL_MDP_DATA_PATH_0, &sDataPath, 0x00))
            {
              eStatus = MDP_STATUS_FAILED;
            }
          }
        }
      default:
        // No other handling for other interfaces
        break;
    }

    if( 0 != uI )
    {
        sDualInterface.bEnableDualInterface   = TRUE;
        sDualInterface.eInterfaceMode         = pInterfaceMap->eInterfaceMode;
        sDualInterface.bEnablePingPongSplit   = (MDP_PANEL_FLAG_ENABLE_PINGPONG_SPLIT & pPanelConfig->eDisplayFlags) ? TRUE : FALSE;
        sDualInterface.uCmdModePixelAlignment = pPanelConfig->uAttrs.sDsi.uCmdModePixelAlignment;
        uMDPSetupFlags                       |= (pPanelConfig->uAttrs.sDsi.uDsiInterfaceMaster & 0x01) ? HAL_MDP_FLAGS_DSI_1_MASTER : 0x0;

        sInterfaceConfig.pDualInterfaceConfig = &sDualInterface;
    }

  	if (HAL_MDP_INTERFACE_MODE_VIDEO == pInterfaceMap->eInterfaceMode)
  	{
  	  // Enable Programmable fetch start (before enabling the timing engine)
  	  sInterfaceConfig.pIntfProgFetchStartConfig = &sProgFetchStartConfig;
  	}

    if (MDP_STATUS_OK == eStatus)    
    {
      /* Setup for command mode only */
      if (HAL_MDP_INTERFACE_MODE_COMMAND == pInterfaceMap->eInterfaceMode)
      {   
        MDP_OSAL_MEMZERO(&sVsyncConfig, sizeof(HAL_MDP_Interface_VsyncConfigType));
        MDP_OSAL_MEMZERO(&sTEConfig, sizeof(HAL_MDP_Interface_TEConfigType));
                
        sInterfaceConfig.pIntrVsyncConfig = &sVsyncConfig;
        sInterfaceConfig.pIntrTEConfig    = &sTEConfig;

        /* Get the vsync counter configuration */
        displayCommandModeVsyncConfig(pPanelConfig, &sVsyncConfig, uI);

        /* Get the TE configuraion */
        displayCommandModeTearCheckConfig(pPanelConfig, &sTEConfig, uI);
        
      }
      
      if (HAL_MDSS_STATUS_SUCCESS == HAL_MDP_Interface_Setup(pPanelConfig->sDisplayControlPath[uI].eInterfaceId,
                                                             &sInterfaceConfig,
                                                             uMDPSetupFlags))
      {
        // Clear and disable all interrupts on this interface
        HAL_MDP_InterruptConfigType  sInterrupts[1];

        // Setup for the blt interrupt
        MDP_OSAL_MEMZERO(&sInterrupts, sizeof(sInterrupts));
        sInterrupts[0].eModuleType            = HAL_MDP_MODULE_INTERFACE;
        sInterrupts[0].eModuleId.eInterfaceId = pPanelConfig->sDisplayControlPath[uI].eInterfaceId;
        sInterrupts[0].eInterruptSrc          = HAL_MDP_INTERRUPT_NONE;

        // Disable all unwanted interrupts
        HAL_MDP_Interrupt_Enable((HAL_MDP_InterruptConfigType*)&sInterrupts, 1, 0x0);

        // Clear all interrupt status
        sInterrupts[0].eInterruptSrc = (HAL_MDP_InterruptType)(HAL_MDP_INTERRUPT_UNDERRUN | HAL_MDP_INTERRUPT_VSYNC); /* TODO: Replace with HAL_MDP_INTERRUPT_ALL (when vailable) */
        HAL_MDP_Interrupt_Clear((HAL_MDP_InterruptConfigType*)&sInterrupts, 1, 0x0);
      }
      else
      {
        eStatus = MDP_STATUS_FAILED;          
      }
    }
  }

  return eStatus;
}   
   
/* -----------------------------------------------------------------------
**
** FUNCTION: displayCommandModeVsyncConfig
**
** DESCRIPTION:
**   This API is used to config vsync counter for smart panel
**
** INPUT:
**   pPanelAttr       - panel attributes
**   uPathIndex       - path index number for dual dsi case
**
** OUTPUT:
**   pVsyncConfig     - vsync counter configuration
**
** RETURN:
**   none
**
** ----------------------------------------------------------------------- */
static void displayCommandModeVsyncConfig(MDP_Panel_AttrType *pPanelAttr, HAL_MDP_Interface_VsyncConfigType *pVsyncConfig, uint32 uPathIndex)
{
    float                               fRefreshRate       = 0.0;
    float                               fVsyncCount        = 0.0;
    float                               fVsyncHeight       = 0.0;
    float                               fVariance          = 0.0;
    float                               fPXOClock          = 0.0;

    fPXOClock = (float)MDP_DEFAULT_CXO_CLK;

    // auto-refresh info
    pVsyncConfig->bEnableAutoRefresh      = pPanelAttr->uAttrs.sDsi.bEnableAutoRefresh;
    pVsyncConfig->uAutoRefreshFrameNumDiv = pPanelAttr->uAttrs.sDsi.uAutoRefreshFrameNumDiv;
    
    if(0 == pVsyncConfig->uAutoRefreshFrameNumDiv)
    {
        pVsyncConfig->uAutoRefreshFrameNumDiv = 1;
    }

    // Calculate requested refresh rate
    fRefreshRate = (float)(pPanelAttr->uAttrs.sDsi.uRefreshRate >> 16);

    // Get TE variance
    fVariance = (float)(pPanelAttr->uAttrs.sDsi.sTE.vSyncPercentVariance >> 16);

    // Calculate the require vsync count.
    // Calculation is equal to the number of vsync ticks it takes to complete a single scanline.
    //          MDP_VSYNC_CLK / (display height * refresh rate)
    //
    //  This clock counts at the same refresh as the panel, if TE is enable the TE synchronizes this counter with the panel.
    //
    fVsyncCount = (float)(fPXOClock/(pPanelAttr->uDisplayHeight*fRefreshRate));

    // apply variance on vsync height
    fVsyncHeight = (float)(pPanelAttr->uDisplayHeight + pPanelAttr->sActiveTiming.uVsyncBackPorchLns + pPanelAttr->sActiveTiming.uVsyncFrontPorchLns - 1);

    if (((int)fVariance > 0) && ((int)fVariance < 100))
    {
        fVsyncHeight = (float)fVsyncHeight / (1 - (fVariance / 100));
    }
    else 
    {
        /*
         * when external TE enabled and variance is not set at panel xml,
         * then set TE height to maximum to configure TE as free run. In this
         * case, read point increase from init value to maximum and can only
         * be reset back to init value by external TE pusle every 16.6 ms 
         */
       if (FALSE == pPanelAttr->uAttrs.sDsi.sTE.bTECheckSetDisable)
       {
             fVsyncHeight = (float) MDP_DEFAULT_TE_HEIGHT_MAX;
       }
    }
    pVsyncConfig->uVsyncCount        = (uint32)fVsyncCount;
    pVsyncConfig->uVsyncHeight       = (uint32)fVsyncHeight;
    pVsyncConfig->uVsyncRdPtrIrqLine = (0 == pPanelAttr->uAttrs.sDsi.sTE.vSyncRdPtrIrqLine) ? (pPanelAttr->uDisplayHeight + 1) : pPanelAttr->uAttrs.sDsi.sTE.vSyncRdPtrIrqLine;
    pVsyncConfig->uVsyncInitValue    = pPanelAttr->uAttrs.sDsi.sTE.uVsyncInitValue;

    // Configure uVsyncWrPtrIrqLine as 1 for all command mode displays
    if ((MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD   == pPanelAttr->ePhysConnect) ||
        (MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD == pPanelAttr->ePhysConnect))
    {
      pVsyncConfig->uVsyncWrPtrIrqLine = 1;
    }

    // for dual-dsi case, we will need to disable auto-refresh for slave controller
    // because the slave is always configured to be triggered by the master
    if (!(MDP_PANEL_FLAG_ENABLE_PINGPONG_SPLIT & pPanelAttr->eDisplayFlags) &&
       ((MDP_DUALPIPE_NUM_MIXERS - 1) == uPathIndex))
    {
      pVsyncConfig->bEnableAutoRefresh = FALSE;
    }
}

/* -----------------------------------------------------------------------
**
** FUNCTION: displayCommandModeTearCheckConfig
**
** DESCRIPTION:
**   This API is used to get/calculate tear check settings for smart panel
**
** INPUT:
**   pPanelAttr       - panel attributes
**   uPathIndex       - path index number for dual dsi case
**
** OUTPUT:
**   pTEConfig        - TE configuration
**
** RETURN:
**   none
**
** ----------------------------------------------------------------------- */
static void displayCommandModeTearCheckConfig(MDP_Panel_AttrType *pPanelAttr, HAL_MDP_Interface_TEConfigType *pTEConfig, uint32 uPathIndex)
{
    uint32                              uDivisor       = 1;
    uint32                              uStartPosition = 0;


    // adjust TE continue threshold
    pTEConfig->uContinueThreshold     = (0 == pPanelAttr->uAttrs.sDsi.sTE.vSyncContinueLines)? 
                                       (MDP_DEFAULT_VSYNC_CONTINUE_LINES) : 
                                       (pPanelAttr->uAttrs.sDsi.sTE.vSyncContinueLines);
    if(pTEConfig->uContinueThreshold > 2*pPanelAttr->uDisplayHeight)
    {
        pTEConfig->uContinueThreshold = 0;
    }

    // adjust TE start threshold
    uDivisor = pPanelAttr->uAttrs.sDsi.sTE.vSyncStartLineDivisor;
    if(uDivisor > 0)
    {
        pTEConfig->uStartThreshold = pPanelAttr->uDisplayHeight/uDivisor;
    }

    if((0 == uDivisor) ||
       (0 == pTEConfig->uStartThreshold))
    {
        /*
         * StartThreshold decide kickoff window size.
         * larger window size have higher possibility of tearing at panel
         * if kickkof triggered close to the end of window.
         * set kickoff window size to 4 vertical lines as default
         */
        pTEConfig->uStartThreshold = MDP_DEFAULT_VSYNC_START_LINES;
    }

    /* adjust TE start position */
    uStartPosition         = (0 == pPanelAttr->uAttrs.sDsi.sTE.vSyncStartPos)?
                             (pPanelAttr->sActiveTiming.uVsyncBackPorchLns+1) : 
                             (pPanelAttr->uAttrs.sDsi.sTE.vSyncStartPos);

    /* dedicated pin used */
    pTEConfig->bDedicatedTEPin        = pPanelAttr->uAttrs.sDsi.sTE.bDedicatedTEPin;

    /* external vsync source select */
    pTEConfig->eVsyncSelect           = (HAL_MDP_Interface_VsyncSelectType)pPanelAttr->uAttrs.sDsi.sTE.vSyncSelect;


    /* For dual DSI case, some panel requires the master controller transfers data on the data lane some time ahead of 
        the slave controller. Current solution is to use the start window in TE block to achieve this, which requires 
        driver to set different start position for the controllers. 

                                (start_pos)      (start_pos+TH)
        master controller       |               |
                                +---------------+

                                                    (start_pos)      (start_pos+TH)
        slave controller                              |               |
                                                      +---------------+

                                                |     |
                                             -->|  x  |<--   (x is the skew lines, which is pPanelAttr->uAttrs.sDsi.iSlaveControllerSkewLines)
                                                |     |
           
        Notes:
            -  This adjustment should be ignored for single DSI case.
            -  For dual DSI case:
                (i)   (iSlaveControllerSkewLines > 0) means master controller transfers data ahead of slave controller
                (ii)  (iSlaveControllerSkewLines == 0) means two controllers can transfer data at same time
                (iii) (iSlaveControllerSkewLines < 0) means master controller transfers data after slave controller
    */

    pTEConfig->uStartPosition = uStartPosition;

    if ((MDP_DUALPIPE_NUM_MIXERS == pPanelAttr->uNumMixers) &&
        (MDP_INTERFACE_DUAL == pPanelAttr->uNumInterfaces))
    {
        /* if this dual pipe case */
        if (pPanelAttr->uAttrs.sDsi.iSlaveControllerSkewLines > 0)
        {
            /* Since iSlaveControllerSkewLines is positive, we need to adjust the start position for slave controller. */
            if ((MDP_DUALPIPE_NUM_MIXERS - 1) == uPathIndex)
            {
                pTEConfig->uStartPosition += (pTEConfig->uStartThreshold + pPanelAttr->uAttrs.sDsi.iSlaveControllerSkewLines);
            }
        }
        else if (pPanelAttr->uAttrs.sDsi.iSlaveControllerSkewLines == 0)
        {
            /* do nothing here. TE window is same for both controllers since there's no skew setting */
        }
        else
        {
            /* Since iSlaveControllerSkewLines is negative, we need to adjust the start position for master controller.*/
            if (0 == uPathIndex)
            {
                pTEConfig->uStartPosition += (pTEConfig->uStartThreshold + (-1)*pPanelAttr->uAttrs.sDsi.iSlaveControllerSkewLines);
            }
        }
    }
}

/****************************************************************************
*
** FUNCTION: SetupPingPong()
*/
/*!
* \brief
*   Local helper function to setup the pingpong block
*
* \param  [in] pPanelConfig          - Panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
static MDP_Status SetupPingPong(MDP_Panel_AttrType *pPanelConfig)
{
    MDP_Status                          eStatus            = MDP_STATUS_OK;
    HAL_MDP_PingPongConfigType          sPingpongConfig;
    HAL_MDP_PingPong_DSCConfigType      sDSCConfig;
    uint32                              uI;

    for(uI = 0; uI < pPanelConfig->uNumMixers; uI++ )
    {
      if (HAL_MDP_INTERFACE_NONE == pPanelConfig->sDisplayControlPath[uI].eInterfaceId)
      {
        continue;
      }

      /* Setup Frame Buffer Compression */
      if (MDP_STATUS_OK == eStatus)
      {
        /* Handle interface specific configurations */
        switch (pPanelConfig->ePhysConnect)
        {
          case MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO:
          case MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD:
          case MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO:
          case MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD:
          {
           if (TRUE == pPanelConfig->uAttrs.sDsi.bDSCEnable)
           {
               MDP_OSAL_MEMZERO(&sDSCConfig, sizeof(HAL_MDP_PingPong_DSCConfigType));
               MDP_OSAL_MEMZERO(&sPingpongConfig, sizeof(HAL_MDP_PingPongConfigType));
            
               sPingpongConfig.pDSCConfig      = &sDSCConfig;

              if (pPanelConfig->uAttrs.sDsi.uDSCProfileID< MDP_DSC_PROFILEID_MAX)
              {
                  sPingpongConfig.pDSCConfig->bEnableDSCEnc = TRUE;
                  sPingpongConfig.pDSCConfig->bEndianess    = TRUE;
              }
              if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_PingPong_Setup(pPanelConfig->sDisplayControlPath[uI].ePingPongId, &sPingpongConfig, 0x00))
              {
                  eStatus = MDP_STATUS_FAILED;        
              }
              // Hardcoding to setup both the pingpongs
              if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_PingPong_Setup(HAL_MDP_PINGPONG_1, &sPingpongConfig, 0x00))
              {
                  eStatus = MDP_STATUS_FAILED;     
              }

           }
          }
          break;
        default:
          // No other handling for other interfaces
          break;
        }
      }
    }
    return eStatus;
}


/****************************************************************************
*
** FUNCTION: SetupControlPath()
*/
/*!
* \brief
*   Local helper function to setup the MDP control path
*
* \param  [in] pPanelConfig          - Panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
static MDP_Status SetupControlPath(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status                                      eStatus = MDP_STATUS_OK;
  MDP_DisplayInterfaceMapType                    *pInterfaceMap;
  HAL_MDP_ControlPathConfigType                   sControlPathConfig;
  HAL_MDP_ControlPath_MixerConfigType             sMixerConfig;
  HAL_MDP_ControlPath_InterfaceConfigType         sInterfaceCfg;
  HAL_MDP_LayerMixerConfigType                    sLayerMixerCfg;
  HAL_MDP_ControlPath_Mixer_ZOrderConfigType      sZorderConfig;
  HAL_MDP_ControlPath_Mixer_BorderColorConfigType sBorderColorConfig;
  HAL_MDP_Mixer_OutputSizeConfigType              sMixerOutputConfig;
  HAL_MDP_Mixer_BorderColorConfigType             sBorderOutputColorConfig;
  HAL_MDP_Mixer_BlendStageConfigType              sBlendStageConfig;
  HAL_MDP_ControlPath_ModulesType                 sActiveModulesConfig;
  uint32                                          uInterfaceId;
  uint32                                          uI;

  // Setup Control Path
  for (uI = 0; uI < pPanelConfig->uNumMixers; uI++)
  {
    // Ensure same interface will not be configured twice.
    if ((uI                                                    > MDP_MASTER_PATH_INDEX)  &&
        (pPanelConfig->sDisplayControlPath[uI-1].eInterfaceId == pPanelConfig->sDisplayControlPath[uI].eInterfaceId))
    {
      continue;
    }
    else if (HAL_MDP_INTERFACE_NONE != pPanelConfig->sDisplayControlPath[uI].eInterfaceId)
    {
      // Ensure interface is disable before programming
      if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_Interface_Enable(pPanelConfig->sDisplayControlPath[uI].eInterfaceId, FALSE, 0x0))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: Failed to disable the interface failed!\n");
      }
    }
  }

  for (uInterfaceId = 0; uInterfaceId < pPanelConfig->uNumMixers; uInterfaceId++)
  {
    uint32 uBlendStage = 0;
    uint32 uFlags      = 0;
    
    pInterfaceMap = (MDP_DisplayInterfaceMapType*)MDP_GET_INTFMODEHMAP(pPanelConfig->ePhysConnect,
                                                                       uInterfaceId, 
                                                                       pPanelConfig->uAttrs.sDsi.uControllerSinglePrimary);
    
    // Setup border color
    MDP_OSAL_MEMZERO(&sBorderColorConfig, sizeof(HAL_MDP_ControlPath_Mixer_BorderColorConfigType));
    sBorderColorConfig.bBorderColorEnable = TRUE;
    
    // Setup the ZOrder
    MDP_OSAL_MEMZERO(&sZorderConfig, sizeof(HAL_MDP_ControlPath_Mixer_ZOrderConfigType));
    sZorderConfig.eSourcePipe[HAL_MDP_BLEND_ORDER_ID][MDP_MASTER_PATH_INDEX] = pPanelConfig->sDisplayControlPath[uInterfaceId].eSourcePipeId;
    
    // Setup the mixer configuration
    MDP_OSAL_MEMZERO(&sMixerConfig, sizeof(HAL_MDP_ControlPath_MixerConfigType));
    sMixerConfig.eMixerId                 = pPanelConfig->sDisplayControlPath[uInterfaceId].eLayerMixerId;
    sMixerConfig.psMixerZOrderConfig      = &sZorderConfig;
    sMixerConfig.psMixerBorderColorConfig = &sBorderColorConfig;
    
    // Setup the interface configuration
    MDP_OSAL_MEMZERO(&sInterfaceCfg, sizeof(HAL_MDP_ControlPath_InterfaceConfigType));
    if (HAL_MDP_INTERFACE_NONE != pPanelConfig->sDisplayControlPath[uInterfaceId].eInterfaceId)
    {
      sInterfaceCfg.eInterfaceId       = pPanelConfig->sDisplayControlPath[uInterfaceId].eInterfaceId;
      sInterfaceCfg.eInterfaceModeType = pInterfaceMap->eInterfaceMode;
    }

    // Setup 3D Mux ( for Dual/Quad Pipe Mode )
    if ((MDP_DUALPIPE_NUM_MIXERS == pPanelConfig->uNumMixers) ||
        (MDP_QUADPIPE_NUM_MIXERS == pPanelConfig->uNumMixers))
    {
       // Setup 3D Mux
       sInterfaceCfg.b3DMuxEnable = MDP_GET_DUALPIPEMUX(pPanelConfig->ePhysConnect);

       if ((MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO   == pPanelConfig->ePhysConnect) ||
           (MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD     == pPanelConfig->ePhysConnect) ||
           (MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO == pPanelConfig->ePhysConnect) ||
           (MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD   == pPanelConfig->ePhysConnect))
       {
         sInterfaceCfg.b3DMuxEnable = pPanelConfig->uAttrs.sDsi.uDSC3DMux;
       }

       if (TRUE == sInterfaceCfg.b3DMuxEnable)
       {
         sInterfaceCfg.eMuxPackerModeConfig = HAL_MDP_3DMUX_PACK_MODE_HORZ_ROW_INTERLEAVE;
       }

       // Setup LM to use source split (LM share same coordinate system)
       if ((MDP_SLAVE_PATH_INDEX   == uInterfaceId) ||
           (MDP_SLAVE_PATH3_INDEX  == uInterfaceId))
       {
          uFlags |= HAL_MDP_FLAGS_SPLIT_DISPLAY_RIGHT_MIXER;
       }
    }
    
    // Setup mixer output configuration
    MDP_OSAL_MEMZERO(&sMixerOutputConfig, sizeof(HAL_MDP_Mixer_OutputSizeConfigType));

    // In dual/quad pipe scenario each mixer will output width equal to pPanelConfig->uDisplayWidth >> (pPanelConfig->uNumMixers/2)
    if ((MDP_DUALPIPE_NUM_MIXERS == pPanelConfig->uNumMixers) ||
        (MDP_QUADPIPE_NUM_MIXERS == pPanelConfig->uNumMixers))
    {
      sMixerOutputConfig.uOutputWidthInPixels = pPanelConfig->uDisplayWidth >> (pPanelConfig->uNumMixers/2);
    }
    else
    {
      sMixerOutputConfig.uOutputWidthInPixels = pPanelConfig->uDisplayWidth;
    }
    sMixerOutputConfig.uOutputHeightInPixels = pPanelConfig->uDisplayHeight;
    
    // Setup the layer mixer config (default to black)
    MDP_OSAL_MEMZERO(&sBorderOutputColorConfig, sizeof(HAL_MDP_Mixer_BorderColorConfigType));
    sBorderOutputColorConfig.uColorValue = 0x0;
    
    // Setup opaque blending (no alpha blending
    MDP_OSAL_MEMZERO(&sBlendStageConfig, sizeof(HAL_MDP_Mixer_BlendStageConfigType));
    // Background is using (1-A) * FG_Alpha
    sBlendStageConfig.sBackGroundAlphaBlend.bConfigure    = TRUE;
    sBlendStageConfig.sBackGroundAlphaBlend.eAlphaSel     = HAL_MDP_ALPHA_SRC_FG_CONST;
    sBlendStageConfig.sBackGroundAlphaBlend.bInverseAlpha = TRUE;
    // Foreground is using 100% * FG_Alpha
    sBlendStageConfig.sForeGroundAlphaBlend.bConfigure    = TRUE;
    sBlendStageConfig.sForeGroundAlphaBlend.eAlphaSel     = HAL_MDP_ALPHA_SRC_FG_CONST;
    sBlendStageConfig.sForeGroundAlphaBlend.uConstant     = 0xFF;
    
    // Setup the layer mixer config
    MDP_OSAL_MEMZERO(&sLayerMixerCfg, sizeof(HAL_MDP_LayerMixerConfigType));
    sLayerMixerCfg.eLayerMixerId         = pPanelConfig->sDisplayControlPath[uInterfaceId].eLayerMixerId;
    sLayerMixerCfg.psOutputSizeConfig    = &sMixerOutputConfig;
    sLayerMixerCfg.psBorderColorConfig   = &sBorderOutputColorConfig;
    for (uBlendStage=HAL_MDP_BLEND_STAGE_0;uBlendStage<HAL_MDP_BLEND_STAGE_6;uBlendStage++)
    {
      sLayerMixerCfg.psBlendStageConfig[uBlendStage] = &sBlendStageConfig;
    }

    // Setup active modules
    MDP_OSAL_MEMZERO(&sActiveModulesConfig, sizeof(HAL_MDP_ControlPath_ModulesType));
    if (pPanelConfig->uAttrs.sDsi.bDSCEnable)
    {
      sActiveModulesConfig.uDSCId = gMDP_ModulesMap[pPanelConfig->sDisplayControlPath[uInterfaceId].ePingPongId].uDSCId;
    }
    if (sInterfaceCfg.b3DMuxEnable)
    {
      sActiveModulesConfig.u3DMuxId= gMDP_ModulesMap[pPanelConfig->sDisplayControlPath[uInterfaceId].ePingPongId].u3DMuxId;
    }
    sActiveModulesConfig.uPingPongId = pPanelConfig->sDisplayControlPath[uInterfaceId].ePingPongId;

    // Setup the control path
    MDP_OSAL_MEMZERO(&sControlPathConfig, sizeof(HAL_MDP_ControlPathConfigType));
    sControlPathConfig.psMixerConfig      = &sMixerConfig;
    sControlPathConfig.psLayerMixerConfig = &sLayerMixerCfg;    
    sControlPathConfig.psModulesConfig    = &sActiveModulesConfig;
    if (HAL_MDP_INTERFACE_NONE != pPanelConfig->sDisplayControlPath[uInterfaceId].eInterfaceId)
    {
      sControlPathConfig.psInterfaceConfig  = &sInterfaceCfg;
    }

    if ((MDP_SLAVE_PATH_INDEX   == uInterfaceId) ||
        (MDP_SLAVE_PATH2_INDEX  == uInterfaceId) ||
        (MDP_SLAVE_PATH3_INDEX  == uInterfaceId))
    {
      uFlags |= HAL_MDP_FLAGS_SLAVE_CONTROL_PATH;
    }

    // Program the control path
    if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_ControlPath_Setup(pPanelConfig->sDisplayControlPath[uInterfaceId].eControlPathId, 
                                                             &sControlPathConfig,
                                                             uFlags))
    {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_ControlPath_Setup failed!\n");
        eStatus = MDP_STATUS_FAILED;
    }
  }

  return eStatus;
}  



/****************************************************************************
*
** FUNCTION: ScaleHelperCleanFrac()
*/
/*!
* \brief
*   Helper to round fractional portion of a 32bit unsigned value (from libScale)
*
* \param [in] uInput          - 32bit signed value
*
* \retval int32 
*
****************************************************************************/
static int32 ScaleHelperCleanFrac(int32  iInput)
{
  int32  uOutput = 0;
  uint64 uTemp = 0;

  if (iInput<0)
  {
    uTemp = iInput + (1 << 31);
    uTemp = uTemp & 0xFFFF8000;
    uTemp = uTemp - (1 << 31);
  }
  else
  {
   uTemp = (uint64)iInput &  (uint64)(0xFFFF8000);
  }

  uOutput = (uint32)(uTemp);

  return uOutput;
}


/****************************************************************************
*
** FUNCTION: ScaleHelperCalcNumExtendedPELS()
*/
/*!
* \brief
*   Helper to calculate the pixel extension length
*
* \param [in] uExtraPixels    - Number of extra pixels
*
* \retval int32
*
****************************************************************************/
static int32 ScaleHelperCalcNumExtendedPELS(int32 uExtraPixels)
{
  int32  uResult    = 0; 
  uint32 uExtraTemp = 0;

  if (uExtraPixels == 0)
  {
    uResult = 0;
  }
  else if (uExtraPixels>0)
  {
    //drop pixels
    uExtraTemp = (uint32)(uExtraPixels);
    uResult    = (uExtraTemp >> MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_BITS);
    uResult    = -(uResult);
  }
  else
  {
    //padding pixels
    uExtraTemp = (uint32)(-uExtraPixels);
    uResult    = uExtraTemp >> MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_BITS;
    if ((uResult << MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_BITS) != (int32)uExtraTemp)
    {
      uResult++;
    }
  }

  return uResult;
}

/****************************************************************************
*
** FUNCTION: ScaleHelperCalcCirLUT()
*/
/*!
* \brief
*   Helper to calculate the circular LUT index
*
* \param [in] uPhaseStep          - Phase step (max of X and Y)
*
* \retval int32 
*
****************************************************************************/
static uint32 ScaleHelperCalcCirLUT(uint32 uPhaseStep)
{
    uint32 uResult = MDP_QSEED3_LUT_INDEX_0;
    uint32 uIndex  = 0;
    
    if (uPhaseStep < (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 2))) // 1.25
      uIndex = 0; // MDP_QSEED3_LUT_INDEX_0;
    else if (uPhaseStep <  (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 1))) // 1.5
      uIndex = 1; //MDP_QSEED3_LUT_INDEX_1;
    else if (uPhaseStep <  (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 1) + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 2))) // 1.75
      uIndex = 2; //MDP_QSEED3_LUT_INDEX_2;
    else if (uPhaseStep <  (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 1)) // 2.0
      uIndex = 3; //MDP_QSEED3_LUT_INDEX_3;
    else if (uPhaseStep <  ((MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 1) + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 1))) // 2.5
      uIndex = 4; // MDP_QSEED3_LUT_INDEX_4;
    else if (uPhaseStep <  ((MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 1) + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE))) // 3.0
      uIndex = 5; //MDP_QSEED3_LUT_INDEX_5;
    else if (uPhaseStep <  ((MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 1) + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE)+(MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 1))) // 3.5
      uIndex = 6; // MDP_QSEED3_LUT_INDEX_6;
    else if (uPhaseStep <  (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 2)) // 4.0
      uIndex = 7; // MDP_QSEED3_LUT_INDEX_7;
    else
      uIndex = 8; // MDP_QSEED3_LUT_INDEX_8;

    return (uResult+uIndex);
}

/****************************************************************************
*
** FUNCTION: ScaleHelperCalcSepLUT()
*/
/*!
* \brief
*   Helper to calculate the separable LUT index
*
* \param [in] uPhaseStep          - Phase step (max of X and Y)
*
* \retval int32 
*
****************************************************************************/
static uint32 ScaleHelperCalcSepLUT(uint32 uPhaseStep)
{
  uint32 uResult = MDP_QSEED3_LUT_INDEX_0;
  uint32 uIndex  = 0;

  if (uPhaseStep <  MDP_QSEED3_FIR_PIXEL_UNIT_SCALE) // 1.0
    uIndex = 0; // MDP_QSEED3_LUT_INDEX_0;
  else if (uPhaseStep <  (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 2))) // 1.25
    uIndex = 1; // MDP_QSEED3_LUT_INDEX_1;
  else if (uPhaseStep <  (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 1))) // 1.5
    uIndex = 2; // MDP_QSEED3_LUT_INDEX_2;
  else if (uPhaseStep <  (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 1) + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 2))) // 1.75
    uIndex = 3; // MDP_QSEED3_LUT_INDEX_3;
  else if (uPhaseStep <  (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 1)) // 2.0
    uIndex = 4; // MDP_QSEED3_LUT_INDEX_4;
  else if (uPhaseStep <  ((MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 1) + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 1))) // 2.5
    uIndex = 5; // MDP_QSEED3_LUT_INDEX_5;
  else if (uPhaseStep <  ((MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 1) + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE))) // 3.0
    uIndex = 6; // MDP_QSEED3_LUT_INDEX_6;
  else if (uPhaseStep <  ((MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 1) + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE)+(MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 1))) // 3.5
    uIndex = 7; // MDP_QSEED3_LUT_INDEX_7;
  else if (uPhaseStep <  (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE << 2)) // 4.0
    uIndex = 8; // MDP_QSEED3_LUT_INDEX_8;
  else
    uIndex = 9; // MDP_QSEED3_LUT_INDEX_9;

  return (uResult+uIndex);  
}


/****************************************************************************
*
** FUNCTION: HelperCalcLUTIndex()
*/
/*!
* \brief
*   Helper to calculate the LUT index for directional, circular and separable
*
* \param [in] uPhaseStepX          - Phase step width
*        [in] uPhaseStepY          - Phase step height
*        [out] pDirLut             - Directional LUT index
*        [out] pCirLut             - Circular  LUT index
*        [out] pSepLut             - Separable LUT index
*        [out] pScaleMethod        - Scale method
*        [out] pBlendFilter        - Blend filter
*
* \retval none
*
****************************************************************************/
static void ScaleHelperCalcLUTIndex(uint32 uPhaseStepX, uint32 uPhaseStepY, uint32 *pDirLut, uint32 *pCirLut, uint32 *pSepLut, uint32 *pScaleMethod, uint32 *pBlendFilter)
{
  uint32 uMaxPhase = (uPhaseStepX>uPhaseStepY) ? uPhaseStepX : uPhaseStepY;
           
  if ((uPhaseStepX == MDP_QSEED3_FIR_PIXEL_UNIT_SCALE) &&
      (uPhaseStepY == MDP_QSEED3_FIR_PIXEL_UNIT_SCALE))
  {
    *pDirLut      = 0;
    *pCirLut      = 0;
    *pSepLut      = 0;
    *pScaleMethod = MDP_QSEED3_SCALE_METHOD_EDGE;
  }
  else if ((uPhaseStepX <= MDP_QSEED3_FIR_PIXEL_UNIT_SCALE) &&  
           (uPhaseStepY <= MDP_QSEED3_FIR_PIXEL_UNIT_SCALE))
  {
    // unity or upscale
    *pDirLut      = MDP_QSEED3_LUT_INDEX_0;
    *pCirLut      = 0;
    *pSepLut      = MDP_QSEED3_LUT_INDEX_0;    
    *pScaleMethod = MDP_QSEED3_SCALE_METHOD_EDGE;
  }
  else if ((uPhaseStepX <= (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 2))) &&
           (uPhaseStepY <= (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE + (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE >> 2))))
  {
    // Turn off it for down scale case
    *pDirLut      = MDP_QSEED3_LUT_INDEX_0;
    *pCirLut      = ScaleHelperCalcCirLUT(uMaxPhase);
    *pSepLut      = 0;
    *pScaleMethod = MDP_QSEED3_SCALE_METHOD_CIRCULAR; 
  }
  else
  {
    *pDirLut      = MDP_QSEED3_LUT_INDEX_0;
    *pCirLut      = MDP_QSEED3_LUT_INDEX_0;
    *pSepLut      = ScaleHelperCalcSepLUT(uMaxPhase);  
    *pScaleMethod = MDP_QSEED3_SCALE_METHOD_SEPARABLE; 
  }
}


/****************************************************************************
*
** FUNCTION: ScaleHelperCalcPreload
*/
/*!
* \brief
*   Helper to calculate the preload values
*
* \param [in] uPhaseStepX          - Phase step width
*        [in] uPhaseStepY          - Phase step height
*        [out] pPreloadX           - Preload value (width)
*        [out] pPreloadY           - Preload value (height)
*
* \retval void
*
****************************************************************************/
static void ScaleHelperCalcPreload(uint32 uPhaseStepX, uint32 uPhaseStepY, int16 *pPreloadX, int16 *pPreloadY)
{
  uint32 uNum_extended_pels_h = 0;
  uint32 uNum_extended_pels_v = 0;

  if ((uPhaseStepX == MDP_QSEED3_FIR_PIXEL_UNIT_SCALE) &&
      (uPhaseStepY == MDP_QSEED3_FIR_PIXEL_UNIT_SCALE))
  {
    // Unity, no calculations needed
  }
  else
  {
    int32 iPhaseX = ((int32)uPhaseStepX - (1 << (int32)MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_BITS)) >> 1;
    int32 iPhaseY = ((int32)uPhaseStepY - (1 << (int32)MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_BITS)) >> 1;
    int64 left    = (int64) ScaleHelperCleanFrac((iPhaseX + (int32)MDP_QSEED3_FIR_PHASE_RESIDUAL)) - (MDP_QSEED3_TAPS_X * MDP_QSEED3_FIR_PIXEL_UNIT_SCALE);
    int64 top     = (int64) ScaleHelperCleanFrac((iPhaseY + (int32)MDP_QSEED3_FIR_PHASE_RESIDUAL)) - (MDP_QSEED3_TAPS_Y * MDP_QSEED3_FIR_PIXEL_UNIT_SCALE);

    uNum_extended_pels_h = ScaleHelperCalcNumExtendedPELS((int32)left);
    uNum_extended_pels_v = ScaleHelperCalcNumExtendedPELS((int32)top);
  }
    
  *pPreloadX = (int16)(MDP_QSEED3_FILTER_TAPS + 2 - uNum_extended_pels_h);
  *pPreloadY = (int16)(MDP_QSEED3_FILTER_TAPS - uNum_extended_pels_v);
}

/****************************************************************************
*
** FUNCTION: ScaleHelperConfigure()
*/
/*!
* \brief
*   Simplified libScale operation for QSEED and Pixel Extension
*
* \param [out] pPixelExtConfig          - Pointer to the pixel extension structure
*        [in]  pCropRectInfo            - Surface cropping information
*        [in]  uNumMixers               - Number of mixers 
*        [in]  uFlags                   - Reserved flags
*
* \retval MDP_Status
*
****************************************************************************/
static MDP_Status ScaleHelperConfigure(HAL_MDP_PixelExtLayerConfigType *pPixelExtConfig, HAL_MDP_CropRectangleType *pCropRectInfo, uint32 uNumMixers, uint32 uFlags)
{
  MDP_Status eStatus = MDP_STATUS_OK;

  if ((NULL == pPixelExtConfig) ||
      (NULL == pCropRectInfo))
  {
    // Cannot handle NULL inputs
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else if ((0 == pCropRectInfo->sSrcRectConfig.uWidthInPixels)  ||
           (0 == pCropRectInfo->sSrcRectConfig.uHeightInPixels) ||
           (0 == pCropRectInfo->sDstRectConfig.uWidthInPixels)  ||
           (0 == pCropRectInfo->sDstRectConfig.uHeightInPixels))
  {
    // Cannot handle invalid cropping rectangles
    eStatus = MDP_STATUS_BAD_PARAM;    
  }
  else
  {
    uint32                    uI;
    HAL_MDP_ScalarConfigType *pScalarConfig = &pPixelExtConfig->sScalarConfig;   
    uint32                    uPhaseStepX;
    uint32                    uPhaseStepY;        
    int32                     uInitPhaseX;
    int32                     uInitPhaseY;
    bool32                    bEnableScale;

    //
    // Calculate phase step
    //
    //uint32 phase_step_h = (input.roi_in_width*FIR_PIXEL_UNIT_SCALE) / input.roi_out_width_left;
    //uint32 phase_step_v = (input.roi_in_height*FIR_PIXEL_UNIT_SCALE) / input.roi_out_height;
    uPhaseStepX  = ((uint64)pCropRectInfo->sSrcRectConfig.uWidthInPixels * MDP_QSEED3_FIR_PIXEL_UNIT_SCALE)  / pCropRectInfo->sDstRectConfig.uWidthInPixels;
    uPhaseStepY  = ((uint64)pCropRectInfo->sSrcRectConfig.uHeightInPixels * MDP_QSEED3_FIR_PIXEL_UNIT_SCALE) / pCropRectInfo->sDstRectConfig.uHeightInPixels;
    bEnableScale = ((MDP_QSEED3_FIR_PIXEL_UNIT_SCALE == uPhaseStepX) && (MDP_QSEED3_FIR_PIXEL_UNIT_SCALE == uPhaseStepY))?FALSE:TRUE;
   
    //
    // Calculate initial phase 
    //        
    uInitPhaseX = (int32)((uPhaseStepX - (1 << MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_BITS)) >> 1) + MDP_QSEED3_FIR_PHASE_RESIDUAL;
    uInitPhaseY = (int32)((uPhaseStepY - (1 << MDP_QSEED3_FIR_PIXEL_UNIT_SCALE_BITS)) >> 1) + MDP_QSEED3_FIR_PHASE_RESIDUAL;

    // Calculate X Initial Phase
    while (uInitPhaseX < 0)
    {
      uInitPhaseX += MDP_QSEED3_FIR_PIXEL_UNIT_SCALE;
    }

    while (uInitPhaseX >= MDP_QSEED3_FIR_PIXEL_UNIT_SCALE)
    {
      uInitPhaseX -= MDP_QSEED3_FIR_PIXEL_UNIT_SCALE;
    }

    // Calculate Y Initial Phase
    while (uInitPhaseY < 0)
    {
      uInitPhaseY += MDP_QSEED3_FIR_PIXEL_UNIT_SCALE;
    }

    while (uInitPhaseY >= MDP_QSEED3_FIR_PIXEL_UNIT_SCALE)
    {
      uInitPhaseY -= MDP_QSEED3_FIR_PIXEL_UNIT_SCALE;
    }

    
    //
    // Setup Scaler (QSEED3) config
    //  - Simplified calculation, supporting RGB formats only.
    // 
    pScalarConfig->bScalarEnable                            = bEnableScale;
    pScalarConfig->sDetailEnhancer.bDetailEnhancerEnable    = FALSE;
    pScalarConfig->bDirectionDetectionEnable                = FALSE;
    pScalarConfig->bAlpha                                   = TRUE;  // Support for only 32bpp ARGB
    pScalarConfig->bYuvColorSpace                           = FALSE; // Support for only 32bpp ARGB
    pScalarConfig->bBitWidth                                = TRUE;

    pScalarConfig->uC03HorizontalInitPhase                  = (uint32)uInitPhaseX;
    pScalarConfig->uC03VerticalInitPhase                    = (uint32)uInitPhaseY;
    pScalarConfig->uC12HorizontalInitPhase                  = (uint32)uInitPhaseX;
    pScalarConfig->uC12VerticalInitPhase                    = (uint32)uInitPhaseY;

    pScalarConfig->uC03HorizontalPhaseStep                  = uPhaseStepX;
    pScalarConfig->uC03VerticalPhaseStep                    = uPhaseStepY;
    pScalarConfig->uC12HorizontalPhaseStep                  = uPhaseStepX; // For RGB formats program the same for UV components
    pScalarConfig->uC12VerticalPhaseStep                    = uPhaseStepY; // For RGB formats program the same for UV components

    ScaleHelperCalcPreload(uPhaseStepX, uPhaseStepY, &pScalarConfig->iC03HorizontalPreload,&pScalarConfig->iC03VerticalPreload);
    pScalarConfig->iC12HorizontalPreload                    = pScalarConfig->iC03HorizontalPreload;   // UV is same as Y for RGB formats
    pScalarConfig->iC12VerticalPreload                      = pScalarConfig->iC03VerticalPreload;     // UV is same as Y for RGB formats

    pScalarConfig->uC03SrcWidth                             = pCropRectInfo->sSrcRectConfig.uWidthInPixels;
    pScalarConfig->uC03SrcHeight                            = pCropRectInfo->sSrcRectConfig.uHeightInPixels;
    pScalarConfig->uC12SrcWidth                             = pCropRectInfo->sSrcRectConfig.uWidthInPixels;
    pScalarConfig->uC12SrcHeight                            = pCropRectInfo->sSrcRectConfig.uHeightInPixels;
    
    pScalarConfig->uDstWidth                                = pCropRectInfo->sDstRectConfig.uWidthInPixels;
    pScalarConfig->uDstHeight                               = pCropRectInfo->sDstRectConfig.uHeightInPixels;
    
    pScalarConfig->bLutSwapEnable                           = FALSE;
    

    ScaleHelperCalcLUTIndex(uPhaseStepX, uPhaseStepY, &pScalarConfig->e2dLutIndex, &pScalarConfig->eYCircLutIndex, &pScalarConfig->eYSepLutIndex,
                            &pScalarConfig->uC0Filter, &pScalarConfig->uBlendFilter);
    pScalarConfig->eUVCircLutIndex                          = 0; // Not used for RGB formats
    pScalarConfig->eUVSepLutIndex                           = 0; // Not used for RGB formats

    pScalarConfig->b2dWriteEnable                           = (pScalarConfig->e2dLutIndex     >= HAL_MDP_QSEED_LUT_INDEX_0)?TRUE:FALSE;
    pScalarConfig->bYCircWriteEnable                        = (pScalarConfig->eYCircLutIndex  >= HAL_MDP_QSEED_LUT_INDEX_0)?TRUE:FALSE;
    pScalarConfig->bYSepWriteEnable                         = (pScalarConfig->eYSepLutIndex   >= HAL_MDP_QSEED_LUT_INDEX_0)?TRUE:FALSE;
    pScalarConfig->bUVCircWriteEnable                       = (pScalarConfig->eUVCircLutIndex >= HAL_MDP_QSEED_LUT_INDEX_0)?TRUE:FALSE;
    pScalarConfig->bUVSepWriteEnable                        = (pScalarConfig->eUVSepLutIndex  >= HAL_MDP_QSEED_LUT_INDEX_0)?TRUE:FALSE;

    
    pScalarConfig->uC12Filter                               = pScalarConfig->uC0Filter;        // UV is the same as Y
    pScalarConfig->uC3Filter                                = MDP_QSEED3_ALPHA_MODE_BILINEAR;  // Fixed alpha filter
    
    pScalarConfig->bDualScalarMode                          = (uNumMixers>1)?TRUE:FALSE;
    
    pScalarConfig->sRectInfo.sSrcRectConfig.uPosX           = pCropRectInfo->sSrcRectConfig.uPosX;
    pScalarConfig->sRectInfo.sSrcRectConfig.uPosY           = pCropRectInfo->sSrcRectConfig.uPosY;
    pScalarConfig->sRectInfo.sSrcRectConfig.uWidthInPixels  = pCropRectInfo->sSrcRectConfig.uWidthInPixels;
    pScalarConfig->sRectInfo.sSrcRectConfig.uHeightInPixels = pCropRectInfo->sSrcRectConfig.uHeightInPixels;
    
    pScalarConfig->sRectInfo.sDstRectConfig.uPosX           = pCropRectInfo->sDstRectConfig.uPosX;
    pScalarConfig->sRectInfo.sDstRectConfig.uPosY           = pCropRectInfo->sDstRectConfig.uPosY;
    pScalarConfig->sRectInfo.sDstRectConfig.uWidthInPixels  = pCropRectInfo->sDstRectConfig.uWidthInPixels;
    pScalarConfig->sRectInfo.sDstRectConfig.uHeightInPixels = pCropRectInfo->sDstRectConfig.uHeightInPixels;

    for (uI = 0; uI < HAL_MDP_SOURCE_PIPE_MAX_COLOR_COMPONENTS; uI++)
    {
      HAL_MDP_PixelExtOverrideConfigType *pPixelExtOverrideConfig = &pPixelExtConfig->aPixelExtComponentConfig[uI].sPixelExtOverrideConfig;

       //
      // Calculate Pixel Extension
      //
      pPixelExtOverrideConfig->iLeftOverFetch   =  0;
      pPixelExtOverrideConfig->uLeftRepeat       = 0;
      pPixelExtOverrideConfig->iRightOverFetch   = 0;
      pPixelExtOverrideConfig->uRightRepeat      = 0;
      pPixelExtOverrideConfig->iTopOverFetch     = 0;
      pPixelExtOverrideConfig->uTopRepeat        = 0;
      pPixelExtOverrideConfig->iBottomOverFetch  = 0; 
      pPixelExtOverrideConfig->uBottomRepeat     = 0;
      pPixelExtOverrideConfig->uTopBottomReqPxls = pCropRectInfo->sSrcRectConfig.uHeightInPixels;
      pPixelExtOverrideConfig->uLeftRightReqPxls = pCropRectInfo->sSrcRectConfig.uWidthInPixels;
    }
  }

  return eStatus;  
}

/****************************************************************************
*
** FUNCTION: SetupSourcePath()
*/
/*!
* \brief
*   Local helper function to setup the MDP control path
*
* \param [in] pPanelConfig          - Panel configuration
*        [in] pSurfaceInfo          - Surface configuration
*
* \retval MDP_Status
*
****************************************************************************/
static MDP_Status SetupSourcePath(MDP_Panel_AttrType *pPanelConfig, MDPSurfaceInfo *pSurfaceInfo)
{
  MDP_Status                      eStatus = MDP_STATUS_OK;
  HAL_MDP_SourcePipeConfigType    sSourcePipeConfig;
  HAL_MDP_CropRectangleType       sCropRectInfo;
  HAL_MDP_SurfaceAttrType         sSurfaceInfo;
  HAL_MDP_PixelExtLayerConfigType sPixelExtConfig;
  HAL_MDP_FetchModeConfigType     sFetchModeConfig;
  MDPRectType                     sSrcRect;
  uint32                          uI;

  for(uI = 0; uI < pPanelConfig->uNumMixers; uI++)
  {    
    HAL_MDP_LayerType eLayerType = MDP_GET_SOURCEPIPE_TYPE(pPanelConfig->sDisplayControlPath[uI].eSourcePipeId).eLayerType;
            
    MDP_OSAL_MEMZERO(&sSurfaceInfo, sizeof(HAL_MDP_SurfaceAttrType));
    sSurfaceInfo.ePixelFormat   = MDP_GET_PIXELFMTMAP(pSurfaceInfo->ePixelFormat).eHALPixelFormat;
    sSurfaceInfo.uHeightInPixel = pSurfaceInfo->uHeight;
    sSurfaceInfo.uWidthInPixel  = pSurfaceInfo->uWidth;
    sSurfaceInfo.uPlanes.sRgb.uStrideInBytes           = pSurfaceInfo->uPlane0Stride;
    sSurfaceInfo.uPlanes.sRgb.sDeviceAddress.iQuadPart = (int64)pSurfaceInfo->pPlane0Offset;

    MDP_OSAL_MEMZERO(&sCropRectInfo, sizeof(HAL_MDP_CropRectangleType));
    // Default the cropping rect to the surface size
    MDP_OSAL_MEMZERO(&sSrcRect, sizeof(MDPRectType));    
    sSrcRect.uHeight = pSurfaceInfo->uHeight;
    sSrcRect.uWidth  = pSurfaceInfo->uWidth;
    SetupCroppingRectangle(pPanelConfig, &sSrcRect, &sCropRectInfo, uI);
    
    MDP_OSAL_MEMZERO(&sPixelExtConfig, sizeof(HAL_MDP_PixelExtLayerConfigType));
    
    if (MDP_STATUS_OK == (eStatus = ScaleHelperConfigure(&sPixelExtConfig, &sCropRectInfo, pPanelConfig->uNumMixers, 0x00)))
    {
      MDP_OSAL_MEMZERO(&sSourcePipeConfig, sizeof(HAL_MDP_SourcePipeConfigType));
      sSourcePipeConfig.psSurfaceInfo         = &sSurfaceInfo;        
      sSourcePipeConfig.psCropRectInfo        = &sCropRectInfo;
      sSourcePipeConfig.psPixelExtLayerConfig = &sPixelExtConfig;

      // If the panel is inverted we need to flip the source fetch 180 degrees.
      if (MDP_ROTATE_180 == pPanelConfig->ePhysPanelOrientation)
      {
        // Flip vertically and horizontally to get 180 rotation.  
        MDP_OSAL_MEMZERO(&sFetchModeConfig, sizeof(HAL_MDP_FetchModeConfigType));
        sFetchModeConfig.eFetchConfigType = HAL_MDP_FETCH_DIRECTION_DOWN_TO_UP | HAL_MDP_FETCH_DIRECTION_RIGHT_TO_LEFT;

        sSourcePipeConfig.psFetchInfo     = &sFetchModeConfig;
      }

      // Start Source Pipe configuration (default is RGB)
      if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_SourcePipe_Setup(pPanelConfig->sDisplayControlPath[uI].eSourcePipeId,
                                                              eLayerType,
                                                              &sSourcePipeConfig,
                                                              0x0))
      {
          MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_SourcePipe_Setup failed!\n");
          eStatus = MDP_STATUS_FAILED;
      }
    }
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: StartInterface()
*/
/*!
* \brief
*   Start interface module
*
* \param [in] eInterfaceId  - Interface id
*        [in] pPanelConfig  - panel configuration info
*
* \retval MDP_Status
*
****************************************************************************/
static MDP_Status StartInterface(HAL_MDP_InterfaceId eInterfaceId, MDP_Panel_AttrType *pPanelConfig)
{
    MDP_Status                             eStatus = MDP_STATUS_OK;
    HAL_MDP_Interface_VsyncEnableType      sVsyncEnable;
    HAL_MDP_Interface_TEEnableType         sTeEnable;
    HAL_MDP_InterfaceConfigType            sInterfaceConfig;

    MDP_OSAL_MEMZERO(&sInterfaceConfig, sizeof(HAL_MDP_InterfaceConfigType));
    MDP_OSAL_MEMZERO(&sVsyncEnable,     sizeof(HAL_MDP_Interface_VsyncEnableType));
    MDP_OSAL_MEMZERO(&sTeEnable,        sizeof(HAL_MDP_Interface_TEEnableType));

    sInterfaceConfig.pIntrVsyncEnable = &sVsyncEnable;
    sInterfaceConfig.pIntrTEEnable    = &sTeEnable;

    // Always enable vsync counter for command mode
    sVsyncEnable.bEnable             = TRUE;

    // TE enable is actually controlled in panel configuration
    if (pPanelConfig->uAttrs.sDsi.sTE.bTECheckSetDisable)
    {
      sTeEnable.bEnable = FALSE;
    }
    else
    {
      sTeEnable.bEnable = TRUE;
    }

    if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_Interface_Setup(eInterfaceId, &sInterfaceConfig, 0x00))
    {
        eStatus = MDP_STATUS_FAILED;
    }

    // TE is not used, therefore, don't need to enable TE
    return eStatus;
}

/****************************************************************************
*
** FUNCTION: SetupAutoRefresh()
*/
/*!
* \brief
*   Setup the MDP for a basic single layer operation
*
* \param [in] pPanelConfig       - The display configuration to setup
*        [in] uPipeIndex         - Pipe index (only used in case of Dual path)
*
* \retval MDP_Status
*
****************************************************************************/
static MDP_Status  SetupAutoRefresh(MDP_Panel_AttrType *pPanelConfig, uint32 uPipeIndex)
{
  MDP_Status  eStatus = MDP_STATUS_OK;

  HAL_MDP_Interface_AutoRefreshEnableType sAutoRefreshConfig;
  HAL_MDP_InterfaceConfigType             sInterfaceConfig;

  MDP_OSAL_MEMZERO(&sInterfaceConfig,   sizeof(HAL_MDP_InterfaceConfigType));
  MDP_OSAL_MEMZERO(&sAutoRefreshConfig, sizeof(HAL_MDP_Interface_AutoRefreshEnableType));

  // auto-refresh info
  sAutoRefreshConfig.bEnable = pPanelConfig->uAttrs.sDsi.bEnableAutoRefresh;

  sInterfaceConfig.pIntrAutoRefreshEnable = &sAutoRefreshConfig;

  if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_Interface_Setup(pPanelConfig->sDisplayControlPath[uPipeIndex].eInterfaceId, &sInterfaceConfig, 0x00))
  {
    eStatus = MDP_STATUS_FAILED;
  }

  return eStatus;
}

/*=========================================================================
      MDP Lib APIs
==========================================================================*/

/****************************************************************************
*
** FUNCTION: MDPCommitPipe()
*/
/*!
* \brief
*   Commit the MDP to trigger an update to display
*
* \param [in] eDisplayId       - The display to commit
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPCommitPipe(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status                    eStatus = MDP_STATUS_OK;
  HAL_MDP_ControlPathFlushType  sFlushType;
  uint32                        uFlags = 0;
  uint32                        uI;
  
  for (uI = 0; uI < pPanelConfig->uNumMixers; uI++)
  {
    MDP_OSAL_MEMZERO(&sFlushType, sizeof(HAL_MDP_ControlPathFlushType));

    if (HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE > pPanelConfig->sDisplayControlPath[uI].eInterfaceId)
    {
      sFlushType.uFlushModules = HAL_CONTROL_PATH_FLUSH_INTF;
    }
    else
    {
      sFlushType.uFlushModules = HAL_CONTROL_PATH_FLUSH_WB;
    }
  
    if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_ControlPath_Commit(pPanelConfig->sDisplayControlPath[uI].eControlPathId,
                                                              &sFlushType,
                                                              HAL_GET_MDP_INTERFACE_TYPE(pPanelConfig->sDisplayControlPath[uI].eInterfaceId),
                                                              uFlags))
    {
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_ControlPath_Commit failed!\n");
      eStatus = MDP_STATUS_FAILED;
    }
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPSetupPipe()
*/
/*!
* \brief
*   Setup the MDP for a basic single layer operation
*
* \param [in] pPanelConfig       - The display configuration to setup
*        [in] pSurfaceInfo       - The source surface to fetch from
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPSetupPipe(MDP_Panel_AttrType *pPanelConfig, MDPSurfaceInfo *pSurfaceInfo)
{
   MDP_Status    eStatus = MDP_STATUS_OK;

   if ((NULL == pPanelConfig) ||
       (NULL == pSurfaceInfo))
   {
      eStatus = MDP_STATUS_BAD_PARAM;
   }
   else if ((pPanelConfig->eDisplayId >= MDP_DISPLAY_MAX) ||
            (pSurfaceInfo->ePixelFormat >= MDP_PIXEL_FORMAT_MAX))
   {
      eStatus = MDP_STATUS_BAD_PARAM;
   }
   else if (TRUE == MDP_GET_PIXELFMTMAP(pSurfaceInfo->ePixelFormat).bYUV)
   {
      // Don't support YUV formats
      eStatus = MDP_STATUS_BAD_PARAM;
   }
   else if (MDP_STATUS_OK != (eStatus = UpdateDisplayControlpathInfo(pPanelConfig)))
   {
       // Control path update failed
   }
   else if (MDP_STATUS_OK != (eStatus = SetupControlPath(pPanelConfig)))
   {
     // Control path failed
   }
   else if (MDP_STATUS_OK != (eStatus = SetupSourcePath(pPanelConfig, pSurfaceInfo)))
   {
     // Source path failed
   }
   else if (MDP_STATUS_OK != (eStatus = SetupInterface(pPanelConfig)))
   {
     // Interface config has failed
   }
   else if (MDP_STATUS_OK != (eStatus = SetupPingPong(pPanelConfig)))
   {
     // PingPong config has failed
   }
   else
   {
      // Everything has passed, commit the changes
      HAL_MDP_ControlPathFlushType  sFlushType;
      uint32                        uI;

      for (uI = 0; uI < pPanelConfig->uNumMixers; uI++)
      {        
        MDP_OSAL_MEMZERO(&sFlushType, sizeof(HAL_MDP_ControlPathFlushType));
        sFlushType.uFlushModules = MDPGetFlushFlags(pPanelConfig, uI, MDP_FLUSH_ALL);
        
        if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_ControlPath_Commit(pPanelConfig->sDisplayControlPath[uI].eControlPathId,
                                                                  &sFlushType,
                                                                  HAL_GET_MDP_INTERFACE_TYPE(pPanelConfig->sDisplayControlPath[uI].eInterfaceId),
                                                                  0x0))
        {
            MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_ControlPath_Commit failed!\n");
            eStatus = MDP_STATUS_FAILED;
        }
      }
   }

   return eStatus;
}


/****************************************************************************
*
** FUNCTION: MDPStartPipe()
*/
/*!
* \brief
*   Start the MDP pipe 
*
* \param [in] pPanelConfig       - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPStartPipe(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status    eStatus = MDP_STATUS_OK;

  if ((NULL == pPanelConfig )||
      (MDP_DISPLAY_MAX <= pPanelConfig->eDisplayId) )
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    uint32 uI;

    for (uI = 0; uI < pPanelConfig->uNumMixers; uI++)
    {
      // Ensure same interface will not be configured twice.
      if ((uI                                                    > MDP_MASTER_PATH_INDEX)  &&
          (pPanelConfig->sDisplayControlPath[uI-1].eInterfaceId == pPanelConfig->sDisplayControlPath[uI].eInterfaceId))
      {
        continue;
      }

      MDP_DisplayInterfaceMapType *pInterfaceMap = (MDP_DisplayInterfaceMapType*)MDP_GET_INTFMODEHMAP(pPanelConfig->ePhysConnect,
                                                                                                      uI,
                                                                                                      pPanelConfig->uAttrs.sDsi.uControllerSinglePrimary);

      if (HAL_MDP_INTERFACE_NONE == pPanelConfig->sDisplayControlPath[uI].eInterfaceId)
      {
        continue;
      }

      // command mode
      if(HAL_MDP_INTERFACE_MODE_COMMAND == pInterfaceMap->eInterfaceMode)
      {
        // disable interface timing
        if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_Interface_Enable(pPanelConfig->sDisplayControlPath[uI].eInterfaceId,
                                                                FALSE,
                                                                0x0))
        {
          MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_Interface_Enable failed!\n");
          eStatus = MDP_STATUS_FAILED;
        }
        else if (MDP_STATUS_OK != (eStatus = SetupAutoRefresh(pPanelConfig, uI)))
        {
          MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: SetupAutoRefresh failed!\n");
        }
        else 
        {
          // start vsync counter in the interface block
          eStatus = StartInterface(pPanelConfig->sDisplayControlPath[uI].eInterfaceId, pPanelConfig);
        }
      }
      // video mode
      else
      {
	    //enable interface timing for master intf
		if (MDP_MASTER_PATH_INDEX == uI)
		{
		  // enable the interface timing
		  if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_Interface_Enable(pPanelConfig->sDisplayControlPath[uI].eInterfaceId,
		 														 TRUE,
		 														 0x0))
		  {
		  MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_Interface_Enable failed!\n");
		  eStatus = MDP_STATUS_FAILED;
		  }
		}
      }
    }

    if (MDP_STATUS_OK == eStatus)
    {
      eStatus = MDPCommitPipe(pPanelConfig);
    }
  }

  return eStatus;
}




/****************************************************************************
*
** FUNCTION: MDPStopPipe()
*/
/*!
* \brief
*   Stop the MDP pipe 
*
* \param [in] pPanelConfig       - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPStopPipe(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status    eStatus = MDP_STATUS_OK;

  if (NULL == pPanelConfig) 
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    HAL_MDP_InterfaceConfigType            sInterfaceConfig;
    HAL_MDP_Interface_VsyncEnableType      sVsyncEnable;
    HAL_MDP_Interface_TEEnableType         sTeEnable;
    uint32                                 uI;

#ifdef MDP_PRESIL
    // If the master is writeback then stop the timer
    if (HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE < pPanelConfig->sDisplayControlPath[MDP_MASTER_PATH_INDEX].eInterfaceId)
    {
      gBS->CloseEvent(pPanelConfig->hWritebackTimerEvent);
    }
#endif

    for (uI = 0; uI < pPanelConfig->uNumInterfaces; uI++)
    {
      if (HAL_MDP_INTERFACE_NONE == pPanelConfig->sDisplayControlPath[uI].eInterfaceId)
      {
        continue;
      }

      // Disable interface vsync counter and tear check
      MDP_OSAL_MEMZERO(&sInterfaceConfig, sizeof(HAL_MDP_InterfaceConfigType)); 
      MDP_OSAL_MEMZERO(&sVsyncEnable, sizeof(HAL_MDP_Interface_VsyncEnableType));    
      MDP_OSAL_MEMZERO(&sTeEnable, sizeof(HAL_MDP_Interface_TEEnableType));

      sInterfaceConfig.pIntrVsyncEnable   = &sVsyncEnable;
      sInterfaceConfig.pIntrTEEnable      = &sTeEnable;
      
      if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_Interface_Setup(pPanelConfig->sDisplayControlPath[uI].eInterfaceId, &sInterfaceConfig, 0x00))
      {
          MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_PingPong_Setup failed!\n");
      }
            
      // Disable interface timing
      if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_Interface_Enable(pPanelConfig->sDisplayControlPath[uI].eInterfaceId,
                                                              FALSE,
                                                              0x0))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_Interface_Enable failed!\n");
      }      
    }

    /* Release writeback buffer, if allocated */
    if (pPanelConfig->sWritebackAddress.iQuadPart)
    {
      MDP_OSAL_FREE((VOID*)pPanelConfig->sWritebackAddress.iQuadPart);
    }
  }

  return eStatus;
}



/****************************************************************************
*
** FUNCTION: MDPOutputConstantColor()
*/
/*!
* \brief
*   Configure MDP to enable/disable constant color output to panel
*
*   COMMAND MODE:
*      bEnabled = TRUE  :  Set up MDP color fill and trigger one frame to panel
*      bEnabled = FALSE :  Remove MDP color fill settings
*   VIDEO MODE:
*      bEnabled = TRUE  :  Set up MDP color fill and enable interface
*      bEnabled = FALSE :  Remove MDP color fill settings
*
* \param [in] pPanelConfig       - The panel configuration
* \param [in] uConstantColor     - Constant color to output
* \param [in] bEnabled           - Flag to enable/disable constant color output
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPOutputConstantColor(MDP_Panel_AttrType *pPanelConfig, uint32 uConstantColor, bool32 bEnabled)
{
  MDP_Status    eStatus = MDP_STATUS_OK;

  if (NULL == pPanelConfig) 
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    uint32  uI;

    for (uI = 0; uI < pPanelConfig->uNumMixers; uI++)
    {
      MDP_DisplayInterfaceMapType *pInterfaceMap    = (MDP_DisplayInterfaceMapType*)MDP_GET_INTFMODEHMAP(pPanelConfig->ePhysConnect,
                                                                                                         uI,
                                                                                                         pPanelConfig->uAttrs.sDsi.uControllerSinglePrimary);
      uint32                       uFlags           = 0;
      HAL_MDP_ControlPathFlushType sFlushType;

      if (HAL_MDP_INTERFACE_NONE == pPanelConfig->sDisplayControlPath[uI].eInterfaceId)
      {
        continue;
      }
      
      if (TRUE == bEnabled)
      {
        HAL_MDP_ControlPath_Mixer_BorderColorConfigType sBorderColorConfig;
        HAL_MDP_ControlPath_Mixer_ZOrderConfigType      sZorderConfig;
        HAL_MDP_Mixer_BorderColorConfigType             sBorderOutputColorConfig;
        HAL_MDP_LayerMixerConfigType                    sLayerMixerCfg;
        HAL_MDP_ControlPath_MixerConfigType             sMixerConfig;
        HAL_MDP_ControlPathConfigType                   sControlPathConfig;

        /*
         * This function is special for command mode to clear the garbage data in the panel display buffer.
         * Calling sequence:
         *
         * DisplayDxe_SetMode --> MDPSetMode --> MDPSetupPipe
         *                                   --> MDPPanelInit --> DSIDriver_SetMode --> send init sequence
         *
         * So, at this moment, MDP pipe has been setup already, we need to restore the changes after constant color been sent out
         *
         */
        // Setup border color
        MDP_OSAL_MEMZERO(&sBorderColorConfig, sizeof(HAL_MDP_ControlPath_Mixer_BorderColorConfigType));
        sBorderColorConfig.bBorderColorEnable = TRUE;

        // Setup the ZOrder
        MDP_OSAL_MEMZERO(&sZorderConfig, sizeof(HAL_MDP_ControlPath_Mixer_ZOrderConfigType));

        // Setup the layer mixer config for constant color
        MDP_OSAL_MEMZERO(&sBorderOutputColorConfig, sizeof(HAL_MDP_Mixer_BorderColorConfigType));
        sBorderOutputColorConfig.uColorValue = uConstantColor;

        // Setup the mixer configuration
        MDP_OSAL_MEMZERO(&sMixerConfig, sizeof(HAL_MDP_ControlPath_MixerConfigType));
        sMixerConfig.eMixerId                 = pPanelConfig->sDisplayControlPath[uI].eLayerMixerId;
        sMixerConfig.psMixerZOrderConfig      = &sZorderConfig;
        sMixerConfig.psMixerBorderColorConfig = &sBorderColorConfig;

        // Setup the layer mixer config
        MDP_OSAL_MEMZERO(&sLayerMixerCfg, sizeof(HAL_MDP_LayerMixerConfigType));
        sLayerMixerCfg.eLayerMixerId       = pPanelConfig->sDisplayControlPath[uI].eLayerMixerId;
        sLayerMixerCfg.psBorderColorConfig = &sBorderOutputColorConfig;

        // Setup the control path
        MDP_OSAL_MEMZERO(&sControlPathConfig, sizeof(HAL_MDP_ControlPathConfigType));
        sControlPathConfig.psMixerConfig      = &sMixerConfig;
        sControlPathConfig.psLayerMixerConfig = &sLayerMixerCfg;    

        // Program the control path
        if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_ControlPath_Setup(pPanelConfig->sDisplayControlPath[uI].eControlPathId, &sControlPathConfig, 0))
        {
          MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_ControlPath_Setup failed!\n");
          eStatus = MDP_STATUS_FAILED;
        }

        if (HAL_MDP_INTERFACE_MODE_COMMAND == pInterfaceMap->eInterfaceMode)
        {
          // Flag to force overlay processor start for command mode
          uFlags |= HAL_MDP_FLAGS_COMMIT_FORCE_UPDATE;
        }
      }
      else
      {
        // Setup the control path based on panel config
        SetupControlPath(pPanelConfig);        
        // Don't force overlay processor start, otherwise previous constant color will be overwritten
        // Only update MDP registers.
        uFlags = 0;
      }

      // Commit the changes
      MDP_OSAL_MEMZERO(&sFlushType, sizeof(HAL_MDP_ControlPathFlushType));
      sFlushType.uFlushModules |= MDPGetFlushFlags(pPanelConfig, uI, MDP_FLUSH_LAYER | MDP_FLUSH_ZORDER);
      
      if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_ControlPath_Commit(pPanelConfig->sDisplayControlPath[uI].eControlPathId,
                                                                &sFlushType,
                                                                HAL_GET_MDP_INTERFACE_TYPE(pPanelConfig->sDisplayControlPath[uI].eInterfaceId),
                                                                uFlags))
      {
          MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_ControlPath_Commit failed!\n");
          eStatus = MDP_STATUS_FAILED;
      } 

      if ((TRUE == bEnabled) && 
          (HAL_MDP_INTERFACE_MODE_VIDEO == pInterfaceMap->eInterfaceMode)) 
      {
        // Enable the interface timing engine.
        if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_Interface_Enable(pInterfaceMap->eInterfaceId,
                                                                TRUE,
                                                                0x0))
        {
          MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_Interface_Enable failed!\n");
          eStatus = MDP_STATUS_FAILED;  
        }
      }      
    }
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPGetFlushFlags()
*/
/*!
* \brief
*     Helper to calculate the pixel extension length
*
* \param [in] pPanelInfo          - The panel configuration
*        [in] uMixerIndex         - Mixer ID (left or right mixer)
*        [in] uFlushMask          - Mask for which modules' flush bits are to be returned
*
* \retval int32 - Flash mask bits (HAL_ControlPath_Flush)
*
****************************************************************************/ 
uint32 MDPGetFlushFlags(MDP_Panel_AttrType         *pPanelInfo,
                        uint32                     uMixerIndex, 
                        MDP_FlushModule_Type       uFlushMask)
{ 
  uint32                      uFlushFlags         = 0;
  MDP_Display_IDType          eDisplayId          = 0;
  uint32                      ePhysConnect        = 0;
  
  if (NULL == pPanelInfo)
  {
      // Invalid input just return
  }
  else
  {
      eDisplayId          = pPanelInfo->eDisplayId;
      ePhysConnect        = pPanelInfo->ePhysConnect;

      if ((eDisplayId  >= MDP_DISPLAY_MAX) ||
          (uMixerIndex >= MDP_QUADPIPE_NUM_MIXERS))
      {
        // Invalid input just return
      }
      else
      {
        const MDP_DisplayCtrlPathMapType *pDisplayControlPath = MDP_GET_CTRLPATHMAP(eDisplayId, uMixerIndex);

        // Layer level flushes - Source pipe changes
        if (MDP_FLUSH_LAYER & uFlushMask)
        {
            const HAL_ControlPath_Flush aSourcePipeFlushMap[HAL_MDP_SOURCE_PIPE_MAX] = {
             HAL_CONTROL_PATH_FLUSH_NONE,     // HAL_MDP_SOURCE_PIPE_NONE
             HAL_CONTROL_PATH_FLUSH_VIG0,     // HAL_MDP_SOURCE_PIPE_VIG_0
             HAL_CONTROL_PATH_FLUSH_VIG1,     // HAL_MDP_SOURCE_PIPE_VIG_1,
             HAL_CONTROL_PATH_FLUSH_VIG2,     // HAL_MDP_SOURCE_PIPE_VIG_2,
             HAL_CONTROL_PATH_FLUSH_VIG3,     // HAL_MDP_SOURCE_PIPE_VIG_3,
             HAL_CONTROL_PATH_FLUSH_NONE,     // HAL_MDP_SOURCE_PIPE_VIG_4,
             
             HAL_CONTROL_PATH_FLUSH_RGB0,     // HAL_MDP_SOURCE_PIPE_RGB_0
             HAL_CONTROL_PATH_FLUSH_RGB1,     // HAL_MDP_SOURCE_PIPE_RGB_1,
             HAL_CONTROL_PATH_FLUSH_RGB2,     // HAL_MDP_SOURCE_PIPE_RGB_2,
             HAL_CONTROL_PATH_FLUSH_RGB3,     // HAL_MDP_SOURCE_PIPE_RGB_3,
             HAL_CONTROL_PATH_FLUSH_NONE,     // HAL_MDP_SOURCE_PIPE_RGB_4,
            
             HAL_CONTROL_PATH_FLUSH_DMA0,     // HAL_MDP_SOURCE_PIPE_DMA_0           = HAL_MDP_SOURCE_PIPE_DMA_BASE,
             HAL_CONTROL_PATH_FLUSH_DMA1,     // HAL_MDP_SOURCE_PIPE_DMA_1,
             HAL_CONTROL_PATH_FLUSH_DMA2,     // HAL_MDP_SOURCE_PIPE_DMA_2,
             HAL_CONTROL_PATH_FLUSH_DMA3,     // HAL_MDP_SOURCE_PIPE_DMA_3,
             HAL_CONTROL_PATH_FLUSH_NONE,     // HAL_MDP_SOURCE_PIPE_DMA_4,
            
             HAL_CONTROL_PATH_FLUSH_CURSOR0,  // HAL_MDP_SOURCE_PIPE_CURSOR_0        = HAL_MDP_SOURCE_PIPE_CURSOR_BASE,
             HAL_CONTROL_PATH_FLUSH_CURSOR1,  // HAL_MDP_SOURCE_PIPE_CURSOR_1,
             HAL_CONTROL_PATH_FLUSH_NONE,     // HAL_MDP_SOURCE_PIPE_CURSOR_2,
             HAL_CONTROL_PATH_FLUSH_NONE,     // HAL_MDP_SOURCE_PIPE_CURSOR_3,
             HAL_CONTROL_PATH_FLUSH_NONE      // HAL_MDP_SOURCE_PIPE_CURSOR_4
              };

             if (pDisplayControlPath->eSourcePipeId < HAL_MDP_SOURCE_PIPE_MAX)
             {
               uFlushFlags |= aSourcePipeFlushMap[pDisplayControlPath->eSourcePipeId];
             }
        }

        // Mixer level flush - Blending operation changes
        if (MDP_FLUSH_MIXER & uFlushMask)
        {
         const HAL_ControlPath_Flush aMixerPipeFlushMap[HAL_MDP_LAYER_MIXER_MAX] =
                {
                    HAL_CONTROL_PATH_FLUSH_NONE, // HAL_MDP_LAYER_MIXER_NONE
                    HAL_CONTROL_PATH_FLUSH_LM0,  // HAL_MDP_LAYER_MIXER_0
                    HAL_CONTROL_PATH_FLUSH_LM1,  // HAL_MDP_LAYER_MIXER_1
                    HAL_CONTROL_PATH_FLUSH_LM2,  // HAL_MDP_LAYER_MIXER_2
                    HAL_CONTROL_PATH_FLUSH_LM3,  // HAL_MDP_LAYER_MIXER_3
                    HAL_CONTROL_PATH_FLUSH_LM4,  // HAL_MDP_LAYER_MIXER_4
                    HAL_CONTROL_PATH_FLUSH_LM5,  // HAL_MDP_LAYER_MIXER_5
                    HAL_CONTROL_PATH_FLUSH_NONE, 
                };

            if (pDisplayControlPath->eLayerMixerId < HAL_MDP_LAYER_MIXER_MAX)
            {
              uFlushFlags |= aMixerPipeFlushMap[pDisplayControlPath->eLayerMixerId];
            }
        }

        // Control Path level flush - Zorder, Attach, detach operations
        if (MDP_FLUSH_CTLPATH & uFlushMask)
        {
          uFlushFlags |= HAL_CONTROL_PATH_ATTACH_DETACH;
        }

        // DSPP level flush - Histogram, IGC LUT
        if (MDP_FLUSH_DSPP & uFlushMask)
        {
          const HAL_ControlPath_Flush aDsppPipeFlushMap[HAL_MDP_DESTINATION_PIPE_MAX] = 
              {
                    HAL_CONTROL_PATH_FLUSH_NONE,  // HAL_MDP_DESTINATION_PIPE_NONE
                    HAL_CONTROL_PATH_FLUSH_DSPP0, // HAL_MDP_DESTINATION_PIPE_0
                    HAL_CONTROL_PATH_FLUSH_DSPP1, // HAL_MDP_DESTINATION_PIPE_1
                    HAL_CONTROL_PATH_FLUSH_DSPP2, // HAL_MDP_DESTINATION_PIPE_2
                    HAL_CONTROL_PATH_FLUSH_DSPP3, // HAL_MDP_DESTINATION_PIPE_3
                    HAL_CONTROL_PATH_FLUSH_NONE,  
              };

          if (pDisplayControlPath->eDestinationPipeId < HAL_MDP_DESTINATION_PIPE_MAX)
          {
            uFlushFlags |= aDsppPipeFlushMap[pDisplayControlPath->eDestinationPipeId];
          }
        }

        // Interface level flush - Dynamic Refresh Rate, Programmable fetch start
        if (MDP_FLUSH_INTERFACE & uFlushMask)
        {
          uint32 uInterface2Prim                            = pPanelInfo->uAttrs.sDsi.uControllerSinglePrimary ? DSI_1_SINGLE_PRIMARY : DSI_0_SINGLE_PRIMARY;
          const  MDP_DisplayInterfaceMapType *pInterfaceMap = (MDP_DisplayInterfaceMapType *)MDP_GET_INTFMODEHMAP(ePhysConnect,
                                                                                                                  uMixerIndex,
                                                                                                                  uInterface2Prim);

          if (HAL_MDP_INTERFACE_MAX > pInterfaceMap->eInterfaceId)
          {
            if (HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE > pInterfaceMap->eInterfaceId)
            {
              uFlushFlags |= HAL_CONTROL_PATH_FLUSH_INTF;
            }
            else
            {
              uFlushFlags |= HAL_CONTROL_PATH_FLUSH_WB;
            }
          }
        }
      }  
  }
  return uFlushFlags;
}


/****************************************************************************
*
** FUNCTION: MDPSetupSourcePipe()
*/
/*!
* \brief
*   Reconfigure the MDP source Pipes, allows for configuring the surface,
*  source and dest rects.
*
* \param [in] pPanelConfig       - The display configuration to setup
*        [in] pSurfaceInfo       - (Optional) The source surface to fetch from
*        [in] pSrcRect           - (Optional) The source cropping rectangle.
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPSetupSourcePipe(MDP_Panel_AttrType *pPanelConfig, MDPSurfaceInfo *pSurfaceInfo, MDPRectType *pSrcRect)
{
   MDP_Status    eStatus = MDP_STATUS_OK;

   if (NULL == pPanelConfig)
   {
      eStatus = MDP_STATUS_BAD_PARAM;
   }
   else if ((NULL == pSurfaceInfo) &&
            (NULL == pSrcRect))
   {
      // Cannot give NULL surface and source rectangle
      eStatus = MDP_STATUS_BAD_PARAM;   
   }
   else if (pPanelConfig->eDisplayId >= MDP_DISPLAY_MAX) 
   {
      eStatus = MDP_STATUS_BAD_PARAM;
   }
   else if ((NULL != pSurfaceInfo) &&
            (pSurfaceInfo->ePixelFormat >= MDP_PIXEL_FORMAT_MAX))
   {
      eStatus = MDP_STATUS_BAD_PARAM;   
   }
   else
   {
        uint32                        uI;

        for(uI = 0; uI < pPanelConfig->uNumMixers; uI++)
        {
            HAL_MDP_LayerType                eLayerType    = MDP_GET_SOURCEPIPE_TYPE(pPanelConfig->sDisplayControlPath[uI].eSourcePipeId).eLayerType;
            HAL_MDP_CropRectangleType        sCropRectInfo;
            HAL_MDP_SurfaceAttrType          sSurfaceInfo;
            HAL_MDP_SourcePipeConfigType     sSourcePipeConfig;
            HAL_MDP_PixelExtLayerConfigType  sPixelExtConfig;
            HAL_MDP_FetchModeConfigType      sFetchModeConfig;
        
            // Initialize source pipe structure
            MDP_OSAL_MEMZERO(&sSourcePipeConfig, sizeof(HAL_MDP_SourcePipeConfigType));
            MDP_OSAL_MEMZERO(&sPixelExtConfig, sizeof(HAL_MDP_PixelExtLayerConfigType));            
            MDP_OSAL_MEMZERO(&sFetchModeConfig, sizeof(HAL_MDP_FetchModeConfigType));

            // Handle surface change requests first
            if (NULL != pSurfaceInfo)
            {
                MDP_OSAL_MEMZERO(&sSurfaceInfo, sizeof(HAL_MDP_SurfaceAttrType));
                sSurfaceInfo.ePixelFormat                          = MDP_GET_PIXELFMTMAP(pSurfaceInfo->ePixelFormat).eHALPixelFormat;
                sSurfaceInfo.uHeightInPixel                        = pSurfaceInfo->uHeight;
                sSurfaceInfo.uWidthInPixel                         = pSurfaceInfo->uWidth;
                sSurfaceInfo.uPlanes.sRgb.uStrideInBytes           = pSurfaceInfo->uPlane0Stride;
                sSurfaceInfo.uPlanes.sRgb.sDeviceAddress.iQuadPart = (int64)pSurfaceInfo->pPlane0Offset;
                // Assign the new surface
                sSourcePipeConfig.psSurfaceInfo                    = &sSurfaceInfo;
            }

            // Handle rectangle changes
            if (NULL != pSrcRect)
            {
                SetupCroppingRectangle(pPanelConfig, pSrcRect, &sCropRectInfo, uI);        

                if (MDP_STATUS_OK == (eStatus = ScaleHelperConfigure(&sPixelExtConfig, &sCropRectInfo, pPanelConfig->uNumMixers, 0x00)))
                {                
                   sSourcePipeConfig.psCropRectInfo        = &sCropRectInfo;
                   sSourcePipeConfig.psPixelExtLayerConfig = &sPixelExtConfig;
                }
            }

            // If the panel is inverted we need to flip the source fetch 180 degrees.
            if (MDP_ROTATE_180 == pPanelConfig->ePhysPanelOrientation)
            {
               sFetchModeConfig.eFetchConfigType = HAL_MDP_FETCH_DIRECTION_DOWN_TO_UP | HAL_MDP_FETCH_DIRECTION_RIGHT_TO_LEFT;
            }
            sSourcePipeConfig.psFetchInfo = &sFetchModeConfig;
            
            // Start Source Pipe configuration (default is RGB)
            if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_SourcePipe_Setup(pPanelConfig->sDisplayControlPath[uI].eSourcePipeId,
                                                                    eLayerType,
                                                                    &sSourcePipeConfig,
                                                                    0x0))
            {
               MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_SourcePipe_Setup failed!\n");
               eStatus = MDP_STATUS_FAILED;
               break;
            }
         }

         // Everything has passed, commit the changes
         if (MDP_STATUS_OK == eStatus)
         {
             HAL_MDP_ControlPathFlushType  sFlushType;
             
             for (uI = 0; uI < pPanelConfig->uNumMixers; uI++)
             {        
                MDP_OSAL_MEMZERO(&sFlushType, sizeof(HAL_MDP_ControlPathFlushType));
                sFlushType.uFlushModules = MDPGetFlushFlags(pPanelConfig, uI, MDP_FLUSH_LAYER);
                
                if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_ControlPath_Commit(pPanelConfig->sDisplayControlPath[uI].eControlPathId,
                                                                          &sFlushType,
                                                                          HAL_MDP_INTERFACE_TYPE_PHYSICAL_CONNECT,
                                                                          0x0))
                {
                   MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: HAL_MDP_ControlPath_Commit failed!\n");
                   eStatus = MDP_STATUS_FAILED;
                   break;
                }
             }
         }
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPCRC8Calculation()
*/
/*!
* \brief
*   This function will calculation checksum 
*
* \param [in] pBuf : pointer to the start of buffer
*        [in] uSize: bytes to be calculated
*
* \retval UINT8
*
****************************************************************************/
UINT8 MDPCRC8Calculation(UINT8 *pBuf, UINT32 uSize)
{
    uint32 uDivisor = 0x1700;   /* prime number */
    uint32 uCRC    = 0; 
    uint8  *bp      = pBuf; 
    int    i,j;

    if (pBuf != NULL)
    {
       if (*bp == 0)   /* make sure first byte can be xored in even it is 0 */
       {
           uCRC = 0xff;
       }

       for (j = 0; j < uSize; j++, bp++)
       {
           uCRC ^= *bp; 

           uCRC <<= 8;

           for (i = 0; i < 8; i++)
           {
               if ((uCRC & 0x8000) != 0)
               {
                   uCRC ^= uDivisor;
               }
               uCRC <<= 1;
           }

           uCRC >>= 8;;
       }
    }

    return (uint8)uCRC;
}
