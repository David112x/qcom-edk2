/** @file ChargerLibCommon.c

  Provide access to Charger Libraries

  Copyright (c) 2017-2019, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     ---------------------------------------------------------
05/20/19      mr      Added support to move display functionality from Chargerlib to ChargerApp
06/29/18      dc      Support Input Current Status API & Log
05/14/18      ra      Support for Debug Accessory Mode
03/30/18      va      charger lib to provide target specific file support
03/28/18      ra      Support for charging LED
03/26/18      ra      Support for HW JEITA status
03/15/18      ra      Store ADC scaled value correctly and comment out Jeita status
03/05/18      ra      Changes to HW JEITA
02/22/18      cs      set batt therm coefficient even when not load profile.
02/20/18      ra      Removing obsolete code
02/09/18      ra      Remove Wipower related code
01/15/18      ra      Use ADC to scale the temperature values
11/09/17      ra      Remove legacy cable detection WA
09/19/17      cs      for SDM855 remove getPowerPath, this is not used.
07/12/17      cs      display new icon for debug board cases
07/07/17      cs      GetBatteryType cannot be called before FGInit causing batt missing mistrigger
06/21/17      dc      Move Platform Check to QcomChargerDxe and support config override
06/13/17      ll      Disable HW AFP when debug board is detected
06/05/17      cs      pmic chargerapp need to do USBInit before usb enumeration happen.
03/30/17      ll      Implemented level file-logging
03/21/17      dch     Initialize ICL for all charger as 500mA and increase it to 1500mA in case charger is not SDP/Float charger
03/07/17      cs      remove duplicate get errors checking emergency shutdown condition.
03/09/17      ll      Updated code based on latest chargerlib_cfgdata_type
02/15/17      va      Disable HVDCP (QC 2 and 3.0) when weak battery charging and re run APSD for 2.0 and 3.0
02/06/17      cs      change emergency shutdown Count to 9 to avoid early emmergency shutdown.
                      check destination size bigger or equal to source size for CopyMemS
02/06/17      cs      for emergency shutdown and unknown battery shutdown change from disable PON1 to edge trigger.
01/30/17      sm      Allow device to boot when vbatt is high enough, and batt temp is in operational range
01/27/17      ai      Fix use of retail
01/23/17      sm      CFG parameter Structure changes
12/19/16      sm      Updated for SCHG and FG Protocol APIs' usage
11/14/16      sm      Added changes in ChargerLib_ForceSysShutdown() for preventing PMi8998 to auto power
                      on upon shutdown with Type-A to C cable attached
11/09/16      cs      added API to check if DCIn is valid
11/08/16      va      addded debug board boot up case and make sure to disable wdog during exit
10/19/16      sm      Removed the 2 seconds debounce from battery presence check
10/14/16      va      adding threshold soc check when charger is disconnected
10/13/16      cs      add protocol API for setting dcin current limit.
10/12/16      sv      Fixed Array 'FormatString' KW issue. CR:1063098
10/06/16      cs      suspend DCIN for debug board
09/29/16      va      update to print function names %a and handle %r format specifier for file logging
09/29/16      sm      Changed error action for battery missing and tsense thermal timrout errors
09/20/16      sm      Added changes to handle unknown battery error type based on configuration
09/15/16      sm      Added another action type for debug board case
09/13/16      sm      Removed GetIUsbMaxFromCfg() and  GetMaxUsbCurrent()
08/05/16      va      Adding Charger led indication support
07/28/16      sm      Changed logic in ChargerLib_WasChargerReinserted() and ChargerLib_Exit()
07/26/16      va      Restarting FG Changes on warm boot
07/18/16      sm      Added changes for setting higher current for USB 3.0
07/15/16      cs      Adding Wipower support
07/07/16      va      DEBUG print to print time stamp in seconds
06/29/16      va      Enable charger configs first to enables logs during Initilization of charger app
06/24/16      sm      Added ChargerLib_GetChargingStatus() API
06/23/16      va      Adding support for Charger Fg Peripheral dumps
06/14/16      va      Adding Thermal and File log support to Charger App
05/26/16      va      Update for Profile load and struct mis match
05/24/16      sm      Corrected ADC channel in GetBatteryId() API
05/10/16      sm      Added back ChargerLib_Exit()
04/26/16      va      Volume Error handling for file Log
04/21/16      sm      Added ChargerLib_EnableWdog
04/12/16      sm      Added ChargerLib_GetBatteryID() API and changes to handle
                      debug board
03/30/16      va      update global file logging flag
03/28/16      va      update for file logging, exit api for ChargerLib
03/22/16      va      Update to HW jeita
02/19/16      sm      Commented out reference to any protocol APIs till design
                      is finalised
01/27/16      sm      Initial revision
=============================================================================*/

/*===========================================================================*/
/*                  INCLUDE FILES                                            */
/*===========================================================================*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>
#include "Library/SerialPortShLib.h"
#include <Library/QcomTargetLib.h>
#include <api/pmic/pm/pm_uefi.h>
#include <api/pmic/charger/ChargerLibCommon.h>
#include "ChargerLibTarget.h"

#include <Library/QcomBaseLib.h>
#include <PrintShLib.h>
#include <Library/UefiCfgLib.h>

#include <Library/BaseMemoryLib.h>
#include "string.h"


/**
  Protocol Dependencies
*/
#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/EFIVariableServices.h>
#include <Protocol/EFITsens.h>
#include <Protocol/EFIPmicRTC.h>


/*===========================================================================*/
/*                  TYPEDEF AND MACRO DEFINITIONS                            */
/*===========================================================================*/
#define BATTERY_EMULATOR_UPPER_THRSHOLD_MV  4450
#define BATTERY_EMULATOR_LOWER_THRSHOLD_MV  3450
#define BATTERY_EMULATOR_BATT_ID            1500
#define BATTERY_EMULATOR_TEMP_C             23
#define BATTERY_EMULATOR_SOC                99

#define VBATT_TOLERANCE                     5 //Battery upper voltage tolerance limit.

#define UNKNOWN_BATT_SHUTDOWN               0
#define UNKNOWN_BATT_BOOT_TO_HLOS           1
#define UNKNOWN_BATT_CONSERVATIVE_CHARGING  2
#define UNKNOWN_BATT_REGULAR_CHARGING       3

#define DEBUG_BOARD_LOWVBAT_SHUTDOWN        0
#define DEBUG_BOARD_LOWVBAT_NOCHARGE        1
#define DEBUG_BOARD_BOOT_TO_HLOS            2

#define CHARGER_COOL_OFF_PERIOD_DEFAULT     300000 //5minutes=1000millisec*60*5=300000 milli sec
#define BOOT_THRESHOLD_VOLT                 3600

#define TWO_SECONDS                         2000000 /*2seconds = 2000000 uSec*/

#define FIVE_SEC_IN_MILLI_SEC               5000
#define TIMEOUT_WAIT_FOR_KEY                0x01

#define MAX_3000_MA                         3000
#define MAX_1500_MA                         1500

#define THERMAL_TIMEOUT_NOTSET              0
#define MIN_TO_SEC                          60

#define MAX_THERMAL_WAIT_PERIOD_IN_MINS     60

#define USB_3_IUSB_MAX                      900 //IUSB_MAX for USB 3.0 port
#define USB_2_IUSB_MAX                      500 //IUSB_MAX for USB 2.0 port
#define FCC_CONSERVE_CHG                    500
#define USB_ENUM_WAIT_DURATION              500 /*500 milliseconds*/

#define MAX_EMERGENCY_SHUTDOWN_COUNT        3

#define THERMAL_TIMEOUT_NOTSET              0
#define MIN_TO_SEC                          60

#define MAX_THERMAL_WAIT_PERIOD_IN_MINS     60

#define CHARGERLIB_SHIPMODE_VARIABLE_NAME   L"ShipMode"

/*===========================================================================*/
/*                  FUNCTIONS PROTOTYPES                                     */
/*===========================================================================*/
EFI_STATUS ChargerLib_SetFccMaxCurrent(UINT32 FccMaxCurrent);
EFI_STATUS ChargerLib_SetFvMaxVoltage(UINT32 VddMaxVoltage);

