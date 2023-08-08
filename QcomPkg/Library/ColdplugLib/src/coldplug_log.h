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

  $Header: //components/rel/boot.xf/3.2/QcomPkg/Library/ColdplugLib/src/coldplug_log.h#1 $ $DateTime: 2018/07/26 18:39:07 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#ifndef __COLDPLUG_LOG_H__
#define __COLDPLUG_LOG_H__

#include "coldplug_config_i.h"


#ifdef COLDPLUG_LOG_ENABLE

void coldplug_log (const char *filename, int line_no, char *msg);

#else

static inline void coldplug_log(const char *filename, int line_no, char *msg)
{
	(void) filename, (void) line_no; (void) msg;
}

#endif

#define COLDPLUG_LOG(msg)					\
	do {							\
		coldplug_log(__FILE__, __LINE__, msg);		\
	} while (0);

#endif /* __COLDPLUG_LOG_H__ */
