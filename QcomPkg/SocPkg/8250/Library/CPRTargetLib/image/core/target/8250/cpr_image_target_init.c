/**
 * @file:  cpr_image_target_init.c
 *
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2019/09/23 12:37:27 $
 * $Header: //components/rel/boot.xf/3.2/QcomPkg/SocPkg/8250/Library/CPRTargetLib/image/core/target/8250/cpr_image_target_init.c#3 $
 * $Change: 20667317 $
 */
#include <string.h>
#include "cpr_logs.h"
#include "cpr_data.h"
#include "cpr_rail.h"
#include "cpr_smem.h"
#include "cpr_image.h"
#include "cpr_measurements.h"
#include "cpr_cfg.h"
#include "cpr_hal.h"
#include "cpr_utils.h"
#include "cpr_image_target_init.h"
#include "cpr_image_target.h"
#include "cprf_hwio.h"

//******************************************************************************
// Local Helper Functions
//******************************************************************************


static cpr_cfg_funcs* init_open_loop(cpr_rail* rail)
{
    /*
     * Kona V1:
     * GLOBAL_RC = 0 – All global rails blown at ceiling voltages
     * GLOBAL_RC = 1 – All global rails blown at CPR recommended voltages, includes all aging fuses blown with correct values for all rails except AON
     * GLOBAL_RC = 2 – All global rails blown at CPR recommended voltages, includes all aging fuses blown correctly
     *
     * Kona V2:
     * GLOBAL_RC = 0 and TARG_VOLT fuse = +0 (i.e: 0.)– None of the CPR fuses blown correctly.
     * GLOBAL_RC = 0 and TARG_VOLT fuse = -0 (i.e: 16.) – All global rails blown at CPR recommended voltages, includes all aging fuses blown correctly 
     * GLOBAL_RC = 1 – All global rails blown at CPR recommended voltages, includes all aging fuses blown correctly 
     */

    uint8 i = 0;
    uint8 rev = 0;
    int16 decodedFuse = 0;
    cpr_rail_state *railState = NULL;
    const cpr_voltage_plan *vp = NULL;
    const cpr_voltage_data *vpMode = NULL;
    boolean useGlobalCeiling = false;

    CPR_LOG_TRACE( "Initializing open loop on %s", rail->name );

    railState = cpr_utils_get_rail_state(rail->id);
    vp = rail->vp;

    if (railState)
    {
        rev = railState->railCPRRev;

        for (i = 0; i < railState->modeSettingsCount; i++)
        {
            vpMode = &(vp->modes[i]);

            if(vpMode->fuses != NULL && vpMode->fuses->volt != NULL)
            {
                //Decode the fuse as unsigned, to get the absolute value of the fuse.
                decodedFuse =
                    cpr_utils_decode_fuse_value( vpMode->fuses->volt, 1, false );
                break;
            }
        }
    }

    if (rev == 0 && decodedFuse == 0)
        useGlobalCeiling = true;

    cpr_rail_set_initial_voltages( rail, useGlobalCeiling, false );

    return NULL;
}

static cpr_cfg_funcs* init_settle(cpr_rail* rail)
{
    CPR_LOG_TRACE( "Settling on %s", rail->name );

    for(int i = 0; i < rail->settleModesCount; i++) {
        cpr_measurements_settle( rail, rail->settleModes[i] );
    }

    CPR_LOG_TRACE( "--- Done with settling ---" );

    return NULL;
}

static cpr_cfg_funcs* init_hw_open_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "--- CPR HW Open Loop ---" );

    cpr_rail_init_rail_hw( rail, true );

    init_open_loop(rail);

    cpr_rail_write_all_voltages( rail );

    return NULL;
}

static cpr_cfg_funcs* enable_hw_open_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "--- Enable CPR HW Open Loop ---" );

    cpr_rail_write_cprf_settings( rail, cpr_image_get_cprf_id(rail) );

    return NULL;
}

static cpr_cfg_funcs* init_hw_closed_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "--- CPR HW Closed Loop ---" );

    cpr_rail_init_rail_hw( rail, true );

    cpr_measurements_aging( rail );

    init_open_loop(rail);

    CPR_LOG_TRACE( "Configuring closed loop on %s", rail->name );

    cpr_rail_calculate_target_quotients( rail, &cpr_info.railStates[rail->railIdx] );

    cpr_rail_write_all_voltages( rail );

    cpr_rail_write_all_target_quotients( rail );

    return NULL;
}

static cpr_cfg_funcs* enable_hw_closed_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "--- Enable CPR HW Closed Loop ---" );

    cpr_rail_write_cprf_settings(rail, cpr_image_get_cprf_id(rail));

    cpr_rail_enable_hw_closed_loop(rail, cpr_image_get_cprf_id(rail));

    return NULL;
}

static cpr_cfg_funcs* init_closed_loop(cpr_rail* rail)
{
    cpr_rail_init_rail_hw( rail, true );

    cpr_measurements_aging( rail );

    init_open_loop(rail);

    CPR_LOG_TRACE( "Configuring closed loop on %s", rail->name );

    cpr_rail_calculate_target_quotients( rail, &cpr_info.railStates[rail->railIdx] );

    init_settle(rail);

    return NULL;
}

static cpr_cfg_funcs* enable_closed_loop(cpr_rail* rail)
{
    CPR_LOG_TRACE( "Enabling closed loop on %s", rail->name );

    cpr_rail_register_isr( rail );

    return NULL;
}


//******************************************************************************
// Default Enablement Structures
//******************************************************************************

static cpr_cfg_funcs init_funcs[CPR_INIT_FUNCS_COUNT] =
{
[CPR_INIT_NONE]           = {.cMode = CPR_CONTROL_NONE,           .init = NULL,                .enable = NULL},
[CPR_INIT_OPEN_LOOP]      = {.cMode = CPR_CONTROL_OPEN_LOOP,      .init = init_open_loop,      .enable = NULL},
[CPR_INIT_SW_CLOSED_LOOP] = {.cMode = CPR_CONTROL_SW_CLOSED_LOOP, .init = init_closed_loop,    .enable = NULL},
[CPR_INIT_HW_OPEN_LOOP]   = {.cMode = CPR_CONTROL_HW_OPEN_LOOP,   .init = init_hw_open_loop,   .enable = enable_hw_open_loop},
[CPR_INIT_HW_CLOSED_LOOP] = {.cMode = CPR_CONTROL_HW_CLOSED_LOOP, .init = init_hw_closed_loop, .enable = enable_hw_closed_loop},
};

cpr_cfg_funcs* cpr_image_target_get_init_func(cpr_init_funcs_id func_id)
{
    CPR_ASSERT(func_id < CPR_INIT_FUNCS_COUNT);

    return &init_funcs[func_id];
}
