#ifndef __PM_SBL_BOOT__H__
#define __PM_SBL_BOOT__H__

/*! \file pm_sbl_boot.h
*  \n
*  \brief This header file contains enums and helper function declarations needed
*  \n during PMIC device initialization and during initial PMIC SBL settings configured
*  \n through the PDM auto-generated code.
*  \n
*  \n &copy; Copyright 2018 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/boot.xf/3.2/QcomPkg/Library/PmicLib/framework/inc/pm_sbl_boot.h#3 $
$Author: pwbldsvc $
$DateTime: 2019/06/17 18:09:44 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/15   aab     Renamed pm_target_information_spmi_chnl_cfg() to pm_bus_init()
06/25/15   al      moving loader specific API to loader specific file
07/03/13   kt      Added Comment header for this file. 
 
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_err_flags.h"
#include "pm_boot.h"
#include "pm_version.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef enum
{
  PM_SWIRE_MAP_A = 0,
  PM_SWIRE_MAP_B = 1,
  PM_SWIRE_MAP_C = 2,
  PM_SWIRE_MAP_D = 3,
  PM_SWIRE_MAP_INVALID
}pm_swire_map_type;

typedef struct
{
  boolean supported;
  pm_swire_map_type swire_map;
}pm_swire_map_cfg;

typedef struct
{
  boolean verbose_uart_logging;  // Used to enable/disable verbose UART logging
  boolean rfclk2_config_flag;    // FALSE: Default for all customer; TRUE: Disable RFCLK2
  pm_swire_map_cfg  swire_cfg;
}pm_sbl_specific_data_type;


/*===========================================================================

                        FUNCTION PROTOTYPES 

===========================================================================*/

unsigned int pm_clk_busy_wait (unsigned int mS);

pm_err_flag_type pm_boot_tcxo_config (void);

pm_err_flag_type pm_clk_tcxo_config(void);

pm_err_flag_type pm_pon_init(void);

/*! \brief This function configures the spmi channel for the PMIC peripherals.
 *  \param[in] .
 *
 *  \return Error flag.
 *
 *  <b>Dependencies</b>
 *  \li pm_processor_init_comms() .
 */ 
pm_err_flag_type pm_bus_init(void);

#endif // __PM_SBL_BOOT__H__
