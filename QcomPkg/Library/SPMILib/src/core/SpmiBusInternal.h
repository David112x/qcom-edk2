/*
  @brief: This module provides an internal the interface to the 
  SPMI bus driver software.
  
  Copyright (c) 2015-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
  
  $DateTime: 2018/07/26 18:39:07 $
  $Header: //components/rel/boot.xf/3.2/QcomPkg/Library/SPMILib/src/core/SpmiBusInternal.h#1 $
  $Change: 16743401 $ 
  
                               Edit History
  Date      Description
  --------  -------------------------------------------------------------------
  11/10/15  Initial Version
*/
#ifndef SPMIBUSINTERNAL_H
#define SPMIBUSINTERNAL_H

#include "SpmiTypes.h"
#include "SpmiInfo.h"
#include "SpmiBusCfg.h"

typedef struct
{
    uint8 slaveId;      /**< Slave id of the SPMI device */
    uint8 periphId;     /**< Peripheral id on the slave device */
    uint16 channel;     /**< The channel number assigned to this peripheral (0 indexed) */
    boolean irqOwnerFlag;  /**< The EE id owner flag that will indicate if periph owner receives the interrupt*/
    uint8 periphOwner;  /**< The EE id owner that has write access to this peripheral */
} SpmiBusCfg_InternalChnlCfg;

/**
 * @brief Get the SPMI bus information structure
 *
 * This function gets the internal SPMI bus information 
 * structure.
 * 
 * @param[in] vSlaveId   Virtual slave Id of the device (see 
 *                       SpmiEncodeVSid)
 * 
 * @return Pointer to the device info structure, or 
 *         NULL on errror
 */
SpmiDevInfo *SpmiBus_GetDevInfo(uint8 vSlaveId);

SpmiInfo* SpmiBus_GetInfo(void);

Spmi_Result SpmiBusCfg_ConfigureChannel(SpmiDevInfo *pDev, SpmiBusCfg_InternalChnlCfg* chanCfg, boolean autoAssignChan);

#endif
