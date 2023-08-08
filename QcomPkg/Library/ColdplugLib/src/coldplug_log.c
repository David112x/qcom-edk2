/***********************************************************************
 * Coldplug Log module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug Log module functions.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/3.2/QcomPkg/Library/ColdplugLib/src/coldplug_log.c#1 $ $DateTime: 2018/07/26 18:39:07 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_log.h"

#ifdef COLDPLUG_LOG_ENABLE

#include "boot_logger.h"

void coldplug_log(const char *filename, int line_no, char *msg)
{
	(void) filename; (void) line_no;
	boot_log_message(msg);
}

#endif /* COLDPLUG_LOG_DISABLE */
