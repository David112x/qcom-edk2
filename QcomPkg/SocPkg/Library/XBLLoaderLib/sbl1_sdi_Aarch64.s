//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
//
//                              SBL1 SDI Aarch64
//
// GENERAL DESCRIPTION
//   This file contains assembly routines used by debug image and xbl loader
//   image API's.
//
// Copyright 2016 by Qualcomm Technologies, Inc.
// All Rights Reserved.
//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
//
//                           EDIT HISTORY FOR FILE
//
// This section contains comments describing changes made to the module.
// Notice that changes are listed in reverse chronological order.
//
// $Header: //components/rel/boot.xf/3.2/QcomPkg/SocPkg/Library/XBLLoaderLib/sbl1_sdi_Aarch64.s#4 $
//
// when       who     what, where, why
// --------   ---     --------------------------------------------------------
// 01/22/19   yps     Removed specific sections
// 03/18/16   kpa     Initial revision
//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


//============================================================================
//
//                            MODULE INCLUDES
//
//============================================================================
#include "boot_msm.h"
#include "boot_target.h"

//============================================================================
//
//                             MODULE DEFINES
//
//============================================================================
//


//============================================================================
//
//                             MODULE IMPORTS
//
//============================================================================

  // Import the external symbols that are referenced in this module if any.


//============================================================================
//
//                             MODULE EXPORTS
//
//============================================================================

  // Export the symbols
  .global sbl1_get_current_el
	
//============================================================================
//
//                             MODULE DATA AREA
//
//============================================================================


//============================================================================
// Qualcomm Debug Image Section
//============================================================================
    .text
    .p2align 4
    
    //CODE

//============================================================================
// sbl1_get_current_el
//
//// RETURN
//   unsigned long  -  value of current exception level
//
// DESCRIPTION -
//  Return the value of the current exception level we are operating in 
//
//============================================================================
.type sbl1_get_current_el, @function
sbl1_get_current_el:
   // Check the current Exception Level
   //----------------------------------
   MRS    x0, CurrentEL
   LSR    x0, x0, #2   
   ret  
  
  .end
