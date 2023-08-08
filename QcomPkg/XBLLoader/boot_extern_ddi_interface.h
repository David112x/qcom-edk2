#ifndef DDI_H
#define DDI_H

/**
  @file boot_extern_ddi_interface.h
  @brief
  Common definitions used by DDI.
*/

/*=============================================================================
  Copyright (c) 2017 Qualcomm Technologies,  Inc.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
when       who        what, where, why
--------   ---        ------------------------------------------------------------
07/14/17   daison     Initial revision.
=============================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "ddr_drivers.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================

**  Function :  boot_pre_ddi_entry

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is the entry point to DDI before training
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* 
*/

void boot_pre_ddi_entry();

/*=============================================================================

**  Function :  boot_ddi_entry

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is the entry point to DDI
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* 
*/

void boot_ddi_entry();


void sbl1_read_from_ddr_partition
(
  uint32 data_offset,
  uint8* data_buf,
  uint32 data_buf_size
);

#endif /* DDI_H */
