/**
 * @file:  cpr_image_target_init.h
 * @brief:
 *
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/09/07 11:44:37 $
 * $Header: //components/rel/boot.xf/3.2/QcomPkg/SocPkg/8250/Library/CPRTargetLib/image/xbl/target/8250/cpr_image_target_init.h#1 $
 * $Change: 17069176 $
 */
#ifndef CPR_IMAGE_TARGET_INIT_H
#define CPR_IMAGE_TARGET_INIT_H

//******************************************************************************
// Default Enablement Structures
//******************************************************************************
struct cpr_cfg_funcs;

typedef enum
{
    CPR_INIT_NONE, /* used to disable CPR */
    CPR_INIT_OPEN_LOOP,
    CPR_INIT_SW_CLOSED_LOOP,
    CPR_INIT_HW_OPEN_LOOP,  /*used to share cpr info via SMEM with out initializing any of the CPR hw */
    CPR_INIT_HW_CLOSED_LOOP,/*used to share cpr info via SMEM with out initializing any of the CPR hw */

    CPR_INIT_FUNCS_COUNT
} cpr_init_funcs_id;

struct cpr_cfg_funcs* cpr_image_target_get_init_func(cpr_init_funcs_id func_id);

#endif

