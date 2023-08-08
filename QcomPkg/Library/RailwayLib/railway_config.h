/*===========================================================================

  railway_config.h - data related to platform-specific customization

  Copyright (c) 2012 Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RAILWAY_CONFIG_H
#define RAILWAY_CONFIG_H

#include "railway.h"
#include "railway_internal.h"
#include "pm_ldo.h"
#include "pm_smps.h"

typedef void (*railway_static_cb_type)(uint32);
typedef pm_err_flag_type (*pmic_volt_func_type)(uint8 pmic_chip, uint8 peripheral_index, pm_volt_level_type volt_level, boolean wait_for_settls);

typedef enum
{
    RAILWAY_RAIL_TYPE_MX = 0,
    RAILWAY_RAIL_TYPE_CX,
    RAILWAY_RAIL_TYPE_EBI,
    RAILWAY_RAIL_TYPE_LPI_MX,
    RAILWAY_RAIL_TYPE_LPI_CX,
    RAILWAY_RAIL_TYPE_WLAN,
    RAILWAY_RAIL_TYPES_COUNT,
} railway_rail_type_t;

typedef struct
{
    const char        *vreg_name;

    uint8             pmic_chip_id;
    uint8             pmic_peripheral_index;

    railway_corner    initial_corner; // Which corner to vote for out of the box (no votes received yet).
    uint32            default_retention_uv; // default retention voltage in microvolt

    pmic_volt_func_type pmic_volt_func; // pm_smps_volt_level or pm_ldo_volt_level
} railway_rail_config_t;

typedef struct
{
    int                    num_rails;
    railway_rail_config_t *rails;
} railway_config_data_t;

extern const railway_config_data_t * const RAILWAY_CONFIG_DATA;

#endif // RPM_CONFIG_H