VOID EFIAPI ChargerLibEnableCallback(IN EFI_EVENT Event, IN VOID *Context);
VOID EFIAPI ChargerLibDisableCallback(IN EFI_EVENT Event, IN VOID *Context);
EFI_STATUS ChargerLib_GetTSensTemp( INT32 *TSensTemp);
EFI_STATUS ChargerLib_HandleThermalErrorType(ChargerLibThermalStatus ThermalStatus, CHARGERLIB_CHARGING_ERROR_TYPES *pChargingError);
EFI_STATUS ChargerLib_GetSEllapsedFromBoot(UINT32 *PmicTimeinS);
EFI_STATUS ChargerLib_GetADCReading(CONST CHAR8 *pszInputName, UINT32 uInputNameSize, EfiAdcResultType *pEfiAdcResult);
EFI_STATUS ChargerLib_PrintDCInStatus();
EFI_STATUS ChargerLib_FlushShipMdeVar(UINT16* variable, UINT8 Value);
EFI_STATUS ChargerLib_GetBatteryType(ChgBattType *pBatteryType);


/*===========================================================================*/
/*                  LOCAL VARIABLE DECLARATIONS                              */
/*===========================================================================*/

chargerlib_cfgdata_type                 gChargerLibCfgData = {0};

STATIC ULogHandle                     gULogHandle = NULL;

STATIC UINT32                          ThermalTimeOutTimeStamp = 0;
STATIC BOOLEAN                         bDbgBoard = FALSE;

/*===========================================================================*/
/*                 FUNCTION DEFINITIONS                                      */
/*===========================================================================*/
/**
  Initialize ChargerDXE Library functions

  @param none

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_INVALID_PARAMETER: A Parameter was incorrect.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_NOT_READY:         The physical device is busy or not ready to
                         process this request.
*/
EFI_STATUS ChargerLibCommon_Init( chargerlib_cfgdata_type *pChargerLibConfig )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!pChargerLibConfig)
  {
    return EFI_INVALID_PARAMETER;
  }

  CopyMemS(&gChargerLibCfgData, sizeof(chargerlib_cfgdata_type), pChargerLibConfig , sizeof(chargerlib_cfgdata_type));

  Status = ChargerLibTarget_Init(pChargerLibConfig);

  return Status;
}

/**
  Get Battery Status Information
  @param[out] BattStatusInfo  refer EFI_PM_FG_BATT_STATUS_INFO
  This function gets the battery status, battery SOC, Charge current, battery voltage
  and battery temperature from Fuel Gauge

  @param[out] BatteryStatus  see chargerlib_batt_status_info for more details

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_GetBatteryStatus(chargerlib_batt_status_info *pBatteryStatus)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(NULL == pBatteryStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = ChargerLibTarget_GetBatteryStatus(pBatteryStatus);

  return Status;
}

/**
  Get Battery Status Information
  @param[out] BattStatusInfo  refer EFI_PM_FG_BATT_STATUS_INFO
  This function gets the battery status, battery SOC, Charge current, battery voltage
  and battery temperature from Fuel Gauge

  @param[out] BatteryStatus  see chargerlib_batt_status_info for more details

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_GetBatteryProfileStatus(chargerlib_batt_profile_status *ProfileSts)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!ProfileSts)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = ChargerLibTarget_GetBatteryProfileStatus(ProfileSts);

  return Status;
}

EFI_STATUS ChargerLib_LoadProfile(OPTIONAL chargerlib_batt_profile_data *pBattProfileInfo)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_LoadProfile(pBattProfileInfo);

  return Status;
}

/**
  Enable charger

  @param[in] Enable  TRUE  - Enable Charging
                     FALSE - Disable Charging

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_ChargerEnable(BOOLEAN Enable)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_ChargerEnable(Enable);

  return Status;
}

/**
  Set charger max current

  @param[in] MaxCurrent  Max current for charger in mA

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_SetMaxUsbCurrent(UINT32 MaxCurrent)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_SetMaxUsbCurrent(MaxCurrent);

  return Status;
}

/**
  Set Battery FCC Max current

  @param[in] FccMaxCurrent  Max current for charger in mA

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_SetFccMaxCurrent(UINT32 FccMaxCurrent)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_SetFccMaxCurrent(FccMaxCurrent);

  return Status;
}

EFI_STATUS ChargerLib_WasChargerReinserted(BOOLEAN *pChargerReinserted)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_WasChargerReinserted(pChargerReinserted);

  return Status;
}

EFI_STATUS ChargerLib_ReRunAicl( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_ReRunAicl();

  return Status;
}

/**
  Set Battery Floating Max Voltage

  @param[in] VddMaxVoltage  Max Voltage for Charger in mV

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_SetFvMaxVoltage(UINT32 VddMaxVoltage)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_SetFvMaxVoltage( VddMaxVoltage);

  return Status;
}

/**
  Get Charger/Power Path

  @param[out] Powerpath

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_INVALID_PARAMETER: Parameter is invalid.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
//EFI_STATUS ChargerLib_GetPowerPath(CHARGERLIB_ATTACHED_CHGR_TYPE *PowerPath)
//{
//  EFI_STATUS Status = EFI_SUCCESS;

//  Status = ChargerLibTarget_GetPowerPath(PowerPath);
//  return Status;
//}

EFI_STATUS ChargerLib_GetChargingPath(CHARGERLIB_ATTACHED_CHGR_TYPE *ChargingPath)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!ChargingPath)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = ChargerLibTarget_GetChargingPath(ChargingPath);

  return Status;
}

/**
  Get Battery Presence

  @param[out] BatteryPresence

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_INVALID_PARAMETER: Parameter is invalid.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_GetBatteryPresence(BOOLEAN *BatteryPresence)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!BatteryPresence)
    return EFI_INVALID_PARAMETER;

  Status = ChargerLibTarget_GetBatteryPresence(BatteryPresence);

  return Status;
}

/**
ChargerLib_PrintDebugMsg()

@brief
Returns ChargerLib_PrintDebugMsg Flag Status
*/
inline BOOLEAN
ChargerLib_PrintDebugMsg(void)
{
  return ((BOOLEAN)gChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg);
}

/**
ChargerLib_PrintDebugMsgToFile()

@brief
Returns ChargerLib_PrintDebugMsgToFile Flag Status
*/
inline BOOLEAN
ChargerLib_PrintDebugMsgToFile(void)
{
  if (NULL != gULogHandle) return (TRUE);
  return FALSE;
}

