/**
  @file  ChargerLibTarget.h
  @brief Charger Lib Target specific API definitions.
*/
/*=============================================================================
  Copyright (c) 2018-2019 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/20/19   mr      Added support to move display functionality from Chargerlib to ChargerApp
 01/10/19   vr      support fg ship mode
 06/29/18   dc      Support Input Current Status API & Log
 05/14/18   ra      Support for Debug Accessory Mode
 03/27/17   va      Created

=============================================================================*/
#ifndef __CHARGERLIBTARGET_H__
#define __CHARGERLIBTARGET_H__

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
/**
Protocol Dependencies
*/
#include <Protocol/EFIPmicFg.h>
#include <Protocol/EFIPmicSchg.h>
#include <Protocol/EFIPmicPwrOn.h>
#include <Protocol/EFIPmicGlink.h>

/**
  ADC Dependencies
 */
#include <Protocol/EFIAdc.h>

/**
UEFI DEBUG Dependencies
*/
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TimerLib.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/

/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
EFI_STATUS ChargerLibTarget_Init( chargerlib_cfgdata_type *pChargerLibConfig );

EFI_STATUS ChargerLibTarget_ChargerEnable(BOOLEAN Enable);

EFI_STATUS ChargerLibTarget_SetMaxUsbCurrent(UINT32 MaxCurrent);

EFI_STATUS ChargerLibTarget_GetPortType(CHARGERLIB_CHGR_PORT_TYPE *pPortType);

EFI_STATUS ChargerLibTarget_GetBatteryPresence(BOOLEAN *BatteryPresence);

/* Enable Pmic AFP Mode */
EFI_STATUS ChargerLibTarget_EnableAfpMode(void);

/*This API resets the device.*/
EFI_STATUS ChargerLibTarget_ForceSysShutdown(ChgAppSysShutdownType ShutdownType);

/* API to pet charger watchdog */
EFI_STATUS ChargerLibTarget_PetChgWdog( void );

/*Enables WDOG if charging is already enabled*/
EFI_STATUS ChargerLibTarget_EnWdogIfCharging(void);

/* Dump Charger Peripheral */
EFI_STATUS ChargerLibTarget_DumpChargerPeripheral(void);

/* Dump FG SRAM Pheripheral */
EFI_STATUS ChargerLibTarget_DumpSram(BOOLEAN bDumpExplicit);

EFI_STATUS ChargerLibTarget_EnableHWJeita(BOOLEAN Enable);

EFI_STATUS ChargerLibTarget_WasChargerReinserted(BOOLEAN *pChargerReinserted);

EFI_STATUS ChargerLibTarget_ReRunAicl( VOID );

EFI_STATUS ChargerLibTarget_GetMaxUsbCurrent( UINT32 *pMaxCurrent);

EFI_STATUS ChargerLibTarget_GetBatteryStatus(chargerlib_batt_status_info *BatteryStatus);

EFI_STATUS ChargerLib_InitializeCharging( VOID );

EFI_STATUS ChargerLibTarget_Exit(VOID);

EFI_STATUS ChargerLibTarget_EnableWdog(BOOLEAN Enable);

EFI_STATUS ChargerLibTarget_PetChgWdog( VOID );

EFI_STATUS ChargerLibTarget_LoadProfile( OPTIONAL chargerlib_batt_profile_data *pBattProfileInfo);

EFI_STATUS ChargerLibTarget_GetBatteryProfileStatus(chargerlib_batt_profile_status *ProfileSts);

EFI_STATUS ChargerLibTarget_GetLogInfo(chargerlib_loginfo *gFileHandle );

EFI_STATUS ChargerLibTarget_GetChargingStatus(BOOLEAN *pChargingEnabled);

EFI_STATUS ChargerLibTarget_GetChargingPath(CHARGERLIB_ATTACHED_CHGR_TYPE *ChargingPath);

EFI_STATUS ChargerLibTarget_DcinSuspend(BOOLEAN Suspend);

EFI_STATUS ChargerLibTarget_DcinSetPowerLimit(UINT32 PowerInMicroW);

EFI_STATUS ChargerLibTarget_LedOn( BOOLEAN bLedOn);


EFI_STATUS ChargerLibTarget_SetDcinCurrent(UINT32 currentLimit);

