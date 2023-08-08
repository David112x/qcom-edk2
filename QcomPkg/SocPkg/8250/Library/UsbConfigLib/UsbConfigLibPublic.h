/** @file \UsbConfigPublic.h
 *
 * The Public Header for UsbConfigLib which defines the command data structure 
 * and lib shared between USB Core and USB Port modules.
 */
//============================================================================
/**
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc. All rights reserved.
  Portions Copyright (c) 2007 - 2013 Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php
  
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 */
//============================================================================
/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 02/13/17   gk	     Port changes from boot.xf.2.0 and boot.xf.2.2
 01/05/17   jaychoi  Porting MP Core Changes for Hana from Poipu
 11/27/17   amitg    MP Core Changes for Poipu
 10/20/17   gk       Updated new Pmic API for UsbPwrCtrlDxe for UEFI
 08/10/17   amitg    Init Changes for Poipu
 03/17/17   pm       Add secondary core
 02/06/17   pm       Add Clamshell Support
 02/23/16   ck       Init Check-In
=============================================================================*/

#ifndef _USB_CONFIG_PUB_H_
#define _USB_CONFIG_PUB_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/EFIUsbPwrCtrl.h>
#include <Protocol/EFIUsbConfig.h>
#include "UsbError.h"
#include <Library/UsbConfigLib.h>
#include <Library/PcdLib.h>
#include <Protocol/EFIPlatformInfo.h>


extern EFI_USB_PWR_CTRL_PROTOCOL              *gPwrCtrlProtocol;
extern EFI_GUID                               gQcomTokenSpaceGuid; // Used for getting the BCD variable "UsbUseSynopsys"
/**
USB core enumeration
*/
typedef enum {
  USB_CORE_0_SM_8250 = 0,      /**< Core0. */
  USB_CORE_1_SM_8250,         /**< Core1. */
  USB_CORE_MAX_NUM_SM_8250,    /**< Invalid Core. */
} QCOM_USB_CORE_NUM_SM_8250;

/**
USB Mode
*/
typedef enum {
  USB_HOST_MODE_SM_8250    = 1, /**< HOST MODE. */
  USB_DEVICE_MODE_SM_8250,      /**< DEVICE MODE. */
  USB_DUAL_ROLE_SM_8250,        /**< Detect USB mode based on cable detection */
} QCOM_USB_MODE_TYPE_SM_8250;

/**
USB Type C Override Value
*/

typedef enum {
  UEFI_VAR_CC_LANE_OVERRIDE_DEFAULT = 0,      /** take HW setting*/
  UEFI_VAR_CC_LANE_OVERRIDE_A,         /** hardcore Lane A*/
  UEFI_VAR_CC_LANE_OVERRIDE_B,    /**< hardcore Lane B */
} UEFI_VAR_CC_LANE_OVERRIDE;

#define INVALID_CORE(CoreNum) (CoreNum < USB_CORE_MAX_NUM_SM_8250)?0:1

#endif /* _USB_CONFIG_PUB_H_ */