/**
ULogPrint ()

@brief Helper API to print multiple arguments
Returns
*/
inline void EFIAPI
ULogPrint(
  IN  UINTN        LogLevel,
  IN  CONST CHAR8  *Format,
  ...
  )
{
  va_list vlist;
  UINT32  dataCount = 0;
  UINTN   i         = 0;
  BOOLEAN  boEsc = FALSE;
  STATIC EFI_STATUS lastLogResult = EFI_SUCCESS;
  EFI_STATUS CurrentLogResult     = EFI_SUCCESS;
  CHAR8 CurrentFormat[512];
  UINTN CurrentFormatIndex = 0;
  UINT32 size = sizeof(CurrentFormat);
  BOOLEAN appendFlag = FALSE;
  CHAR8 *FnName = NULL;

  if ((LogLevel & gChargerLibCfgData.dbg_cfg_Data.file_log_level_mask) != 0)
  {
      SetMem(CurrentFormat, sizeof(CurrentFormat), 0x00);

      /* If volume corrupted do not attempt to log */
      if (EFI_VOLUME_CORRUPTED == lastLogResult)
          return;

      for (i = 0, dataCount = 0; (NULL != (Format + i)) && (Format[i] != '\0'); i++, CurrentFormatIndex++)
      {
          CurrentFormat[CurrentFormatIndex] = Format[i];
          if (Format[i] == '%' && !boEsc)
          {
              if (Format[i + 1] == 'a')
              {
                  appendFlag = TRUE;
                  va_start(vlist, Format);
                  FnName = (CHAR8*)va_arg(vlist, CHAR8*);
                  AsciiVSPrint((CurrentFormat + CurrentFormatIndex), size, FnName, vlist);
                  CurrentFormatIndex = CurrentFormatIndex + AsciiStrLen(FnName);
                  CurrentFormat[CurrentFormatIndex] = ' ';
                  i += 2;
              }
              else if (Format[i + 1] == 'r')
              {
                  CurrentFormat[++CurrentFormatIndex] = 'd';
                  i++;
                  dataCount++;
              }
              else
              {
                  dataCount++;
              }
          }
          else if (Format[i] == '\\' && !boEsc)
          {
              boEsc = TRUE;
          }
          else if (boEsc)
          {
              boEsc = FALSE;
          }
      }
      /*
      if(appendFlag)
      {
      va_end(vlist);
      }*/
      /* Append end of the string */
      CurrentFormat[CurrentFormatIndex] = '\0';

      //CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a dataCount = %d \r\n",__FUNCTION__, dataCount));

      if (appendFlag == FALSE)
      {
          va_start(vlist, Format);
      }
      //CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a formatstring = %a \r\n",__FUNCTION__, Format));
      //CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a Current Fn Name = %a size = %d \r\n",__FUNCTION__, CurrentFormat, size));
      if (appendFlag)
      {
          CurrentLogResult = (EFI_STATUS)ULogFront_RealTimeVprintf(gULogHandle, dataCount, CurrentFormat, vlist);
      }
      else
      {
          CurrentLogResult = (EFI_STATUS)ULogFront_RealTimeVprintf(gULogHandle, dataCount, Format, vlist);
      }
      if (CurrentLogResult == EFI_VOLUME_CORRUPTED)
      {
          CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a File Log Print Volume Error = %r \r\n", __FUNCTION__, CurrentLogResult));
          lastLogResult = CurrentLogResult;
      }
      else if (CurrentLogResult != EFI_SUCCESS)
      {
          CHARGER_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a File Log Print Error = %r \r\n", __FUNCTION__, CurrentLogResult));
      }
      else { /* SUCCESS use case */ }

      va_end(vlist);
  }
}


/**
ChargerLib_InitFileLog(BOOLEAN )

@brief  Init File Logging in ULog
Returns Status
*/
EFI_STATUS ChargerLib_InitFileLog(BOOLEAN PrintChgAppDbgMsgToFile)
{
  ULogResult result = 0;
  EFI_STATUS       Status     =  EFI_SUCCESS;
  UINT32           VarSize    =  1;
  BOOLEAN          FileLoggingIsEnabled = TRUE;
  ULOG_CONFIG_TYPE  configType = {0};
  UINT32            Value      = 0;
  UINTN             MaxLogFileSize = DEFAULT_LOGFILE_SIZE; // Target-specific maximum log size

  DEBUG(( EFI_D_WARN, "ChargerLib:: %a \r\n", __FUNCTION__));

  if (!RETAIL)
  {
    /* Enable default file logging if production mode is disabled */
    if( TRUE == PrintChgAppDbgMsgToFile )
    {
      Status = gRT->SetVariable (L"EnableFileLogging", &gQcomTokenSpaceGuid,
                                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                                 VarSize, &FileLoggingIsEnabled);
      if (EFI_SUCCESS != Status)
      {
        DEBUG(( EFI_D_WARN, "ChargerLib:: %a Global File Logging BDS Menu Variable read error = ?%d \r\n", __FUNCTION__, Status));
        return EFI_SUCCESS;
      }

      Status = GetConfigValue("MaxLogFileSize", &Value);
      if ((Status == EFI_SUCCESS) && (Value > 0)) {
        DEBUG((EFI_D_WARN, "ChargerLib:: %a InitFileLog SUCCESS MaxLogFileSize = (%x)\r\n",  __FUNCTION__, Value));
        MaxLogFileSize = Value;
      } else {
        MaxLogFileSize = DEFAULT_LOGFILE_SIZE;
      }

      result = ULogFile_Open(&gULogHandle, LOGFILE_IN_EFS, MaxLogFileSize);
      if (0 == result)
      {
        /* Set ULog configuration */
        ULogFile_GetConfig(&gULogHandle, &configType);
        configType.separator = ',';
        configType.PrintTimestamp = TRUE;
        configType.TrimNewline = TRUE;
        ULogFile_SetConfig(&gULogHandle, &configType);
        if (Status != EFI_SUCCESS){
          CHARGER_UART_DEBUG(( EFI_D_ERROR, "ChargerLib:: ULogFile_SetConfig FAILED \r\n"));
        }
        ULOG_RT_PRINTF_1(gULogHandle, "ChargerLib::InitFileLog SUCCESS gULogHandle = %lld", gULogHandle);
        DEBUG((EFI_D_WARN, "ChargerLib:: %a InitFileLog SUCCESS \r\n",  __FUNCTION__));

        /* Print Battery status information Log headers during charging loop */
        /* DEBUG((EFI_D_WARN, "RebootCount,TimeStamp,StateOfCharge,RatedCapacity,Voltage,ChargeCurrent,Temp \r\n"));
        DEBUG((EFI_D_WARN, "TimeStamp,StateOfCharge,RatedCapacity,Voltage,ChargeCurrent,Temp \r\n")); */
      }
      else
      {
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a InitFileLog FAILED \r\n",  __FUNCTION__));
      }
    }
  }

  return EFI_SUCCESS;
}


/**
ChargerLib_GetFileLogHandle(BOOLEAN )

@brief  Returns File long handle if alreayd initilized
Returns Status
*/
EFI_STATUS ChargerLib_GetLogInfo(chargerlib_loginfo *gFileHandle )
{
  ULogResult        result     = 0;
  ULOG_CONFIG_TYPE  configType = {0};
  UINT32            Value      = 0;
  UINTN             MaxLogFileSize = DEFAULT_LOGFILE_SIZE; // Target-specific maximum log size
  EFI_STATUS        Status     = EFI_SUCCESS;

  if(!gFileHandle)
    return EFI_INVALID_PARAMETER;

  if(NULL != gULogHandle)
  {
    Status = GetConfigValue("MaxLogFileSize", &Value);
    if ((Status == EFI_SUCCESS) && (Value > 0)) {
      MaxLogFileSize = Value;
    } else {
      MaxLogFileSize = DEFAULT_LOGFILE_SIZE;
    }
    result = ULogFile_Open(&(gFileHandle->gChargerLogHandle), LOGFILE_IN_EFS, MaxLogFileSize);
    if (0 == result)
    {
      /* Set ULog configuration */
      ULogFile_GetConfig(&(gFileHandle->gChargerLogHandle), &configType);
      configType.separator = ',';
      configType.PrintTimestamp = TRUE;
      configType.TrimNewline = TRUE;
      ULogFile_SetConfig(&gFileHandle->gChargerLogHandle, &configType);
      ULOG_RT_PRINTF_1(gFileHandle->gChargerLogHandle, "ChargerLib::InitFileLog SUCCESS gULogHandle = %lld", gFileHandle->gChargerLogHandle);
      DEBUG((EFI_D_WARN, "ChargerLib:: %a InitFileLog SUCCESS \r\n",  __FUNCTION__));
    }
    else
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a InitFileLog FAILED \r\n",  __FUNCTION__));
    }
  }
  gFileHandle->print_charger_app_dbg_msg = gChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg;
  gFileHandle->print_charger_app_dbg_msg_to_file = gChargerLibCfgData.dbg_cfg_Data.print_charger_app_dbg_msg_to_file;

   return Status;
}

/**
  Gets charger port type

  @param[out] pPortType  Charger port type

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_GetPortType(CHARGERLIB_CHGR_PORT_TYPE *pPortType)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status  = ChargerLibTarget_GetPortType(pPortType);

  return Status;
}


EFI_STATUS ChargerLib_HandleNoChargeAndWait(void)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (TRUE == gChargerLibCfgData.no_charge_and_wait)
  {
    /*Disable Charging */
    Status = ChargerLibTarget_ChargerEnable(FALSE);
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Charging Disabled \r\n", __FUNCTION__));
  }
  else
  {
    ChargerLib_ForceSysShutdown(CHGAPP_RESET_AFP);
  }

  return Status;
}

