/*! \file pm_rgb_driver.c 
*  \n
*  \brief This file contains RGB peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================

$Header: //components/rel/boot.xf/3.2/QcomPkg/Library/PmicLib/drivers/rgb/src/pm_rgb_driver.c#2 $
$Author: pwbldsvc $
$DateTime: 2019/06/17 18:09:44 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/12/17   aab     Moved common Register address defination to driver file  
08/20/14   al      Updating comm lib 
08/29/14   al      KW fixes
05/20/14   al      Architecture update 
05/09/14   va      Using common debug and assert Marco
02/27/13   aab     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rgb_driver.h"
#include "CoreVerify.h"
#include "pm_utils.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the RGB driver data */
static pm_rgb_data_type *pm_rgb_data_arr[PM_MAX_NUM_PMICS];

static pm_rgb_register_info_type rgb_reg =
{
  .base_address      = 0xD000,
  .peripheral_offset =  0x100,
  .src_sel           =  0x045,
  .en_ctl            =  0x046,
  .chgr_led_config   =  0x058,
}; 





/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_rgb_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_rgb_data_type *rgb_ptr = NULL;

    rgb_ptr = pm_rgb_data_arr[pmic_index];

    if (rgb_ptr == NULL)
    {
        pm_malloc( sizeof(pm_rgb_data_type), (void**)&rgb_ptr);
                                                    
        /* Assign Comm ptr */
        rgb_ptr->comm_ptr = comm_ptr;

        /* RGB Register Info - Obtaining Data through dal config */
        rgb_ptr->rgb_reg_table = &rgb_reg;
        CORE_VERIFY_PTR(rgb_ptr->rgb_reg_table);

        rgb_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_RGB_NUM, pmic_index);
        CORE_VERIFY(rgb_ptr->num_of_peripherals != 0);

        rgb_ptr->sub_type = peripheral_info->peripheral_subtype;

        pm_rgb_data_arr[pmic_index] = rgb_ptr;
    }
}

pm_rgb_data_type* pm_rgb_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_rgb_data_arr[pmic_index];
  }

  return NULL;
}

uint8 pm_rgb_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_rgb_data_arr[pmic_index] !=NULL))
  {
      return pm_rgb_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}



