/*
===========================================================================
*/
/**
  @file VCSImage.c

*/
/*
  ====================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/3.2/QcomPkg/SocPkg/8250/Library/VcsLib/VCSImage.c#1 $
  $DateTime: 2019/03/27 17:31:08 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "VCSImage.h"
#include "npa_resource.h"

/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/

/*
 * Main image context.
 */
VCSImageCtxtType VCS_ImageCtxt;


/* =========================================================================
      Prototypes
==========================================================================*/


/* =========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : VCS_LoadNV
** =========================================================================*/
/*
  See VCSMSS.h
*/

void VCS_LoadNV
(
  void
)
{

} /* END VCS_LoadNV */

/* =========================================================================
**  Function : VCS_InitImage
** =========================================================================*/
/*
  See VCSDriver.h
*/

DALResult VCS_InitImage
(
  VCSDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Assign the image context.                                             */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->pImageCtxt = &VCS_ImageCtxt;

  return DAL_SUCCESS;

} /* END VCS_InitImage */