EFI_STATUS ChargerLib_EnableAfpMode(void)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_EnableAfpMode();

  return Status;
}

EFI_STATUS ChargerLib_GetBatteryID(INT32 *pBatteryID)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(NULL == pBatteryID)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pBatteryID = 0;

  Status = ChargerLibTarget_GetBatteryID(pBatteryID);

  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a BATT_ID_2 = %d \r\n", __FUNCTION__, *pBatteryID));

  return Status;
}

EFI_STATUS ChargerLib_GetBatteryType(ChgBattType *pBatteryType)
{
  EFI_STATUS Status = EFI_SUCCESS;
  INT32      battery_id = 0;

  if(NULL == pBatteryType)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pBatteryType = CHG_BATT_TYPE_INVALID;

  Status = ChargerLib_GetBatteryID(&battery_id);
  if(EFI_SUCCESS != Status)
  {
    return Status;
  }

  /*Check if battery ID is within real battery range*/
  if((battery_id >= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.SmartBatteryIdMin) &&
     (battery_id <= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.SmartBatteryIdMax))
   {
     *pBatteryType = CHG_BATT_TYPE_SMART;
   }
   else if((battery_id >= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.RegularBatteryIdMin) &&
     (battery_id <= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.RegularBatteryIdMax))
   {
     *pBatteryType = CHG_BATT_TYPE_NORMAL;
   }
   else if((battery_id >= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.DebugBoardBatteryIdMin) &&
     (battery_id <= gChargerLibCfgData.fg_cfg_data.FgBattIdCfg.DebugBoardBatteryIdMax))
   {
     *pBatteryType = CHG_BATT_TYPE_DEBUG_BOARD;
   }
   else
   {
     *pBatteryType = CHG_BATT_TYPE_UNKNOWN;
   }

   CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Battery type = %d \r\n", __FUNCTION__, *pBatteryType));

  return Status;
}

EFI_STATUS ChargerLib_UsbSuspend(BOOLEAN Enable)
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib UsbSuspend = %d \r\n", __FUNCTION__, Enable));

  Status = ChargerLibTarget_UsbSuspend(Enable);

  return Status;
}

EFI_STATUS ChargerLib_DcinSuspend(BOOLEAN Suspend)
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib DcinSuspend = %d \r\n", __FUNCTION__, Suspend));

  Status = ChargerLibTarget_DcinSuspend(Suspend);

  return Status;
}

EFI_STATUS ChargerLib_DcinSetPowerLimit(UINT32 PowerInMicroW)
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib powerInMicroW = %d w\r\n", __FUNCTION__, PowerInMicroW));

  Status = ChargerLibTarget_DcinSetPowerLimit(PowerInMicroW);

  return Status;
}

EFI_STATUS ChargerLib_GetDebugBoardAction(CHARGERLIB_ERROR_ACTION_TYPE *pDebugBoardAction)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!pDebugBoardAction)
  {
    return EFI_INVALID_PARAMETER;
  }
  Status = ChargerLibTarget_GetDebugBoardAction(pDebugBoardAction);

  return Status;
}


EFI_STATUS ChargerLib_ConservativeCharging( void )
{
  EFI_STATUS Status = EFI_SUCCESS;

  /*set conservative parameter */
  Status = ChargerLib_SetFvMaxVoltage(gChargerLibCfgData.schg_cfg_data.ChgFvMax - gChargerLibCfgData.conserv_chg_fv_delta);
  Status |= ChargerLib_SetFccMaxCurrent(FCC_CONSERVE_CHG);

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Disabling charging \r\n", __FUNCTION__));
  Status |= ChargerLib_ChargerEnable(FALSE);

  return Status;
}

EFI_STATUS ChargerLib_GetUnknownBatteryAction(CHARGERLIB_ERROR_ACTION_TYPE *pUnknownBatteryAction)
{
  EFI_STATUS Status = EFI_SUCCESS;
  if (!pUnknownBatteryAction)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = ChargerLibTarget_GetUnknownBatteryAction(pUnknownBatteryAction);

  return Status;
}

/*This API initiates different types of reset */
EFI_STATUS ChargerLib_ForceSysShutdown(ChgAppSysShutdownType ShutdownType)
{
  EFI_STATUS Status = EFI_SUCCESS;

  /*Disable charging*/
  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a  Disable Charging - Shutdown Type = %d \r\n", __FUNCTION__, ShutdownType));
  ChargerLib_ChargerEnable(FALSE);
  /* Turn Off Charging Led */
  if(gChargerLibCfgData.charger_led_config)
  {
    /* Turn Off Charging */
    ChargerLib_LedOn(FALSE);
  }

  if (!RETAIL)
  {
    /* Flush serial buffer in production mode */
    SerialPortFlush();
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a  SerialPortFlush - Shutdown Type = %d \r\n", __FUNCTION__, ShutdownType));
    SerialPortFlush();
  }

  /* Clean up */
  Status = ChargerLib_Exit();

  Status = ChargerLibTarget_ForceSysShutdown(ShutdownType);


  return Status;
}

UINT64 ChargerLib_TimeGet64(void)
{
  UINT64 Result = 0 ;
  Result = GetPerformanceCounter();
  return(Result);
}

UINT64 ChargerLib_CalculateTime()
{
  UINTN  CpuFrequency = 0;
  UINT64 TotalTicks = 0;
  UINT64 Seconds =0;

  /* Debug Print CPU Frequncey */
  TotalTicks   = ChargerLib_TimeGet64();
  /* Debug Print CPU Frequncey */
  CpuFrequency = GetPerformanceCounterProperties(NULL, NULL);
  Seconds = MultU64x32(TotalTicks, 1000000);
  Seconds = DivU64x32(Seconds, CpuFrequency);
  Seconds /= 1000000;

  return Seconds;
}

UINT64  ChargerLib_CalculateTimeMSec()
{
  UINTN  CpuFrequency = 0;
  UINT64 TotalTicks = 0;
  UINT64 Seconds =0;

  /* Debug Print CPU Frequncey */
  TotalTicks   = ChargerLib_TimeGet64();
  /* Debug Print CPU Frequncey */
  CpuFrequency = GetPerformanceCounterProperties(NULL, NULL);
  Seconds = MultU64x32(TotalTicks, 1000000);
  Seconds = DivU64x32(Seconds, CpuFrequency);
  Seconds /= 1000;

  return Seconds;
}

EFI_STATUS ChargerLib_GetThermalStatus( ChargerLibThermalStatus *ThermalStatus)
{
  EFI_STATUS Status = EFI_SUCCESS;
  INT32 CurrTemp = 0;
  STATIC ChargerLibThermalStatus ThermalSts = CHARGERLIB_THERMAL_INIT; /* Start with Init */

  if(!ThermalStatus)
    return EFI_INVALID_PARAMETER;

  Status = ChargerLibTarget_GetTSensTemp(&CurrTemp);

  /*  Handle if any Error */
  if(EFI_ERROR(Status))
    return Status;

  switch(ThermalSts)
  {
    case CHARGERLIB_THERMAL_INIT:
    case CHARGERLIB_THERMAL_OK:
      if (CurrTemp >= gChargerLibCfgData.thermal_configs.tsens_extreme_Temp)
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_CRITICAL;/*Updated output params*/
      }
      else if (CurrTemp >= gChargerLibCfgData.thermal_configs.tsens_high_temp )
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_HIGH;
      }
      else
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_OK;
      }
    break;

    case CHARGERLIB_THERMAL_HIGH:
      if (CurrTemp >= gChargerLibCfgData.thermal_configs.tsens_extreme_Temp)
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_CRITICAL;/*Updated output params*/
      }
      else if ((CurrTemp > gChargerLibCfgData.thermal_configs.tsens_low_temp) && (CurrTemp < gChargerLibCfgData.thermal_configs.tsens_extreme_Temp))
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_HIGH;
      }
      else if (CurrTemp <= gChargerLibCfgData.thermal_configs.tsens_low_temp)
      {
        *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_OK;
      }
      else
      {
        CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a Error = %d ThermalSts = %d \r\n", __FUNCTION__, CurrTemp, ThermalSts));
      }
    break;

    case CHARGERLIB_THERMAL_CRITICAL:
      /*should not hit this use case as it is already hanlded by then */
      *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_CRITICAL;/*Updated output params*/
    break;

    case CHARGERLIB_THERMAL_HIGH_TIMEOUT:
      /*should not hit this use case as it is already hanlded by then */
      *ThermalStatus = ThermalSts = CHARGERLIB_THERMAL_HIGH_TIMEOUT;/*Updated output params*/
    break;

    default:
    break;
  }

  CHARGER_DEBUG((EFI_D_WARN,"ChargerLib:: %a CurrTsensTemp = %d ThermalStatus = %d\r\n", __FUNCTION__, CurrTemp, *ThermalStatus));

  return Status;
}


