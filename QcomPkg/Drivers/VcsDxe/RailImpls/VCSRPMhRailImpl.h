#ifndef __VCSRPMHRAILIMPL_H__
#define __VCSRPMHRAILIMPL_H__
/*
===========================================================================
*/
/**
  @file VCSRailRPMh.h

  Interface definitions for the RPMh rail impl type.
*/
/*
  ====================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/boot.xf/3.2/QcomPkg/Drivers/VcsDxe/RailImpls/VCSRPMhRailImpl.h#1 $
  $DateTime: 2019/03/27 17:31:08 $
  $Author: pwbldsvc $
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "../VCSBSP.h"
#include "rpmh_client.h"


/*=========================================================================
      Extern data
==========================================================================*/

extern VCSRailControlType VCSRPMhRailImplControl;


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * VCSRPMhRailImplType
 *
 *  szName        - Name of RPMh RSC resource.
 *  eDRVId        - Enum identifier for this driver.
 *  bBatchCommand - Boolean indicator whether to batch command along with dependencies.
 *  hRPMh         - Handle to our RPMh client.
 *  anAddress     - Array of addresses to the RSC for this rail.
 *  aSets         - Array of command sets for this rail.
 */
typedef struct VCSRPMhRailImplType
{
  const char         *szName;
  RSCSW_DRV_MAPPING   eDRVId;
  boolean             bBatchCommand;

  rpmh_client_handle  hRPMh;
  uint32              anAddress[VCS_NPA_RAIL_NUM_RESOURCES];
  rpmh_command_set_t  aSets[RPMH_NUM_SETS];
} VCSRPMhRailImplType;


#endif  /* __VCSRPMHRAILIMPL_H__ */ 

