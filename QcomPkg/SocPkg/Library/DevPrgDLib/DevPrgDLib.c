/** @file DeviceProgrammerDdrLib.c

  Stub functions for DeviceProgrammer DDR.

  Copyright (c) 2014,2019 Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


when       who       what, where, why
--------   ---       -----------------------------------------------------------
12/02/14   wkimberl  Initial revision.

=============================================================================*/

#include "boot_sbl_if.h"
#include "deviceprogrammer_initialize.h"

/*!
 * Stub for boot_hand_control_to_deviceprogrammer_ddr_main()
 *
 * @param bl_shared_data
 *    The shared bootloader information.
 *
*****************************************************************************/
void
boot_hand_control_to_deviceprogrammer_ddr_main (bl_shared_data_type *bl_shared_data)
{
  deviceprogrammer_entry(bl_shared_data->sbl_shared_data->pbl_shared_data);
}