EFI_STATUS ChargerLib_DumpSram(BOOLEAN bDumpExplicit)
{
  EFI_STATUS    Status = EFI_SUCCESS;

  Status = ChargerLibTarget_DumpSram(bDumpExplicit);

  return Status;
}

EFI_STATUS ChargerLib_DumpChargerPeripheral()
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_DumpChargerPeripheral();

  return Status;
}

EFI_STATUS ChargerLib_GetHwJeitaStatus(CHARGERLIB_HW_JEITA_STATUS *pHwJeitaStatus)
{
  EFI_STATUS  Status = EFI_SUCCESS;

  if (!pHwJeitaStatus)
    return EFI_INVALID_PARAMETER;

  Status = ChargerLibTarget_GetHwJeitaStatus(pHwJeitaStatus);

  return Status;
}

EFI_STATUS ChargerLib_GetBatteryTempStatus(CHARGERLIB_BATT_TEMP_STATUS *pBattTempStatus)
{
  //Battery temperature outside of operational range, extreme cold or hot condition (below -15 , above 70 degree, configurable) (AFP) -- BatteryTemperatureOutOfRangeError
  EFI_STATUS Status      = EFI_SUCCESS;
  CHARGERLIB_HW_JEITA_STATUS   HwJeitaStatus = {0};
  chargerlib_batt_status_info  BatteryStatus = {0};

  if (!pBattTempStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pBattTempStatus = ChargerLib_Batt_Temp_Normal;

  Status = ChargerLib_GetBatteryStatus(&BatteryStatus);
  if(EFI_SUCCESS != Status )
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a Error getting battery status = %r \r\n", __FUNCTION__, Status));
    return Status;
  }
  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a BatteryTemp = %d C \r\n", __FUNCTION__, BatteryStatus.BatteryTemperature));

  Status = ChargerLib_GetHwJeitaStatus( &HwJeitaStatus );
  if((TRUE == HwJeitaStatus.JeitaHardCold) || (TRUE == HwJeitaStatus.JeitaHardHot))
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a BatteryTemp out-of-range = %d C \r\n",__FUNCTION__, BatteryStatus.BatteryTemperature));
    *pBattTempStatus = ChargerLib_Batt_Temp_OutsideChargingRange;
  }

  if(( BatteryStatus.BatteryTemperature < gChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempLowLimit) ||
     ( BatteryStatus.BatteryTemperature > gChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempHighLimit))
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a BatteryTemp = %d C, Operational Range lower limit = %d, upper limit = %d \r\n",
                __FUNCTION__, BatteryStatus.BatteryTemperature, gChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempLowLimit,
                  gChargerLibCfgData.fg_cfg_data.HwJeitaThreshold.JeitaCriticalTempHighLimit));

    *pBattTempStatus = ChargerLib_Batt_Temp_OutsideOperationalRange;
  }

  if(EFI_SUCCESS != Status)
  {
    *pBattTempStatus = ChargerLib_Batt_Temp_Invalid;
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib::%a BatteryTemp invalid = %r \r\n",__FUNCTION__, Status));
  }

  return Status;
}

EFI_STATUS ChargerLib_GetBatteryVoltageStatus(CHARGERLIB_VBATT_STATUS *pBatteryVoltageStatus)
{
  EFI_STATUS                   Status      = EFI_SUCCESS;
  STATIC UINT32                EmergencyShutdownCounter = 0;
  chargerlib_batt_status_info  BatteryStatus = {0};

  if (!pBatteryVoltageStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pBatteryVoltageStatus = ChargerLib_VBatt_Normal;

  Status = ChargerLib_GetBatteryStatus(&BatteryStatus);
  if( EFI_SUCCESS != Status )
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error getting battery status = %r \r\n", __FUNCTION__, Status));
    return Status;
  }

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Battery voltage = %d mV \r\n", __FUNCTION__, BatteryStatus.BatteryVoltage));

  /* Return status if vbatt is good */
  if ((UINT32)BatteryStatus.BatteryVoltage > (gChargerLibCfgData.schg_cfg_data.ChgFvMax + gChargerLibCfgData.batt_volt_lim_high_delta))
  {
    EmergencyShutdownCounter = 0;

    Status = ChargerLib_GetBatteryStatus(&BatteryStatus);
    if( EFI_SUCCESS != Status )
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error getting battery status = %r \r\n", __FUNCTION__, Status));
      return Status;
    }

    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Current Battery voltage =  %d mV \r\n", __FUNCTION__, BatteryStatus.BatteryVoltage));

    /* Check for High Voltage Limit */
    if (((UINT32)(BatteryStatus.BatteryVoltage - VBATT_TOLERANCE)) > (gChargerLibCfgData.schg_cfg_data.ChgFvMax + gChargerLibCfgData.batt_volt_lim_high_delta))
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Current Battery voltage too high %d mV, Threshold = %d mV  \r\n", __FUNCTION__,
                                BatteryStatus.BatteryVoltage, (gChargerLibCfgData.schg_cfg_data.ChgFvMax + gChargerLibCfgData.batt_volt_lim_high_delta)));
      *pBatteryVoltageStatus = ChargerLib_VBatt_TooHigh;
    }
  }
  /* Check for vbatt below emergency shutdown limit */
  else if ((UINT32)BatteryStatus.BatteryVoltage < gChargerLibCfgData.emergency_shutdown_vbatt)
  {
    if(BatteryStatus.ChargeCurrent > 0) /* Battery is discharging */
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Current BattVoltage = %d mV is below EmergencyShutdownVbatt = %d mV ", __FUNCTION__,
                    BatteryStatus.BatteryVoltage, gChargerLibCfgData.emergency_shutdown_vbatt));
    CHARGER_DEBUG((EFI_D_WARN, " EmergencyShutdownCounter = %d \r\n", EmergencyShutdownCounter));
      if(EmergencyShutdownCounter >= MAX_EMERGENCY_SHUTDOWN_COUNT)
      {
        *pBatteryVoltageStatus = ChargerLib_VBatt_BelowThreshold;
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a EmergencyShutdownCounter Exceeded : \r\n", __FUNCTION__ ));
    CHARGER_DEBUG((EFI_D_WARN, " Curr Battery voltage = %d mV is below threshold = %d mV \r\n",
          BatteryStatus.BatteryVoltage, gChargerLibCfgData.emergency_shutdown_vbatt ));
      }
      else
      {
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a EmergencyShutdownCounter = %d \r\n", __FUNCTION__, EmergencyShutdownCounter));
      }
      EmergencyShutdownCounter++;
    }
    else
    {
      EmergencyShutdownCounter = 0;
    }
  }
  else
  {
    EmergencyShutdownCounter = 0;
  }

  return Status;
}

