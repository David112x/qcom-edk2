/*! \file pm_init.c
*   \n
*   \brief This file contains PMIC initialization function which initializes the PMIC Comm
*   \n layer, PMIC drivers and PMIC applications.
*   \n
*   \n &copy; Copyright 2010-2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


$Header: //components/rel/boot.xf/3.2/QcomPkg/Library/PmicLib/framework/src/pm_core_init.c#3 $
$Author: pwbldsvc $
$DateTime: 2019/06/17 18:09:44 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/13/17    al     Add support to read pbs info in core
08/08/15   aab     Added pm_device_setup()
07/23/15   al      Removing NPA from common
12/06/14   al      enabling npa
11/12/14   al      adding for compilation
07/16/14   akm     Comm change Updates
03/31/14   akm     Cleanup
01/15/13   aab     Fixed KW error
05/10/11   jtn     Fix RPM init bug for 8960
07/01/10   umr     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_resource_manager.h"
#include "pm_comm.h"
#include "device_info.h"
#include "pm_device.h"
#include "pm_target_information.h"
#include "pm_rpmh.h"
#include "pm_prm_init.h"
#include "pm_prm_os.h"
#include "pm_log_utils.h"

#define PM_PRIMARY_PMIC_INDEX  0

boolean bPMICStub = FALSE;

static volatile boolean is_standalone = FALSE;

void pm_print_all_pmic(void)
{
  int i = 0;
  /*print pmic revision*/
  while (i < PM_MAX_NUM_PMICS)
  {
    if (pm_get_pmic_model(i) != PMIC_IS_UNKNOWN)
    {
      pm_log_message("PM%d: %d, ",i, pm_get_pmic_model(i));
    }
    i += 1;
  }
  pm_log_message("\n");
}


pm_err_flag_type pm_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  err_flag = pm_device_setup();

  if (pm_get_pmic_model(PM_PRIMARY_PMIC_INDEX) == PMIC_IS_UNKNOWN)
  {
    return PM_ERR_FLAG_FAILURE;
  }

  err_flag |= pm_target_information_init();

  pm_comm_info_init();

  pm_resource_manager_init();

  if (is_standalone)
  {
    return err_flag;
  }

  pm_rpmh_init();

  pm_prm_init();

  pm_print_all_pmic();

  return err_flag;

}

uint32 pm_get_img_type(void)
{
  return PM_IMG_CORE;
}
