/**************************************************************************
 * FILE: devprg_utils.h
 *
 * Provides utility functions for devprg.
 * For now we just have the sleep and time
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/3.2/QcomPkg/Library/DevPrgLib/devprg_utils.h#1 $
  $DateTime: 2018/07/26 18:39:07 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-13-07   svl   Create.

===========================================================================*/

#ifndef __DEVPRG_UTILS_H__
#define __DEVPRG_UTILS_H__

#include "comdef.h"


uint64 devprg_time_usec(void);
int devprg_usleep(uint64 microsecs);

#endif /* __DEVPRG_UTILS_H__ */