EFI_STATUS ChargerLib_GetErrors(BOOLEAN vbattChecking, CHARGERLIB_CHARGING_ERROR_TYPES  *pChargingError)
{
  EFI_STATUS                        Status            = EFI_SUCCESS;
  ChargerLibThermalStatus           ThermalStatus     = CHARGERLIB_THERMAL_STATUS_MAX;
  CHARGERLIB_BATT_TEMP_STATUS       BattTempStatus    = ChargerLib_Batt_Temp_Normal;
  CHARGERLIB_VBATT_STATUS           BattVoltageStatus = ChargerLib_VBatt_Normal;
  CHARGERLIB_ATTACHED_CHGR_TYPE     AttachedCharger   = CHARGERLIB_ATTACHED_CHGR__NONE;

  BOOLEAN     BatteryPresent = FALSE;
  ChgBattType BatteryType = CHG_BATT_TYPE_INVALID;

  STATIC BOOLEAN FirstTimeUnkownBattDetected = FALSE;

  if (!pChargingError)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pChargingError = CHARGERLIB_CHARGING_ERROR_NONE;

  /* Check Critical errors first */

  /* Battery Presence Detection */
  Status = ChargerLib_GetBatteryPresence(&BatteryPresent);
  if (!BatteryPresent)
  {
    *pChargingError = CHARGERLIB_CHARGING_ERROR_BATTERY_NOT_DETECTED;
    CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR Battery not detected \r\n", __FUNCTION__));
    return Status;//Got Critical error, return
  }

  ChargerLib_GetBatteryType(&BatteryType);
  if(CHG_BATT_TYPE_DEBUG_BOARD == BatteryType)
  {
    *pChargingError = CHARGERLIB_CHARGING_ERROR_DEBUG_BOARD;
    CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a Debug board detected. \r\n", __FUNCTION__));
    return Status; //Not real battery, no need to check other errors
  }
  else if(CHG_BATT_TYPE_UNKNOWN == BatteryType)
  {
    if(FALSE == FirstTimeUnkownBattDetected)
    { /*If this is the first time unknown battery is detected, return error type as
        Unknown Battery so that required initial configurations can be done*/
      *pChargingError = CHARGERLIB_CHARGING_ERROR_UNKNOWN_BATTERY;
      FirstTimeUnkownBattDetected = TRUE;
      CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a Unknown battery detected. \r\n", __FUNCTION__));
      return Status;
    }
    else
    {/*if Unknown battery was detected earlier, return error type as unknown battery
       so that GetAction will return required action, otherwise look for other errors*/
      if(UNKNOWN_BATT_SHUTDOWN == gChargerLibCfgData.unknown_battery_behavior)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_UNKNOWN_BATTERY;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a Unknown battery detected. \r\n", __FUNCTION__));
        return Status;
      }
    }
  }
  else
  {
    //No error to be returned for regular/smart batteries.
  }

  //Check battery temperature related errors
  if(EFI_SUCCESS == ChargerLib_GetBatteryTempStatus(&BattTempStatus))
  {
    if(ChargerLib_Batt_Temp_OutsideOperationalRange == BattTempStatus)
    {
      *pChargingError = CHARGERLIB_CHARGING_ERROR_BATT_TEMP_OUTSIDE_OPERATIONAL_RANGE;
      CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR: Battery temperature out of operational range \r\n", __FUNCTION__));
      return Status; //Got Critical error, return
    }
  }
  else
  {
    *pChargingError = CHARGERLIB_CHARGING_ERROR_FG_ERROR;
    CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR ChargerLib_GetBatteryTempStatus FG Error \r\n", __FUNCTION__));
  }

  if (vbattChecking)
  {
    /* Check Voltage errors*/
    if(EFI_SUCCESS == ChargerLib_GetBatteryVoltageStatus(&BattVoltageStatus))
    {
      if(ChargerLib_VBatt_TooHigh == BattVoltageStatus)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_VBATT_OUTSIDE_RANGE;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR Battery voltage out of range \r\n", __FUNCTION__));
        return Status;
      }
      else if(ChargerLib_VBatt_BelowThreshold == BattVoltageStatus)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_LOW_VBATT;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR Battery voltage below threshold \r\n", __FUNCTION__));
        return Status;
      }
    }
    else
    {
      *pChargingError = CHARGERLIB_CHARGING_ERROR_FG_ERROR;
       CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR FG Error from  ChargerLib_GetBatteryVoltageStatus \r\n", __FUNCTION__));
    }
  }

  /* Charge Source Detection */
  Status = ChargerLib_GetChargingPath(&AttachedCharger);
  if ((CHARGERLIB_ATTACHED_CHGR__BATT == AttachedCharger) || (CHARGERLIB_ATTACHED_CHGR__NONE == AttachedCharger))
  {
    *pChargingError = CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED;
    CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib::%a ERROR Charging source not detected \r\n", __FUNCTION__));
  }

  if(*pChargingError != CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED)
  {
    /* Check for errors that require to stop charging */
    if(ChargerLib_Batt_Temp_OutsideChargingRange == BattTempStatus)
    {
      *pChargingError = CHARGERLIB_CHARGING_ERROR_BATT_TEMP_OUTSIDE_CHARGING_RANGE;
      CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ERROR Battery temperature out of charging range \r\n", __FUNCTION__));
      //return Status;
    }
  }

  /* Check MSM tesens temp if enabled */
  if(TRUE == gChargerLibCfgData.sw_thermal_mitigation_enable)
  {
    Status |= ChargerLib_GetThermalStatus(&ThermalStatus);
    if (EFI_SUCCESS == Status)
    {
      CHARGERLIB_CHARGING_ERROR_TYPES thermalError = CHARGERLIB_CHARGING_ERROR_TSENSE_OK;
      /* Charging Error will be updated if tesens critical or time out error take place */
      Status |= ChargerLib_HandleThermalErrorType(ThermalStatus, &thermalError);
      if (EFI_SUCCESS != Status)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a ChargerLib_HandleThermalErrorType Failed = %r \r\n",__FUNCTION__, Status));
      }
      if((thermalError == CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH )
         || (thermalError == CHARGERLIB_CHARGING_ERROR_TSENSE_TIMEOUT)
         || (thermalError == CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL))
      {
        *pChargingError = thermalError;
        CHARGER_DEBUG(( EFI_D_ERROR, "ChargerLib:: %a Tsense pChargingError = %d \r\n",__FUNCTION__, pChargingError));
      }

      if ( (thermalError == CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH) &&
      ((CHARGERLIB_ATTACHED_CHGR__BATT == AttachedCharger) || (CHARGERLIB_ATTACHED_CHGR__NONE == AttachedCharger)))
      {
        chargerlib_batt_status_info BatteryStatus = { 0 };
        //need to check soc or vbatt, if vbatt low, no charger presense then report no charger presense to shutdown
        Status |= ChargerLib_GetBatteryStatus(&BatteryStatus);
        if (gChargerLibCfgData.soc_based_boot == TRUE)
        {
          if (BatteryStatus.StateOfCharge < gChargerLibCfgData.boot_to_hlos_threshold_in_soc)
          {
            *pChargingError = CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED;
          }
        }
        else
        {
          if (BatteryStatus.BatteryVoltage < gChargerLibCfgData.boot_to_hlos_threshold_in_mv)
          {
            *pChargingError = CHARGERLIB_CHARGING_ERROR_CHARGING_SOURCE_NOT_DETECTED;
          }
        }
      }
     //no need to handle else condition
     //CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH chargerPresent will be considered as thermal Error
     //no matter vbatt or soc threshold.
     //if there is no thermalError, the do not overwrite pChargingError, so no action here either.
    }
    else
    {
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN,"ChargerLib:: %a ChargerLib_GetThermalStatus Failed %r \r\n", __FUNCTION__,Status));
      /* break and exit to AFP ??*/
      *pChargingError = CHARGERLIB_DEVICE_ERROR;
      return Status;
    }
  }

  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a pChargingError = %d \r\n", __FUNCTION__, *pChargingError));
  return Status;
}

EFI_STATUS ChargerLib_EnableHWJeita(BOOLEAN Enable)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!bDbgBoard)
  {
    Status = ChargerLibTarget_EnableHWJeita(Enable);
  }

  return Status;
}

