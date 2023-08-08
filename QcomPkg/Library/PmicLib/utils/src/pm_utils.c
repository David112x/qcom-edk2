/*! \file
*  
*  \brief  pm_utils.c ----This file contains the implementation of pm_malloc()
*  \details This file contains the implementation of pm_malloc()
*  
*  &copy; Copyright 2012, 2015-2019 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


$Header: //components/rel/boot.xf/3.2/QcomPkg/Library/PmicLib/utils/src/pm_utils.c#3 $
$Author: pwbldsvc $
$DateTime: 2019/06/17 18:09:44 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/18/17   aab     Depricated pm_convert_time_to_timetick() and  pm_convert_timetick_to_time(), Since it assumes a fixed Clock speed
12/05/17   aab     Added support to log total number of mem allocated
07/21/17   czq     Use Spare reg PON_PERPH_RB_SPARE instead of PON_DVDD_RB_SPARE
08/12/16   aab     Added pm_sbl_get_adc_batt_id_pd_res_ohms() 
08/03/16   akm     Moved pm_clk_busy_wait to framework/src pm_busy_wait
05/26/16   al      Fix for API to return idx 
09/25/15   al      Removing the redundant check
05/01/14   aab     Added pm_clk_busy_wait()
03/20/14   aab     Added pm_boot_adc_get_mv() 
06/11/13   hs      Adding settling time for regulators.
06/20/12   hs      Created

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_utils.h"
#include "DALSys.h"
#include "CoreVerify.h"
#include "busywait.h"
#include "pmio_pon.h"
#include "pm_comm.h"


/*===========================================================================

                        GLOBALS and TYPE DEFINITIONS 

===========================================================================*/
static uint32 pmic_malloc_total_size = 0;


/*===========================================================================

                        FUNCTION DEFINITIONS 

===========================================================================*/
void pm_malloc(uint32 dwSize, void **ppMem)
{
    DALResult dalResult = DAL_SUCCESS;

    dalResult = DALSYS_Malloc(dwSize, ppMem);
    CORE_VERIFY(dalResult == DAL_SUCCESS );
    CORE_VERIFY_PTR(*ppMem);

    DALSYS_memset(*ppMem, 0, dwSize);

    pmic_malloc_total_size += dwSize;
}



uint32 pm_get_mem_allocated(void)
{
  return pmic_malloc_total_size;
}



uint8
pm_utils_get_upper_idx(uint32 data_value, uint32 *data_list, uint8 list_count)
{
  uint8 idx = 0;

  if (data_list == NULL || list_count == 0)
  {
    CORE_VERIFY(0);
  }

  for (idx = 0; idx < list_count-1; idx++)
  {
    if (data_value <= data_list[idx]) 
      break;
  }
 
  return idx;
} 


boolean 
pm_driver_init_status(void)
{

  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 spare_data;
  uint8 perph_reset_data;  

  err_flag = pm_comm_read_byte_mask(0, PMIO_PON_PERPH_RB_SPARE_ADDR, PON_PERPH_RB_SPARE_DRIVER_INIT_MASK, &spare_data);
  err_flag = pm_comm_read_byte_mask(0, PMIO_PON_PERPH_RESET_CTL3, PMIO_PON_FOLLOW_WARM_RB_RMSK, &perph_reset_data);

  if ((spare_data == PON_PERPH_RB_SPARE_DRIVER_INIT_MASK) && (perph_reset_data == PMIO_PON_FOLLOW_WARM_RB_RMSK))
  {
     return TRUE;
  }
  else
  {
     return FALSE;
  }
}

boolean 
pm_device_init_status(void)
{

  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 spare_data;
  uint8 perph_reset_data;  

  err_flag = pm_comm_read_byte_mask(0, PMIO_PON_PERPH_RB_SPARE_ADDR, PON_PERPH_RB_SPARE_DEVICE_INIT_MASK, &spare_data);
  err_flag = pm_comm_read_byte_mask(0, PMIO_PON_PERPH_RESET_CTL3, PMIO_PON_FOLLOW_WARM_RB_RMSK, &perph_reset_data);

  if ((spare_data == PON_PERPH_RB_SPARE_DEVICE_INIT_MASK) && (perph_reset_data == PMIO_PON_FOLLOW_WARM_RB_RMSK))
  {
     return TRUE;
  }
  else
  {
     return FALSE;
  }
}

