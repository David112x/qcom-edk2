/**
 * @file:  cpr_defs.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2019/09/12 12:21:02 $
 * $Header: //components/rel/boot.xf/3.2/QcomPkg/Include/cpr_defs.h#4 $
 * $Change: 20526788 $
 */
#ifndef CPR_DEFS_H
#define CPR_DEFS_H

#include "comdef.h"

typedef enum
{
    CPR_VOLTAGE_MODE_OFF,
    CPR_VOLTAGE_MODE_RETENTION,
    CPR_VOLTAGE_MODE_MIN_SVS,
    CPR_VOLTAGE_MODE_LOW_SVS,
    CPR_VOLTAGE_MODE_LOW_SVS_L1,
    CPR_VOLTAGE_MODE_LOW_SVS_L2,
    CPR_VOLTAGE_MODE_SVS,
    CPR_VOLTAGE_MODE_SVS_L0,
    CPR_VOLTAGE_MODE_SVS_L1,
    CPR_VOLTAGE_MODE_SVS_L2,
    CPR_VOLTAGE_MODE_NOMINAL,
    CPR_VOLTAGE_MODE_NOMINAL_L1,
    CPR_VOLTAGE_MODE_NOMINAL_L2,
    CPR_VOLTAGE_MODE_TURBO,
    CPR_VOLTAGE_MODE_TURBO_L1,
    CPR_VOLTAGE_MODE_SUPER_TURBO,
    CPR_VOLTAGE_MODE_SUPER_TURBO_NO_CPR,

    CPR_VOLTAGE_MODE_COUNT
} cpr_voltage_mode;

typedef enum
{
    // Core rails
    CPR_RAIL_MX     = 0x100,
    CPR_RAIL_CX,
    CPR_RAIL_MSS,
    CPR_RAIL_GFX,
    CPR_RAIL_VDDA,
    CPR_RAIL_WLAN,
    CPR_RAIL_MM,

    // APC rails
    CPR_RAIL_APC0   = 0x200,
    CPR_RAIL_APC1,
    CPR_RAIL_APC2,
    CPR_RAIL_APC3,
    CPR_RAIL_APC4,
    CPR_RAIL_APC5,
    CPR_RAIL_CBF,
    CPR_RAIL_APCC_LVT,
    CPR_RAIL_APCC_SLVT,
    CPR_RAIL_APCC_M4M,

    // SSC rails
    CPR_RAIL_SSC_MX = 0x300,
    CPR_RAIL_SSC_CX,

    // LPI rails
    CPR_RAIL_LPI_MX = 0x400,
    CPR_RAIL_LPI_CX,

    CPR_RAIL_MSS_LDO = 0x500,
    CPR_RAIL_TURING_LDO,

    CPR_RAIL_WMSS_CX = 0x600
} cpr_domain_id;

typedef enum {
    CPR_DOMAIN_TYPE_MODE_BASED,
    CPR_DOMAIN_TYPE_FREQ_BASED
} cpr_domain_type;

typedef struct
{
    cpr_domain_type type;
    union
    {
        cpr_voltage_mode mode;
        uint32 freq;
    } u;
} cpr_domain_info;

typedef cpr_domain_id cpr_rail_id_t;
typedef cpr_voltage_mode cpr_voltage_mode_t;

#endif //CPR_DEFS_H