EFI_STATUS ChargerLib_GetSdpMaxCurrent(UINT32 *pSdpMaxCurrent)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(NULL == pSdpMaxCurrent)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pSdpMaxCurrent = USB_2_IUSB_MAX;

  Status = ChargerLibTarget_GetSdpMaxCurrent(pSdpMaxCurrent);

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a CHARGERLIB_CHG_PORT_SDP_CHARGER 2.0 Detected, ChargerCurrent = %d mA \r\n", __FUNCTION__, *pSdpMaxCurrent));

  return Status;
}


EFI_STATUS ChargerLib_GetChgWdogStatus( CHARGERLIB_CHG_WDOG_STS_TYPE *pChgWdogSts)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(NULL == pChgWdogSts)
    return EFI_INVALID_PARAMETER;

  Status = ChargerLibTarget_GetChgWdogStatus(pChgWdogSts);

  return Status;
}


EFI_STATUS ChargerLib_InitializeCharging( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_InitializeCharging();

  return Status;
}

EFI_STATUS ChargerLib_GetErrorAction( CHARGERLIB_CHARGING_ERROR_TYPES  ChargingErrorType, CHARGERLIB_ERROR_ACTION_TYPE *pErrorAction)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!pErrorAction)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pErrorAction = CHARGERLIB_ERROR_ACTION_NONE;

  /* Call Target level get error action as action could be target specific */
  Status = ChargerLibTarget_GetErrorAction(ChargingErrorType, pErrorAction);

  return Status;
}

EFI_STATUS ChargerLib_EnableWdog(BOOLEAN Enable)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_EnableWdog(Enable);

  return Status;
}

EFI_STATUS ChargerLib_PetChgWdog( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_PetChgWdog();

  return Status;
}

/**
  Charger Lib Exit

  @return
  EFI_SUCCESS:           Function returned successfully.
  EFI_DEVICE_ERROR:      The physical device reported an error.
  EFI_UNSUPPORTED:       No Library function linked
*/
EFI_STATUS ChargerLib_Exit( VOID )
{
  EFI_STATUS     Status      = EFI_SUCCESS;
  STATIC BOOLEAN ChgExitDone = FALSE;

  if(FALSE == ChgExitDone)
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLibExit \r\n", __FUNCTION__));
    /* ChargerExit is called only once */
    ChgExitDone = TRUE;

     /* Turn Off Charging Led */
    if(gChargerLibCfgData.charger_led_config)
    {
      /* Make sure to Turn Off LED during exit */
      ChargerLib_LedOn(FALSE);
    }
    /* This is to make sure wdog actions are taken in terms of charger app skips charging as SBL have enabled charger wdog */
    if(CHARGERLIB_CHG_WDOG_LEAVE_ENABLED_ON_EXIT == gChargerLibCfgData.enable_charger_wdog)
    { //Pet the watchdog if feature is enabled, before booting to HLOS
      ChargerLib_PetChgWdog();
    }
    else
    { /* Disable Charger Watchdog before booting to HLOS*/
      ChargerLib_EnableWdog( FALSE );
    }
  }

  Status = ChargerLibTarget_Exit();

  return Status;
}

EFI_STATUS ChargerLib_RecordThermalStartTimeOut()
{
  EFI_STATUS Status   = EFI_SUCCESS;

  /*Update time stamp to global */
  Status = ChargerLib_GetSEllapsedFromBoot(&ThermalTimeOutTimeStamp);
  if (Status == EFI_SUCCESS)
  {
    CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a ThermalTimeOutTimeStamp = %d Seconds \r\n",__FUNCTION__, ThermalTimeOutTimeStamp));
  }
  else
  {
    CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerApp:: %a Error = %d \r\n",__FUNCTION__, Status));
  }

  return Status;
}

EFI_STATUS ChargerLib_GetSEllapsedFromBoot(UINT32 *PmicTimeinS)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 PmicTime = 0;
  STATIC EFI_QCOM_PMIC_RTC_PROTOCOL  *pPmicRtc = NULL;

  if(!PmicTimeinS)
    return EFI_INVALID_PARAMETER;

  if(!pPmicRtc)
  {
    Status = gBS->LocateProtocol( &gQcomPmicRtcProtocolGuid,
                                  NULL,
                                  (VOID**) &pPmicRtc);
    if (Status != EFI_SUCCESS) {
        return Status;
    }
  }

  Status |= pPmicRtc->GetTimeValueRaw(0, &PmicTime);
  if (Status != EFI_SUCCESS)
  {
    CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a Error = %d \r\n",__FUNCTION__, Status));
  }

  *PmicTimeinS = PmicTime;

  return Status;
}

EFI_STATUS ChargerLib_HandleThermalHigh(CHARGERLIB_CHARGING_ERROR_TYPES *pChargingError)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 PmicCurrTimeinS = 0;
  UINT32 ConfigThermalTimeOutinS = 0;

  if(NULL == pChargingError)
    return EFI_INVALID_PARAMETER;

  /* Assign default - but do not need to override charge none error type unless it is time out*/
  /* *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_OK; */

  /* Note Thermal Start Time */
  if(THERMAL_TIMEOUT_NOTSET == ThermalTimeOutTimeStamp)
  {
    /*Record start Time in Seconds */
    Status |= ChargerLib_RecordThermalStartTimeOut();
    *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH;
  }
  else
  {
    /*Timeout in seconds */
    ConfigThermalTimeOutinS = gChargerLibCfgData.thermal_configs.tsens_timeout_mins * MIN_TO_SEC;

    Status |= ChargerLib_GetSEllapsedFromBoot(&PmicCurrTimeinS);
    if((PmicCurrTimeinS - ThermalTimeOutTimeStamp ) >= ConfigThermalTimeOutinS)
    {
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %aChgAppHandleThermalHigh TimeOut: Shutdown device \r\n", __FUNCTION__));
      /*DEBUG */
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a PmicCurrTimeinS = %d s, ThermalTimeOutTimeStamp =  %d s, ",
                               __FUNCTION__, PmicCurrTimeinS, ThermalTimeOutTimeStamp ));
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ConfigThermalTimeOutinS = %d s \r\n", ConfigThermalTimeOutinS));
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_TIMEOUT;
    }
    else
    { /*DEBUG */
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "ChargerLib:: %a PmicCurrTimeinS = %d s, ThermalTimeOutTimeStamp = %d s, ",
                               __FUNCTION__, PmicCurrTimeinS, ThermalTimeOutTimeStamp));
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN, " ConfigThermalTimeOutinS = %d s  Status = %d \r\n",
                              ConfigThermalTimeOutinS, Status));
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_HIGH;
    }
  }
  return Status;
}


EFI_STATUS ChargerLib_HandleThermalErrorType(ChargerLibThermalStatus ThermalStatus, CHARGERLIB_CHARGING_ERROR_TYPES *pChargingError)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(NULL == pChargingError)
    return EFI_INVALID_PARAMETER;

  /* Assign default - but do not need to override charge none error type unless it is time out*/
  /* *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_OK;*/

  switch(ThermalStatus)
  {
    /*Thermal CRITICAL shutodwn with AFP */
    case CHARGERLIB_THERMAL_CRITICAL:
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL;
      break;

    /*Themal timeout during HIGH thermal zone, shutdown */
    case CHARGERLIB_THERMAL_HIGH_TIMEOUT:
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_TIMEOUT;
      break;

    case CHARGERLIB_THERMAL_OK:
      /* Do not need to override charger error none here keeping below for easy reading */
      /* *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_OK; */

      /*Reset TimeOut Stamp*/
      if(THERMAL_TIMEOUT_NOTSET != ThermalTimeOutTimeStamp)
      {
        /* Enable Charging */
        Status |= ChargerLib_ChargerEnable(TRUE);
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Thermal Time Record Reset \r\n", __FUNCTION__));
        ThermalTimeOutTimeStamp = 0;
      }
      break;

    /* Themal HIGH to wait for thermal to come down TsensLowTemp */
    case CHARGERLIB_THERMAL_HIGH:
      /* Disable Charging - take action will diable charging */
      /* Status |= ChargerLib_ChargerEnable(FALSE); */
      Status |= ChargerLib_HandleThermalHigh(pChargingError);
      if(EFI_SUCCESS != Status)
      {
        *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL;
        CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib_HandleThermalHigh Error = %r Setting Thermal Time out Timer \r\n", __FUNCTION__, Status));
      }
    break;

    default:
    case CHARGERLIB_THERMAL_STATUS_MAX:
      CHARGER_FILE_UART_DEBUG((EFI_D_WARN,"ChargerLib:: %a ThermalStatus = %d \r\n", __FUNCTION__, ThermalStatus));
      *pChargingError = CHARGERLIB_CHARGING_ERROR_TSENSE_CRITICAL;
    break;
  }

  CHARGER_FILE_UART_DEBUG((EFI_D_WARN,"ChargerLib:: %a ThermalStatus = %d \r\n", __FUNCTION__, ThermalStatus));
  return Status;
}