EFI_STATUS ChargerLibTarget_IsDcinValid(BOOLEAN* pIsValid);

EFI_STATUS ChargerLibTarget_GetShipMode(BOOLEAN* Enabled);

EFI_STATUS ChargerLibTarget_IsChargingSupported(BOOLEAN *pChargingSupported);

EFI_STATUS ChargerLibTarget_TargetInit(VOID);

EFI_STATUS ChargerLibTarget_GetBatteryStatus(chargerlib_batt_status_info *pBatteryStatus);

EFI_STATUS ChargerLibTarget_GetIPCError(CHARGERLIB_CHARGING_ERROR_TYPES  *pChargingError);

EFI_STATUS ChargerLibTarget_GetIPCStatus(EFI_PM_GLINK_LINK_STATUS *PmicGLinkStatus);

EFI_STATUS ChargerLibTarget_LoadChargerFW();
EFI_STATUS ChargerLibTarget_LinkChargerFW();

EFI_STATUS ChargerLibTarget_GetSdpMaxCurrent(UINT32 *pSdpMaxCurrent);

EFI_STATUS ChargerLibTarget_GetChargingStatus(BOOLEAN *pChargingEnabled);

EFI_STATUS ChargerLibTarget_SetFccMaxCurrent(UINT32 FccMaxCurrent);

EFI_STATUS ChargerLibTarget_SetFvMaxVoltage(UINT32 VddMaxVoltage);
EFI_STATUS ChargerLibTarget_GetBatteryID(INT32 *pBatteryID);
EFI_STATUS ChargerLibTarget_UsbSuspend(BOOLEAN Enable);

EFI_STATUS ChargerLibTarget_DcinSuspend(BOOLEAN Suspend);

EFI_STATUS ChargeLibTarget_IsDcinValid(BOOLEAN* pIsValid);


EFI_STATUS ChargerLibTarget_GetTSensTemp( INT32 *TSensTemp);


EFI_STATUS ChargerLibTarget_GetHwJeitaStatus(CHARGERLIB_HW_JEITA_STATUS *pHwJeitaStatus);

EFI_STATUS ChargerLibTarget_GetChargingStatus(BOOLEAN *pChargingEnabled);

EFI_STATUS ChargerLibTarget_InitializeCharging();

EFI_STATUS ChargerLibTarget_GetChgWdogStatus( CHARGERLIB_CHG_WDOG_STS_TYPE *pChgWdogSts);

EFI_STATUS ChargerLibTarget_SetPon1Trigger(EFI_PM_PON_OPTION_BIT_TYPE bit_type, BOOLEAN bFlag );

EFI_STATUS ChargerLibTarget_GetADCReading(CONST CHAR8 *pszInputName, UINT32 uInputNameSize, EfiAdcResultType *pEfiAdcResult);

EFI_STATUS ChargerLibTarget_GetErrorAction( CHARGERLIB_CHARGING_ERROR_TYPES  ChargingErrorType, CHARGERLIB_ERROR_ACTION_TYPE *pErrorAction);

EFI_STATUS ChargerLibTarget_GetUnknownBatteryAction(CHARGERLIB_ERROR_ACTION_TYPE *pUnknownBatteryAction);

EFI_STATUS ChargerLibTarget_GetDebugBoardAction(CHARGERLIB_ERROR_ACTION_TYPE *pDebugBoardAction);

EFI_STATUS ChargerLibTarget_HandleRecovery();


EFI_STATUS ChargerLibTarget_DAMConnectSts(BOOLEAN *DAMConnectSts);

EFI_STATUS ChargerLibTarget_SetDAMIcl(DAM_SEL_ICL DAMIcl);

EFI_STATUS ChargerLibTarget_GetChargerInputStatus(CHARGERLIB_CHARGER_INPUT_STATUS *pChargerInputStatus);

EFI_STATUS ChargerLibTarget_GetBatteryTempStatus(CHARGERLIB_BATT_TEMP_STATUS *pBattTempStatus);

EFI_STATUS ChargerLibTarget_EnableFgShipMode();

EFI_STATUS ChargerLibTarget_GetDisplayImageType(CHARGERLIB_EVENT_DISP_IMAGE_TYPE *pDispImage);


#endif  /*__CHARGERLIBTARGET_H__ */