EFI_STATUS ChargerLib_GetChargingStatus(BOOLEAN *pChargingEnabled)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(!pChargingEnabled)
    return EFI_INVALID_PARAMETER;

  *pChargingEnabled = FALSE;

  Status = ChargerLibTarget_GetChargingStatus(pChargingEnabled);

  return Status;
}

EFI_STATUS ChargerLib_PrintDCInStatus()
{
  EFI_STATUS Status = EFI_SUCCESS;
  EfiAdcResultType AdcResult = { 0 };

  Status = ChargerLib_GetADCReading(ADC_INPUT_DC_IN, sizeof(ADC_INPUT_DC_IN), &AdcResult);

  if (EFI_ERROR(Status))
  {
    CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: Failed to get ADC reading from ADC_INPUT_DC_IN!\n", __FUNCTION__));
    return Status;
  }

  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: DCInVoltage = %d\n", __FUNCTION__, AdcResult.nPhysical));

  Status = ChargerLib_GetADCReading(ADC_INPUT_DC_IN_I, sizeof(ADC_INPUT_DC_IN_I), &AdcResult);
  if (EFI_ERROR(Status))
  {
    CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: Failed to get ADC reading from ADC_INPUT_DC_IN_I!\n", __FUNCTION__));
    return Status;
  }

  CHARGER_DEBUG((EFI_D_ERROR, "ChargerLib:: %a: DCInCurrent = %d\n", __FUNCTION__, AdcResult.nPhysical));
  return Status;
}

EFI_STATUS ChargerLib_GetADCReading(CONST CHAR8 *pszInputName, UINT32 uInputNameSize, EfiAdcResultType *pEfiAdcResult)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (NULL == pszInputName || NULL == pEfiAdcResult)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = ChargerLibTarget_GetADCReading(pszInputName, uInputNameSize, pEfiAdcResult);

  return Status;
}


EFI_STATUS ChargerLib_LedOn( BOOLEAN bLedOn)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_LedOn(bLedOn);

  return Status;
}

EFI_STATUS ChargerLib_SetDcinCurrent(UINT32 DcinCurrentInmA)
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a ChargerLib Current = %d mA\r\n", __FUNCTION__, DcinCurrentInmA));

  Status = ChargerLibTarget_SetDcinCurrent(DcinCurrentInmA);

  return Status;
}

EFI_STATUS ChargeLib_IsDcinValid(BOOLEAN* pIsValid)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!pIsValid)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = ChargeLibTarget_IsDcinValid(pIsValid);

  return Status;
}


EFI_STATUS ChargerLib_GetShipMode(BOOLEAN* Enabled)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8  EnShipMode = 0;
  UINTN  DataSize = 0;
  UINT16 UniStr[64] = {0};

  AsciiStrToUnicodeStr((CHAR8 *)gChargerLibCfgData.shipmode_cfg.shipMode_str, UniStr);

  /* Read Factory Mode variable  */
  DataSize = sizeof(EnShipMode);
  Status = gRT->GetVariable (UniStr,
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &EnShipMode );
  /* Assign only if variable exist in fv*/
  EnShipMode = (Status == EFI_SUCCESS) ? EnShipMode : 0;

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Status = 0x%08X \r\n", __FUNCTION__, Status));
  if(Status == EFI_NOT_FOUND)
  {
    EnShipMode = 0;
    Status |= ChargerLib_FlushShipMdeVar(UniStr, EnShipMode);
    if(Status == EFI_SUCCESS)
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship Mode variable for first time read failure - SUCCESS = %r \r\n", __FUNCTION__, Status));
    }
    else
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship Mode variable for first time failed = %r \r\n",__FUNCTION__, Status));
      return Status;
    }
  }
  if(EnShipMode)
  {
    *Enabled = TRUE;
    /* Clear FV variable */
    EnShipMode = 0;
    Status |= ChargerLib_FlushShipMdeVar(UniStr, EnShipMode);
    if(Status == EFI_SUCCESS)
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship mode Clearing variable SUCCESS = %r \r\n",__FUNCTION__, Status));
    }
    else
    {
      CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship mode Clearing variable failed = %r \r\n", __FUNCTION__,Status));
    }
  }
  else
  {
    *Enabled = FALSE;
  }
  return Status;
}

/* Flush the NV Storage tables before turning ON write protection
 * Do this only if the protocol available. During initial stages the
 * the protocol might not be installed */

EFI_STATUS ChargerLib_FlushShipMdeVar(UINT16* variable, UINT8 Value)
{
  EFI_VARIABLESERVICES_PROTOCOL *VariableServicesProtocol = NULL;
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN   DataSize = sizeof(Value);
  UINT32  Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS);

  if (NULL == variable)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = gRT->SetVariable(variable,
                            &gQcomTokenSpaceGuid,
                            Attributes,
                            DataSize,
                            &Value);
  Status |= gBS->LocateProtocol(&gEfiVariableServicesProtocolGuid, NULL, (VOID **)&VariableServicesProtocol);

  if (VariableServicesProtocol != NULL)
  {
    Status |= VariableServicesProtocol->FlushVariableNV(VariableServicesProtocol);
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Clearing Factory Mode variable flushed SUCCESS = 0x%08X \r\n", __FUNCTION__, Status));
  }
  else
  {
    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Ship Mode variable flushed Failed = %r \r\n", __FUNCTION__, Status));
  }

  return Status;
}

EFI_STATUS ChargerLib_IsChargingSupported(BOOLEAN *pChargingSupported)
{
  EFI_STATUS Status = EFI_SUCCESS;


  Status = ChargerLibTarget_IsChargingSupported(pChargingSupported);

  //Add HW Validation here in the future

  CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Support Charging = %d\r\n", __FUNCTION__, *pChargingSupported));
  return Status;
}

EFI_STATUS ChargerLib_GetDAMConnectStatus(BOOLEAN *DAMConnectSts)
{
    EFI_STATUS Status = EFI_SUCCESS;

    Status = ChargerLibTarget_DAMConnectSts(DAMConnectSts);

    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Factory Mode Connect Status = %d\r\n", __FUNCTION__, *DAMConnectSts));
    return Status;
}

EFI_STATUS ChargerLib_SetDAMICL(DAM_SELECT_ICL DAMIcl)
{
    EFI_STATUS Status = EFI_SUCCESS;

    Status = ChargerLibTarget_SetDAMIcl((DAM_SEL_ICL)DAMIcl);

    CHARGER_DEBUG((EFI_D_WARN, "ChargerLib:: %a Setting DAM ICL to = %d\r\n", __FUNCTION__, DAMIcl));
    return Status;

}

EFI_STATUS ChargerLib_GetChargerInputStatus(CHARGERLIB_CHARGER_INPUT_STATUS *pChargerInputStatus)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_GetChargerInputStatus(pChargerInputStatus);

  return Status;
}

EFI_STATUS ChargerLib_GetDisplayImageType(CHARGERLIB_EVENT_DISP_IMAGE_TYPE *pDispImage)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ChargerLibTarget_GetDisplayImageType(pDispImage);

  return Status;
}

