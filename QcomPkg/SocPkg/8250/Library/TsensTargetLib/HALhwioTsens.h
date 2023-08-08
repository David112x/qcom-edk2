#ifndef __HALHWIOTSENS_H__
#define __HALHWIOTSENS_H__
/*
===========================================================================
*/
/**
  @file HALhwioTsens.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SM8250 (Kona) [kona_v1.0_p3q2r17.8_MTO]
 
  This file contains HWIO register definitions for the following modules:
    TSENS0
    TSENS0_TM


  Generation parameters: 
  { u'explicit-addressing': True,
    u'filename': u'HALhwioTsens.h',
    u'ignore-prefixes': True,
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'TSENS0', u'TSENS0_TM'],
    u'output-fvals': True}
*/
/*
  ===========================================================================

  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

  ===========================================================================

  $Header: //components/rel/boot.xf/3.2/QcomPkg/SocPkg/8250/Library/TsensTargetLib/HALhwioTsens.h#4 $
  $DateTime: 2019/01/15 16:40:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: TSENS0
 *--------------------------------------------------------------------------*/

#define TSENS0_REG_BASE                                                            (AOSS_BASE      + 0x01222000)
#define TSENS0_REG_BASE_SIZE                                                       0x1000
#define TSENS0_REG_BASE_USED                                                       0x148

#define HWIO_TSENS_HW_VER_ADDR(x)                                                  ((x) + 0x00000000)
#define HWIO_TSENS_HW_VER_RMSK                                                     0xffffffff
#define HWIO_TSENS_HW_VER_IN(x)      \
        in_dword(HWIO_TSENS_HW_VER_ADDR(x))
#define HWIO_TSENS_HW_VER_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_HW_VER_ADDR(x), m)
#define HWIO_TSENS_HW_VER_MAJOR_BMSK                                               0xf0000000
#define HWIO_TSENS_HW_VER_MAJOR_SHFT                                                     0x1c
#define HWIO_TSENS_HW_VER_MINOR_BMSK                                                0xfff0000
#define HWIO_TSENS_HW_VER_MINOR_SHFT                                                     0x10
#define HWIO_TSENS_HW_VER_STEP_BMSK                                                    0xffff
#define HWIO_TSENS_HW_VER_STEP_SHFT                                                       0x0

#define HWIO_TSENS_CTRL_ADDR(x)                                                    ((x) + 0x00000004)
#define HWIO_TSENS_CTRL_RMSK                                                       0x7fffffff
#define HWIO_TSENS_CTRL_IN(x)      \
        in_dword(HWIO_TSENS_CTRL_ADDR(x))
#define HWIO_TSENS_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CTRL_ADDR(x), m)
#define HWIO_TSENS_CTRL_OUT(x, v)      \
        out_dword(HWIO_TSENS_CTRL_ADDR(x),v)
#define HWIO_TSENS_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_CTRL_ADDR(x),m,v,HWIO_TSENS_CTRL_IN(x))
#define HWIO_TSENS_CTRL_INTER_SENSOR_DELAY_EN_BMSK                                 0x40000000
#define HWIO_TSENS_CTRL_INTER_SENSOR_DELAY_EN_SHFT                                       0x1e
#define HWIO_TSENS_CTRL_INTER_SENSOR_DELAY_EN_DISABLED_FVAL                               0x0
#define HWIO_TSENS_CTRL_INTER_SENSOR_DELAY_EN_ENABLED_FVAL                                0x1
#define HWIO_TSENS_CTRL_MAX_TEMP_PWM_EN_BMSK                                       0x20000000
#define HWIO_TSENS_CTRL_MAX_TEMP_PWM_EN_SHFT                                             0x1d
#define HWIO_TSENS_CTRL_MAX_TEMP_PWM_EN_DISABLED_FVAL                                     0x0
#define HWIO_TSENS_CTRL_MAX_TEMP_PWM_EN_ENABLED_FVAL                                      0x1
#define HWIO_TSENS_CTRL_VALID_DELAY_BMSK                                           0x1e000000
#define HWIO_TSENS_CTRL_VALID_DELAY_SHFT                                                 0x19
#define HWIO_TSENS_CTRL_PSHOLD_ARES_EN_BMSK                                         0x1000000
#define HWIO_TSENS_CTRL_PSHOLD_ARES_EN_SHFT                                              0x18
#define HWIO_TSENS_CTRL_PSHOLD_ARES_EN_DISABLED_FVAL                                      0x0
#define HWIO_TSENS_CTRL_PSHOLD_ARES_EN_ENABLED_FVAL                                       0x1
#define HWIO_TSENS_CTRL_TEMP_BROADCAST_EN_BMSK                                       0x800000
#define HWIO_TSENS_CTRL_TEMP_BROADCAST_EN_SHFT                                           0x17
#define HWIO_TSENS_CTRL_TEMP_BROADCAST_EN_DISABLED_FVAL                                   0x0
#define HWIO_TSENS_CTRL_TEMP_BROADCAST_EN_ENABLED_FVAL                                    0x1
#define HWIO_TSENS_CTRL_AUTO_ADJUST_PERIOD_EN_BMSK                                   0x400000
#define HWIO_TSENS_CTRL_AUTO_ADJUST_PERIOD_EN_SHFT                                       0x16
#define HWIO_TSENS_CTRL_AUTO_ADJUST_PERIOD_EN_DISABLED_FVAL                               0x0
#define HWIO_TSENS_CTRL_AUTO_ADJUST_PERIOD_EN_ENABLED_FVAL                                0x1
#define HWIO_TSENS_CTRL_RESULT_FORMAT_CODE_OR_TEMP_BMSK                              0x200000
#define HWIO_TSENS_CTRL_RESULT_FORMAT_CODE_OR_TEMP_SHFT                                  0x15
#define HWIO_TSENS_CTRL_RESULT_FORMAT_CODE_OR_TEMP_ADC_CODE_FVAL                          0x0
#define HWIO_TSENS_CTRL_RESULT_FORMAT_CODE_OR_TEMP_REAL_TEMPERATURE_FVAL                  0x1
#define HWIO_TSENS_CTRL_TSENS_CLAMP_BMSK                                             0x100000
#define HWIO_TSENS_CTRL_TSENS_CLAMP_SHFT                                                 0x14
#define HWIO_TSENS_CTRL_TSENS_CLAMP_UNCLAMPED_FVAL                                        0x0
#define HWIO_TSENS_CTRL_TSENS_CLAMP_CLAMPED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_TSENS_BYPASS_EN_BMSK                                          0x80000
#define HWIO_TSENS_CTRL_TSENS_BYPASS_EN_SHFT                                             0x13
#define HWIO_TSENS_CTRL_TSENS_BYPASS_EN_DISABLED_FVAL                                     0x0
#define HWIO_TSENS_CTRL_TSENS_BYPASS_EN_ENABLED_FVAL                                      0x1
#define HWIO_TSENS_CTRL_SENSOR15_EN_BMSK                                              0x40000
#define HWIO_TSENS_CTRL_SENSOR15_EN_SHFT                                                 0x12
#define HWIO_TSENS_CTRL_SENSOR15_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR15_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR14_EN_BMSK                                              0x20000
#define HWIO_TSENS_CTRL_SENSOR14_EN_SHFT                                                 0x11
#define HWIO_TSENS_CTRL_SENSOR14_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR14_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR13_EN_BMSK                                              0x10000
#define HWIO_TSENS_CTRL_SENSOR13_EN_SHFT                                                 0x10
#define HWIO_TSENS_CTRL_SENSOR13_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR13_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR12_EN_BMSK                                               0x8000
#define HWIO_TSENS_CTRL_SENSOR12_EN_SHFT                                                  0xf
#define HWIO_TSENS_CTRL_SENSOR12_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR12_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR11_EN_BMSK                                               0x4000
#define HWIO_TSENS_CTRL_SENSOR11_EN_SHFT                                                  0xe
#define HWIO_TSENS_CTRL_SENSOR11_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR11_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR10_EN_BMSK                                               0x2000
#define HWIO_TSENS_CTRL_SENSOR10_EN_SHFT                                                  0xd
#define HWIO_TSENS_CTRL_SENSOR10_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR10_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR9_EN_BMSK                                                0x1000
#define HWIO_TSENS_CTRL_SENSOR9_EN_SHFT                                                   0xc
#define HWIO_TSENS_CTRL_SENSOR9_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR9_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR8_EN_BMSK                                                 0x800
#define HWIO_TSENS_CTRL_SENSOR8_EN_SHFT                                                   0xb
#define HWIO_TSENS_CTRL_SENSOR8_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR8_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR7_EN_BMSK                                                 0x400
#define HWIO_TSENS_CTRL_SENSOR7_EN_SHFT                                                   0xa
#define HWIO_TSENS_CTRL_SENSOR7_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR7_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR6_EN_BMSK                                                 0x200
#define HWIO_TSENS_CTRL_SENSOR6_EN_SHFT                                                   0x9
#define HWIO_TSENS_CTRL_SENSOR6_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR6_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR5_EN_BMSK                                                 0x100
#define HWIO_TSENS_CTRL_SENSOR5_EN_SHFT                                                   0x8
#define HWIO_TSENS_CTRL_SENSOR5_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR5_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR4_EN_BMSK                                                  0x80
#define HWIO_TSENS_CTRL_SENSOR4_EN_SHFT                                                   0x7
#define HWIO_TSENS_CTRL_SENSOR4_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR4_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR3_EN_BMSK                                                  0x40
#define HWIO_TSENS_CTRL_SENSOR3_EN_SHFT                                                   0x6
#define HWIO_TSENS_CTRL_SENSOR3_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR3_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR2_EN_BMSK                                                  0x20
#define HWIO_TSENS_CTRL_SENSOR2_EN_SHFT                                                   0x5
#define HWIO_TSENS_CTRL_SENSOR2_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR2_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR1_EN_BMSK                                                  0x10
#define HWIO_TSENS_CTRL_SENSOR1_EN_SHFT                                                   0x4
#define HWIO_TSENS_CTRL_SENSOR1_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR1_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR0_EN_BMSK                                                   0x8
#define HWIO_TSENS_CTRL_SENSOR0_EN_SHFT                                                   0x3
#define HWIO_TSENS_CTRL_SENSOR0_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR0_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_TSENS_ADC_CLK_SEL_BMSK                                            0x4
#define HWIO_TSENS_CTRL_TSENS_ADC_CLK_SEL_SHFT                                            0x2
#define HWIO_TSENS_CTRL_TSENS_ADC_CLK_SEL_INTERNAL_OSCILLATOR_FVAL                        0x0
#define HWIO_TSENS_CTRL_TSENS_ADC_CLK_SEL_EXTERNAL_CLOCK_SOURCE_FVAL                      0x1
#define HWIO_TSENS_CTRL_TSENS_SW_RST_BMSK                                                 0x2
#define HWIO_TSENS_CTRL_TSENS_SW_RST_SHFT                                                 0x1
#define HWIO_TSENS_CTRL_TSENS_SW_RST_RESET_DEASSERTED_FVAL                                0x0
#define HWIO_TSENS_CTRL_TSENS_SW_RST_RESET_ASSERTED_FVAL                                  0x1
#define HWIO_TSENS_CTRL_TSENS_EN_BMSK                                                     0x1
#define HWIO_TSENS_CTRL_TSENS_EN_SHFT                                                     0x0
#define HWIO_TSENS_CTRL_TSENS_EN_DISABLED_FVAL                                            0x0
#define HWIO_TSENS_CTRL_TSENS_EN_ENABLED_FVAL                                             0x1

#define HWIO_TSENS_MEASURE_PERIOD_ADDR(x)                                          ((x) + 0x00000008)
#define HWIO_TSENS_MEASURE_PERIOD_RMSK                                              0xfffffff
#define HWIO_TSENS_MEASURE_PERIOD_IN(x)      \
        in_dword(HWIO_TSENS_MEASURE_PERIOD_ADDR(x))
#define HWIO_TSENS_MEASURE_PERIOD_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_MEASURE_PERIOD_ADDR(x), m)
#define HWIO_TSENS_MEASURE_PERIOD_OUT(x, v)      \
        out_dword(HWIO_TSENS_MEASURE_PERIOD_ADDR(x),v)
#define HWIO_TSENS_MEASURE_PERIOD_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_MEASURE_PERIOD_ADDR(x),m,v,HWIO_TSENS_MEASURE_PERIOD_IN(x))
#define HWIO_TSENS_MEASURE_PERIOD_TBCB_CLK_STRETCH_CMUX_DELAY_BMSK                  0xfc00000
#define HWIO_TSENS_MEASURE_PERIOD_TBCB_CLK_STRETCH_CMUX_DELAY_SHFT                       0x16
#define HWIO_TSENS_MEASURE_PERIOD_TBCB_SSCTX_CLK_STRETCH_CNTR_BMSK                   0x3f0000
#define HWIO_TSENS_MEASURE_PERIOD_TBCB_SSCTX_CLK_STRETCH_CNTR_SHFT                       0x10
#define HWIO_TSENS_MEASURE_PERIOD_POWERDOWN_MEASURE_PERIOD_BMSK                        0xff00
#define HWIO_TSENS_MEASURE_PERIOD_POWERDOWN_MEASURE_PERIOD_SHFT                           0x8
#define HWIO_TSENS_MEASURE_PERIOD_MAIN_MEASURE_PERIOD_BMSK                               0xff
#define HWIO_TSENS_MEASURE_PERIOD_MAIN_MEASURE_PERIOD_SHFT                                0x0

#define HWIO_TSENS_TEST_CTRL_ADDR(x)                                               ((x) + 0x0000000c)
#define HWIO_TSENS_TEST_CTRL_RMSK                                                  0xffff0fff
#define HWIO_TSENS_TEST_CTRL_IN(x)      \
        in_dword(HWIO_TSENS_TEST_CTRL_ADDR(x))
#define HWIO_TSENS_TEST_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TEST_CTRL_ADDR(x), m)
#define HWIO_TSENS_TEST_CTRL_OUT(x, v)      \
        out_dword(HWIO_TSENS_TEST_CTRL_ADDR(x),v)
#define HWIO_TSENS_TEST_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_TEST_CTRL_ADDR(x),m,v,HWIO_TSENS_TEST_CTRL_IN(x))
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_SEL_EXT_BMSK                               0xffff0000
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_SEL_EXT_SHFT                                     0x10
#define HWIO_TSENS_TEST_CTRL_TSENS_RO_SELECT_OVR_BMSK                                   0x800
#define HWIO_TSENS_TEST_CTRL_TSENS_RO_SELECT_OVR_SHFT                                     0xb
#define HWIO_TSENS_TEST_CTRL_TSENS_RO_ENABLE_OVR_BMSK                                   0x400
#define HWIO_TSENS_TEST_CTRL_TSENS_RO_ENABLE_OVR_SHFT                                     0xa
#define HWIO_TSENS_TEST_CTRL_CM_DFT_ENABLE_BMSK                                         0x200
#define HWIO_TSENS_TEST_CTRL_CM_DFT_ENABLE_SHFT                                           0x9
#define HWIO_TSENS_TEST_CTRL_BYPASS_DIST_SEL_BMSK                                       0x1e0
#define HWIO_TSENS_TEST_CTRL_BYPASS_DIST_SEL_SHFT                                         0x5
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_SEL_BMSK                                         0x1e
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_SEL_SHFT                                          0x1
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_EN_BMSK                                           0x1
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_EN_SHFT                                           0x0
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_DISABLED_FVAL                             0x0
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_ENABLED_FVAL                              0x1

#define HWIO_TSENS_MAX_MIN_INT_STATUS_ADDR(x)                                      ((x) + 0x00000010)
#define HWIO_TSENS_MAX_MIN_INT_STATUS_RMSK                                         0xffffffff
#define HWIO_TSENS_MAX_MIN_INT_STATUS_IN(x)      \
        in_dword(HWIO_TSENS_MAX_MIN_INT_STATUS_ADDR(x))
#define HWIO_TSENS_MAX_MIN_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_MAX_MIN_INT_STATUS_ADDR(x), m)
#define HWIO_TSENS_MAX_MIN_INT_STATUS_MAX_INT_STATUS_BMSK                          0xffff0000
#define HWIO_TSENS_MAX_MIN_INT_STATUS_MAX_INT_STATUS_SHFT                                0x10
#define HWIO_TSENS_MAX_MIN_INT_STATUS_MIN_INT_STATUS_BMSK                              0xffff
#define HWIO_TSENS_MAX_MIN_INT_STATUS_MIN_INT_STATUS_SHFT                                 0x0

#define HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x)                                       ((x) + 0x00000014)
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_RMSK                                          0xffffffff
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_IN(x)      \
        in_dword(HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x))
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x), m)
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_OUT(x, v)      \
        out_dword(HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x),v)
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x),m,v,HWIO_TSENS_MAX_MIN_INT_CLEAR_IN(x))
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_MAX_INT_CLEAR_BMSK                            0xffff0000
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_MAX_INT_CLEAR_SHFT                                  0x10
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_MIN_INT_CLEAR_BMSK                                0xffff
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_MIN_INT_CLEAR_SHFT                                   0x0

#define HWIO_TSENS_0C_CTRL_ADDR(x)                                                 ((x) + 0x00000018)
#define HWIO_TSENS_0C_CTRL_RMSK                                                       0x1ffff
#define HWIO_TSENS_0C_CTRL_IN(x)      \
        in_dword(HWIO_TSENS_0C_CTRL_ADDR(x))
#define HWIO_TSENS_0C_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_0C_CTRL_ADDR(x), m)
#define HWIO_TSENS_0C_CTRL_OUT(x, v)      \
        out_dword(HWIO_TSENS_0C_CTRL_ADDR(x),v)
#define HWIO_TSENS_0C_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_0C_CTRL_ADDR(x),m,v,HWIO_TSENS_0C_CTRL_IN(x))
#define HWIO_TSENS_0C_CTRL_TSENS_0C_SENSOR_EN_BMSK                                    0x1fffe
#define HWIO_TSENS_0C_CTRL_TSENS_0C_SENSOR_EN_SHFT                                        0x1
#define HWIO_TSENS_0C_CTRL_TSENS_0C_INTERRUPT_EN_BMSK                                     0x1
#define HWIO_TSENS_0C_CTRL_TSENS_0C_INTERRUPT_EN_SHFT                                     0x0

#define HWIO_TSENS_0C_THRESHOLDS_ADDR(x)                                           ((x) + 0x0000001c)
#define HWIO_TSENS_0C_THRESHOLDS_RMSK                                                0xffffff
#define HWIO_TSENS_0C_THRESHOLDS_IN(x)      \
        in_dword(HWIO_TSENS_0C_THRESHOLDS_ADDR(x))
#define HWIO_TSENS_0C_THRESHOLDS_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_0C_THRESHOLDS_ADDR(x), m)
#define HWIO_TSENS_0C_THRESHOLDS_OUT(x, v)      \
        out_dword(HWIO_TSENS_0C_THRESHOLDS_ADDR(x),v)
#define HWIO_TSENS_0C_THRESHOLDS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_0C_THRESHOLDS_ADDR(x),m,v,HWIO_TSENS_0C_THRESHOLDS_IN(x))
#define HWIO_TSENS_0C_THRESHOLDS_THRESHOLD_SET_BMSK                                  0xfff000
#define HWIO_TSENS_0C_THRESHOLDS_THRESHOLD_SET_SHFT                                       0xc
#define HWIO_TSENS_0C_THRESHOLDS_THRESHOLD_RESET_BMSK                                   0xfff
#define HWIO_TSENS_0C_THRESHOLDS_THRESHOLD_RESET_SHFT                                     0x0

#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n)                             ((base) + 0x00000020 + 0x4 * (n))
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_RMSK                                      0x3ffffff
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAXn                                             15
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n), HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_RMSK)
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n),val)
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n),mask,val,HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_INI(base,n))
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_STATUS_MASK_BMSK                      0x2000000
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_STATUS_MASK_SHFT                           0x19
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_STATUS_MASK_NORMAL_OPERATION_FVAL           0x0
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_STATUS_MASK_MASK_OFF_MAX_STATUS_FVAL        0x1
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_STATUS_MASK_BMSK                      0x1000000
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_STATUS_MASK_SHFT                           0x18
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_STATUS_MASK_NORMAL_OPERATION_FVAL           0x0
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_STATUS_MASK_MASK_OFF_MIN_STATUS_FVAL        0x1
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_THRESHOLD_BMSK                         0xfff000
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_THRESHOLD_SHFT                              0xc
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_THRESHOLD_BMSK                            0xfff
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_THRESHOLD_SHFT                              0x0

#define HWIO_TSENS_Sn_CONVERSION_ADDR(base,n)                                      ((base) + 0x00000060 + 0x4 * (n))
#define HWIO_TSENS_Sn_CONVERSION_RMSK                                               0x1ffffff
#define HWIO_TSENS_Sn_CONVERSION_MAXn                                                      15
#define HWIO_TSENS_Sn_CONVERSION_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_CONVERSION_ADDR(base,n), HWIO_TSENS_Sn_CONVERSION_RMSK)
#define HWIO_TSENS_Sn_CONVERSION_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_CONVERSION_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_CONVERSION_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_Sn_CONVERSION_ADDR(base,n),val)
#define HWIO_TSENS_Sn_CONVERSION_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_Sn_CONVERSION_ADDR(base,n),mask,val,HWIO_TSENS_Sn_CONVERSION_INI(base,n))
#define HWIO_TSENS_Sn_CONVERSION_SHIFT_BMSK                                         0x1800000
#define HWIO_TSENS_Sn_CONVERSION_SHIFT_SHFT                                              0x17
#define HWIO_TSENS_Sn_CONVERSION_SLOPE_BMSK                                          0x7ffc00
#define HWIO_TSENS_Sn_CONVERSION_SLOPE_SHFT                                               0xa
#define HWIO_TSENS_Sn_CONVERSION_CZERO_BMSK                                             0x3ff
#define HWIO_TSENS_Sn_CONVERSION_CZERO_SHFT                                               0x0

#define HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000a0)
#define HWIO_TSENS_S0_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S0_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S0_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S0_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S0_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S0_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S0_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S0_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000a4)
#define HWIO_TSENS_S1_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S1_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S1_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S1_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S1_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S1_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S1_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S1_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000a8)
#define HWIO_TSENS_S2_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S2_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S2_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S2_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S2_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S2_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S2_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S2_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000ac)
#define HWIO_TSENS_S3_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S3_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S3_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S3_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S3_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S3_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S3_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S3_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000b0)
#define HWIO_TSENS_S4_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S4_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S4_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S4_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S4_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S4_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S4_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S4_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000b4)
#define HWIO_TSENS_S5_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S5_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S5_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S5_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S5_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S5_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S5_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S5_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000b8)
#define HWIO_TSENS_S6_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S6_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S6_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S6_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S6_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S6_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S6_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S6_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000bc)
#define HWIO_TSENS_S7_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S7_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S7_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S7_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S7_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S7_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S7_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S7_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000c0)
#define HWIO_TSENS_S8_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S8_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S8_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S8_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S8_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S8_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S8_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S8_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000c4)
#define HWIO_TSENS_S9_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S9_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S9_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S9_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S9_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S9_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S9_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S9_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000c8)
#define HWIO_TSENS_S10_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S10_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S10_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S10_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S10_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S10_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S10_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S10_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000cc)
#define HWIO_TSENS_S11_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S11_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S11_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S11_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S11_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S11_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S11_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S11_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000d0)
#define HWIO_TSENS_S12_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S12_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S12_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S12_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S12_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S12_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S12_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S12_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000d4)
#define HWIO_TSENS_S13_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S13_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S13_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S13_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S13_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S13_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S13_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S13_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000d8)
#define HWIO_TSENS_S14_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S14_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S14_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S14_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S14_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S14_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S14_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S14_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000dc)
#define HWIO_TSENS_S15_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S15_ID_ASSIGNMENT_IN(x)      \
        in_dword(HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x))
#define HWIO_TSENS_S15_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S15_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S15_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S15_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S15_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S15_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TS_CONTROL_ADDR(x)                                                    ((x) + 0x000000e0)
#define HWIO_TS_CONTROL_RMSK                                                       0xffffffff
#define HWIO_TS_CONTROL_IN(x)      \
        in_dword(HWIO_TS_CONTROL_ADDR(x))
#define HWIO_TS_CONTROL_INM(x, m)      \
        in_dword_masked(HWIO_TS_CONTROL_ADDR(x), m)
#define HWIO_TS_CONTROL_OUT(x, v)      \
        out_dword(HWIO_TS_CONTROL_ADDR(x),v)
#define HWIO_TS_CONTROL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TS_CONTROL_ADDR(x),m,v,HWIO_TS_CONTROL_IN(x))
#define HWIO_TS_CONTROL_TS_CONTROL_BMSK                                            0xffffffff
#define HWIO_TS_CONTROL_TS_CONTROL_SHFT                                                   0x0

#define HWIO_TS_CONFIG_ADDR(x)                                                     ((x) + 0x000000e4)
#define HWIO_TS_CONFIG_RMSK                                                        0xffffffff
#define HWIO_TS_CONFIG_IN(x)      \
        in_dword(HWIO_TS_CONFIG_ADDR(x))
#define HWIO_TS_CONFIG_INM(x, m)      \
        in_dword_masked(HWIO_TS_CONFIG_ADDR(x), m)
#define HWIO_TS_CONFIG_OUT(x, v)      \
        out_dword(HWIO_TS_CONFIG_ADDR(x),v)
#define HWIO_TS_CONFIG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TS_CONFIG_ADDR(x),m,v,HWIO_TS_CONFIG_IN(x))
#define HWIO_TS_CONFIG_TS_CONFIG_BMSK                                              0xffffffff
#define HWIO_TS_CONFIG_TS_CONFIG_SHFT                                                     0x0

#define HWIO_TSENS_SIDEBAND_EN_ADDR(x)                                             ((x) + 0x000000e8)
#define HWIO_TSENS_SIDEBAND_EN_RMSK                                                    0xffff
#define HWIO_TSENS_SIDEBAND_EN_IN(x)      \
        in_dword(HWIO_TSENS_SIDEBAND_EN_ADDR(x))
#define HWIO_TSENS_SIDEBAND_EN_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_SIDEBAND_EN_ADDR(x), m)
#define HWIO_TSENS_SIDEBAND_EN_OUT(x, v)      \
        out_dword(HWIO_TSENS_SIDEBAND_EN_ADDR(x),v)
#define HWIO_TSENS_SIDEBAND_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_SIDEBAND_EN_ADDR(x),m,v,HWIO_TSENS_SIDEBAND_EN_IN(x))
#define HWIO_TSENS_SIDEBAND_EN_SENSOR_EN_BMSK                                          0xffff
#define HWIO_TSENS_SIDEBAND_EN_SENSOR_EN_SHFT                                             0x0

#define HWIO_TSENS_TBCB_CONTROL_ADDR(x)                                            ((x) + 0x000000ec)
#define HWIO_TSENS_TBCB_CONTROL_RMSK                                               0xffffffff
#define HWIO_TSENS_TBCB_CONTROL_IN(x)      \
        in_dword(HWIO_TSENS_TBCB_CONTROL_ADDR(x))
#define HWIO_TSENS_TBCB_CONTROL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TBCB_CONTROL_ADDR(x), m)
#define HWIO_TSENS_TBCB_CONTROL_OUT(x, v)      \
        out_dword(HWIO_TSENS_TBCB_CONTROL_ADDR(x),v)
#define HWIO_TSENS_TBCB_CONTROL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_TBCB_CONTROL_ADDR(x),m,v,HWIO_TSENS_TBCB_CONTROL_IN(x))
#define HWIO_TSENS_TBCB_CONTROL_TBCB_ACK_DELAY_BMSK                                0xff000000
#define HWIO_TSENS_TBCB_CONTROL_TBCB_ACK_DELAY_SHFT                                      0x18
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLK_DIV_BMSK                                    0xff0000
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLK_DIV_SHFT                                        0x10
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLIENT_EN_BMSK                                    0xffff
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLIENT_EN_SHFT                                       0x0
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLIENT_EN_DISABLED_FVAL                              0x0
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLIENT_EN_ENABLED_FVAL                               0x1

#define HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n)                                  ((base) + 0x000000f0 + 0x4 * (n))
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_RMSK                                              0xffff
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_MAXn                                                  15
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n), HWIO_TSENS_TBCB_CLIENT_n_REQ_RMSK)
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n), mask)
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n),val)
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n),mask,val,HWIO_TSENS_TBCB_CLIENT_n_REQ_INI(base,n))
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_TBCB_CLIENT_REQ_SENSOR_BMSK                       0xffff
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_TBCB_CLIENT_REQ_SENSOR_SHFT                          0x0

#define HWIO_TSENS_TBCB_OVERRIDE_ADDR(x)                                           ((x) + 0x00000130)
#define HWIO_TSENS_TBCB_OVERRIDE_RMSK                                                 0x3ffff
#define HWIO_TSENS_TBCB_OVERRIDE_IN(x)      \
        in_dword(HWIO_TSENS_TBCB_OVERRIDE_ADDR(x))
#define HWIO_TSENS_TBCB_OVERRIDE_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TBCB_OVERRIDE_ADDR(x), m)
#define HWIO_TSENS_TBCB_OVERRIDE_OUT(x, v)      \
        out_dword(HWIO_TSENS_TBCB_OVERRIDE_ADDR(x),v)
#define HWIO_TSENS_TBCB_OVERRIDE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_TBCB_OVERRIDE_ADDR(x),m,v,HWIO_TSENS_TBCB_OVERRIDE_IN(x))
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_DATA_BMSK                              0x3fffc
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_DATA_SHFT                                  0x2
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_CMD_BMSK                                   0x2
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_CMD_SHFT                                   0x1
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_EN_BMSK                                    0x1
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_EN_SHFT                                    0x0

#define HWIO_TSENS_WATCHDOG_CTRL_ADDR(x)                                           ((x) + 0x00000140)
#define HWIO_TSENS_WATCHDOG_CTRL_RMSK                                                 0xfffff
#define HWIO_TSENS_WATCHDOG_CTRL_IN(x)      \
        in_dword(HWIO_TSENS_WATCHDOG_CTRL_ADDR(x))
#define HWIO_TSENS_WATCHDOG_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_WATCHDOG_CTRL_ADDR(x), m)
#define HWIO_TSENS_WATCHDOG_CTRL_OUT(x, v)      \
        out_dword(HWIO_TSENS_WATCHDOG_CTRL_ADDR(x),v)
#define HWIO_TSENS_WATCHDOG_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_WATCHDOG_CTRL_ADDR(x),m,v,HWIO_TSENS_WATCHDOG_CTRL_IN(x))
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_DISABLE_KNOB_BMSK                           0x80000
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_DISABLE_KNOB_SHFT                              0x13
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_DISABLE_KNOB_TBCB_ONLY_FVAL                     0x0
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_DISABLE_KNOB_MAIN_TSENS_EN_FVAL                 0x1
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_SELF_RESET_BMSK                             0x40000
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_SELF_RESET_SHFT                                0x12
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_LOAD_VAL_BMSK                               0x3fffc
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_LOAD_VAL_SHFT                                   0x2
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_TRIGGER_STATE_BMSK                              0x2
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_TRIGGER_STATE_SHFT                              0x1
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_TRIGGER_STATE_IDLE_FVAL                         0x0
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_TRIGGER_STATE_GLOBAL_MAX_FVAL                   0x1
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_EN_BMSK                                         0x1
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_EN_SHFT                                         0x0

#define HWIO_TSENS_SERVER_CFG_ADDR(x)                                              ((x) + 0x00000144)
#define HWIO_TSENS_SERVER_CFG_RMSK                                                        0xf
#define HWIO_TSENS_SERVER_CFG_IN(x)      \
        in_dword(HWIO_TSENS_SERVER_CFG_ADDR(x))
#define HWIO_TSENS_SERVER_CFG_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_SERVER_CFG_ADDR(x), m)
#define HWIO_TSENS_SERVER_CFG_OUT(x, v)      \
        out_dword(HWIO_TSENS_SERVER_CFG_ADDR(x),v)
#define HWIO_TSENS_SERVER_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_SERVER_CFG_ADDR(x),m,v,HWIO_TSENS_SERVER_CFG_IN(x))
#define HWIO_TSENS_SERVER_CFG_SLP_CLK_XO_DIV_COUNT_BMSK                                   0xf
#define HWIO_TSENS_SERVER_CFG_SLP_CLK_XO_DIV_COUNT_SHFT                                   0x0

#define HWIO_TSENS_RESERVED_REG_ADDR(x)                                            ((x) + 0x00000148)
#define HWIO_TSENS_RESERVED_REG_RMSK                                               0xffffffff
#define HWIO_TSENS_RESERVED_REG_IN(x)      \
        in_dword(HWIO_TSENS_RESERVED_REG_ADDR(x))
#define HWIO_TSENS_RESERVED_REG_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_RESERVED_REG_ADDR(x), m)
#define HWIO_TSENS_RESERVED_REG_OUT(x, v)      \
        out_dword(HWIO_TSENS_RESERVED_REG_ADDR(x),v)
#define HWIO_TSENS_RESERVED_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_RESERVED_REG_ADDR(x),m,v,HWIO_TSENS_RESERVED_REG_IN(x))
#define HWIO_TSENS_RESERVED_REG_RSRVED_REG_1_BMSK                                  0xffff0000
#define HWIO_TSENS_RESERVED_REG_RSRVED_REG_1_SHFT                                        0x10
#define HWIO_TSENS_RESERVED_REG_RSRVED_REG_0_BMSK                                      0xffff
#define HWIO_TSENS_RESERVED_REG_RSRVED_REG_0_SHFT                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: TSENS0_TM
 *--------------------------------------------------------------------------*/

#define TSENS0_TM_REG_BASE                                                           (AOSS_BASE      + 0x01263000)
#define TSENS0_TM_REG_BASE_SIZE                                                      0x1000
#define TSENS0_TM_REG_BASE_USED                                                      0x13c

#define HWIO_TSENS_CONTROLLER_ID_ADDR(x)                                             ((x) + 0x00000000)
#define HWIO_TSENS_CONTROLLER_ID_RMSK                                                      0x1f
#define HWIO_TSENS_CONTROLLER_ID_IN(x)      \
        in_dword(HWIO_TSENS_CONTROLLER_ID_ADDR(x))
#define HWIO_TSENS_CONTROLLER_ID_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CONTROLLER_ID_ADDR(x), m)
#define HWIO_TSENS_CONTROLLER_ID_CONTROLLER_ID_BMSK                                        0x1f
#define HWIO_TSENS_CONTROLLER_ID_CONTROLLER_ID_SHFT                                         0x0

#define HWIO_TSENS_TM_INT_EN_ADDR(x)                                                 ((x) + 0x00000004)
#define HWIO_TSENS_TM_INT_EN_RMSK                                                           0x7
#define HWIO_TSENS_TM_INT_EN_IN(x)      \
        in_dword(HWIO_TSENS_TM_INT_EN_ADDR(x))
#define HWIO_TSENS_TM_INT_EN_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TM_INT_EN_ADDR(x), m)
#define HWIO_TSENS_TM_INT_EN_OUT(x, v)      \
        out_dword(HWIO_TSENS_TM_INT_EN_ADDR(x),v)
#define HWIO_TSENS_TM_INT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_TM_INT_EN_ADDR(x),m,v,HWIO_TSENS_TM_INT_EN_IN(x))
#define HWIO_TSENS_TM_INT_EN_CRITICAL_INT_EN_BMSK                                           0x4
#define HWIO_TSENS_TM_INT_EN_CRITICAL_INT_EN_SHFT                                           0x2
#define HWIO_TSENS_TM_INT_EN_CRITICAL_INT_EN_DISABLED_FVAL                                  0x0
#define HWIO_TSENS_TM_INT_EN_CRITICAL_INT_EN_ENABLED_FVAL                                   0x1
#define HWIO_TSENS_TM_INT_EN_UPPER_INT_EN_BMSK                                              0x2
#define HWIO_TSENS_TM_INT_EN_UPPER_INT_EN_SHFT                                              0x1
#define HWIO_TSENS_TM_INT_EN_UPPER_INT_EN_DISABLED_FVAL                                     0x0
#define HWIO_TSENS_TM_INT_EN_UPPER_INT_EN_ENABLED_FVAL                                      0x1
#define HWIO_TSENS_TM_INT_EN_LOWER_INT_EN_BMSK                                              0x1
#define HWIO_TSENS_TM_INT_EN_LOWER_INT_EN_SHFT                                              0x0
#define HWIO_TSENS_TM_INT_EN_LOWER_INT_EN_DISABLED_FVAL                                     0x0
#define HWIO_TSENS_TM_INT_EN_LOWER_INT_EN_ENABLED_FVAL                                      0x1

#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_ADDR(x)                                    ((x) + 0x00000008)
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_RMSK                                       0xffffffff
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_IN(x)      \
        in_dword(HWIO_TSENS_UPPER_LOWER_INT_STATUS_ADDR(x))
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_UPPER_LOWER_INT_STATUS_ADDR(x), m)
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_UPPER_INT_STATUS_BMSK                      0xffff0000
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_UPPER_INT_STATUS_SHFT                            0x10
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_LOWER_INT_STATUS_BMSK                          0xffff
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_LOWER_INT_STATUS_SHFT                             0x0

#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x)                                     ((x) + 0x0000000c)
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_RMSK                                        0xffffffff
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_IN(x)      \
        in_dword(HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x))
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x), m)
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_OUT(x, v)      \
        out_dword(HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x),v)
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x),m,v,HWIO_TSENS_UPPER_LOWER_INT_CLEAR_IN(x))
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_UPPER_INT_CLEAR_BMSK                        0xffff0000
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_UPPER_INT_CLEAR_SHFT                              0x10
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_LOWER_INT_CLEAR_BMSK                            0xffff
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_LOWER_INT_CLEAR_SHFT                               0x0

#define HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x)                                      ((x) + 0x00000010)
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_RMSK                                         0xffffffff
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_IN(x)      \
        in_dword(HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x))
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x), m)
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_OUT(x, v)      \
        out_dword(HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x),v)
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x),m,v,HWIO_TSENS_UPPER_LOWER_INT_MASK_IN(x))
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_UPPER_INT_MASK_BMSK                          0xffff0000
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_UPPER_INT_MASK_SHFT                                0x10
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_LOWER_INT_MASK_BMSK                              0xffff
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_LOWER_INT_MASK_SHFT                                 0x0

#define HWIO_TSENS_CRITICAL_INT_STATUS_ADDR(x)                                       ((x) + 0x00000014)
#define HWIO_TSENS_CRITICAL_INT_STATUS_RMSK                                          0xc000ffff
#define HWIO_TSENS_CRITICAL_INT_STATUS_IN(x)      \
        in_dword(HWIO_TSENS_CRITICAL_INT_STATUS_ADDR(x))
#define HWIO_TSENS_CRITICAL_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CRITICAL_INT_STATUS_ADDR(x), m)
#define HWIO_TSENS_CRITICAL_INT_STATUS_WATCHDOG_BARK_BMSK                            0x80000000
#define HWIO_TSENS_CRITICAL_INT_STATUS_WATCHDOG_BARK_SHFT                                  0x1f
#define HWIO_TSENS_CRITICAL_INT_STATUS_CYCLE_COMPLETION_MONITOR_BMSK                 0x40000000
#define HWIO_TSENS_CRITICAL_INT_STATUS_CYCLE_COMPLETION_MONITOR_SHFT                       0x1e
#define HWIO_TSENS_CRITICAL_INT_STATUS_CRITICAL_INT_STATUS_BMSK                          0xffff
#define HWIO_TSENS_CRITICAL_INT_STATUS_CRITICAL_INT_STATUS_SHFT                             0x0

#define HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x)                                        ((x) + 0x00000018)
#define HWIO_TSENS_CRITICAL_INT_CLEAR_RMSK                                           0xc000ffff
#define HWIO_TSENS_CRITICAL_INT_CLEAR_IN(x)      \
        in_dword(HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x))
#define HWIO_TSENS_CRITICAL_INT_CLEAR_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x), m)
#define HWIO_TSENS_CRITICAL_INT_CLEAR_OUT(x, v)      \
        out_dword(HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x),v)
#define HWIO_TSENS_CRITICAL_INT_CLEAR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x),m,v,HWIO_TSENS_CRITICAL_INT_CLEAR_IN(x))
#define HWIO_TSENS_CRITICAL_INT_CLEAR_WATCHDOG_BARK_BMSK                             0x80000000
#define HWIO_TSENS_CRITICAL_INT_CLEAR_WATCHDOG_BARK_SHFT                                   0x1f
#define HWIO_TSENS_CRITICAL_INT_CLEAR_CYCLE_COMPLETION_MONITOR_BMSK                  0x40000000
#define HWIO_TSENS_CRITICAL_INT_CLEAR_CYCLE_COMPLETION_MONITOR_SHFT                        0x1e
#define HWIO_TSENS_CRITICAL_INT_CLEAR_CRITICAL_INT_CLEAR_BMSK                            0xffff
#define HWIO_TSENS_CRITICAL_INT_CLEAR_CRITICAL_INT_CLEAR_SHFT                               0x0

#define HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x)                                         ((x) + 0x0000001c)
#define HWIO_TSENS_CRITICAL_INT_MASK_RMSK                                            0xc000ffff
#define HWIO_TSENS_CRITICAL_INT_MASK_IN(x)      \
        in_dword(HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x))
#define HWIO_TSENS_CRITICAL_INT_MASK_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x), m)
#define HWIO_TSENS_CRITICAL_INT_MASK_OUT(x, v)      \
        out_dword(HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x),v)
#define HWIO_TSENS_CRITICAL_INT_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x),m,v,HWIO_TSENS_CRITICAL_INT_MASK_IN(x))
#define HWIO_TSENS_CRITICAL_INT_MASK_WATCHDOG_BARK_BMSK                              0x80000000
#define HWIO_TSENS_CRITICAL_INT_MASK_WATCHDOG_BARK_SHFT                                    0x1f
#define HWIO_TSENS_CRITICAL_INT_MASK_CYCLE_COMPLETION_MONITOR_BMSK                   0x40000000
#define HWIO_TSENS_CRITICAL_INT_MASK_CYCLE_COMPLETION_MONITOR_SHFT                         0x1e
#define HWIO_TSENS_CRITICAL_INT_MASK_CRITICAL_INT_MASK_BMSK                              0xffff
#define HWIO_TSENS_CRITICAL_INT_MASK_CRITICAL_INT_MASK_SHFT                                 0x0

#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n)                             ((base) + 0x00000020 + 0x4 * (n))
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_RMSK                                       0xffffff
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_MAXn                                             15
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n), HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_RMSK)
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n),val)
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n),mask,val,HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_INI(base,n))
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_UPPER_THRESHOLD_BMSK                       0xfff000
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_UPPER_THRESHOLD_SHFT                            0xc
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_LOWER_THRESHOLD_BMSK                          0xfff
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_LOWER_THRESHOLD_SHFT                            0x0

#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n)                                ((base) + 0x00000060 + 0x4 * (n))
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_RMSK                                             0xfff
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_MAXn                                                15
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n), HWIO_TSENS_Sn_CRITICAL_THRESHOLD_RMSK)
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n),val)
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n),mask,val,HWIO_TSENS_Sn_CRITICAL_THRESHOLD_INI(base,n))
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_CRITICAL_THRESHOLD_BMSK                          0xfff
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_CRITICAL_THRESHOLD_SHFT                            0x0

#define HWIO_TSENS_0C_INT_STATUS_ADDR(x)                                             ((x) + 0x000000e0)
#define HWIO_TSENS_0C_INT_STATUS_RMSK                                                       0x1
#define HWIO_TSENS_0C_INT_STATUS_IN(x)      \
        in_dword(HWIO_TSENS_0C_INT_STATUS_ADDR(x))
#define HWIO_TSENS_0C_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_0C_INT_STATUS_ADDR(x), m)
#define HWIO_TSENS_0C_INT_STATUS_INT_STATUS_BMSK                                            0x1
#define HWIO_TSENS_0C_INT_STATUS_INT_STATUS_SHFT                                            0x0

#define HWIO_TSENS_MIN_TEMP_ADDR(x)                                                  ((x) + 0x000000ec)
#define HWIO_TSENS_MIN_TEMP_RMSK                                                        0x1ffff
#define HWIO_TSENS_MIN_TEMP_IN(x)      \
        in_dword(HWIO_TSENS_MIN_TEMP_ADDR(x))
#define HWIO_TSENS_MIN_TEMP_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_MIN_TEMP_ADDR(x), m)
#define HWIO_TSENS_MIN_TEMP_MIN_TEMP_VALID_BMSK                                         0x10000
#define HWIO_TSENS_MIN_TEMP_MIN_TEMP_VALID_SHFT                                            0x10
#define HWIO_TSENS_MIN_TEMP_MIN_TEMP_SENSOR_ID_BMSK                                      0xf000
#define HWIO_TSENS_MIN_TEMP_MIN_TEMP_SENSOR_ID_SHFT                                         0xc
#define HWIO_TSENS_MIN_TEMP_MIN_TEMP_BMSK                                                 0xfff
#define HWIO_TSENS_MIN_TEMP_MIN_TEMP_SHFT                                                   0x0

#define HWIO_TSENS_Sn_STATUS_ADDR(base,n)                                            ((base) + 0x000000a0 + 0x4 * (n))
#define HWIO_TSENS_Sn_STATUS_RMSK                                                     0x3ffffff
#define HWIO_TSENS_Sn_STATUS_MAXn                                                            15
#define HWIO_TSENS_Sn_STATUS_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_STATUS_ADDR(base,n), HWIO_TSENS_Sn_STATUS_RMSK)
#define HWIO_TSENS_Sn_STATUS_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_STATUS_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_STATUS_SENSOR_ID_BMSK                                           0x3c00000
#define HWIO_TSENS_Sn_STATUS_SENSOR_ID_SHFT                                                0x16
#define HWIO_TSENS_Sn_STATUS_VALID_BMSK                                                0x200000
#define HWIO_TSENS_Sn_STATUS_VALID_SHFT                                                    0x15
#define HWIO_TSENS_Sn_STATUS_MAX_STATUS_BMSK                                           0x100000
#define HWIO_TSENS_Sn_STATUS_MAX_STATUS_SHFT                                               0x14
#define HWIO_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_NOT_VIOLATED_FVAL                     0x0
#define HWIO_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_VIOLATED_FVAL                         0x1
#define HWIO_TSENS_Sn_STATUS_CRITICAL_STATUS_BMSK                                       0x80000
#define HWIO_TSENS_Sn_STATUS_CRITICAL_STATUS_SHFT                                          0x13
#define HWIO_TSENS_Sn_STATUS_CRITICAL_STATUS_CRITICAL_THRESHOLD_NOT_VIOLATED_FVAL           0x0
#define HWIO_TSENS_Sn_STATUS_CRITICAL_STATUS_CRITICAL_THRESHOLD_VIOLATED_FVAL               0x1
#define HWIO_TSENS_Sn_STATUS_UPPER_STATUS_BMSK                                          0x40000
#define HWIO_TSENS_Sn_STATUS_UPPER_STATUS_SHFT                                             0x12
#define HWIO_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_NOT_VIOLATED_FVAL                 0x0
#define HWIO_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_VIOLATED_FVAL                     0x1
#define HWIO_TSENS_Sn_STATUS_LOWER_STATUS_BMSK                                          0x20000
#define HWIO_TSENS_Sn_STATUS_LOWER_STATUS_SHFT                                             0x11
#define HWIO_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_NOT_VIOLATED_FVAL                 0x0
#define HWIO_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_VIOLATED_FVAL                     0x1
#define HWIO_TSENS_Sn_STATUS_MIN_STATUS_BMSK                                            0x10000
#define HWIO_TSENS_Sn_STATUS_MIN_STATUS_SHFT                                               0x10
#define HWIO_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_NOT_VIOLATED_FVAL                     0x0
#define HWIO_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_VIOLATED_FVAL                         0x1
#define HWIO_TSENS_Sn_STATUS_LAST_TEMP_BMSK                                              0xffff
#define HWIO_TSENS_Sn_STATUS_LAST_TEMP_SHFT                                                 0x0

#define HWIO_TSENS_MAX_TEMP_ADDR(x)                                                  ((x) + 0x000000e8)
#define HWIO_TSENS_MAX_TEMP_RMSK                                                        0x1ffff
#define HWIO_TSENS_MAX_TEMP_IN(x)      \
        in_dword(HWIO_TSENS_MAX_TEMP_ADDR(x))
#define HWIO_TSENS_MAX_TEMP_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_MAX_TEMP_ADDR(x), m)
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_VALID_BMSK                                         0x10000
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_VALID_SHFT                                            0x10
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_SENSOR_ID_BMSK                                      0xf000
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_SENSOR_ID_SHFT                                         0xc
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_BMSK                                                 0xfff
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_SHFT                                                   0x0

#define HWIO_TSENS_TRDY_ADDR(x)                                                      ((x) + 0x000000e4)
#define HWIO_TSENS_TRDY_RMSK                                                                0xf
#define HWIO_TSENS_TRDY_IN(x)      \
        in_dword(HWIO_TSENS_TRDY_ADDR(x))
#define HWIO_TSENS_TRDY_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TRDY_ADDR(x), m)
#define HWIO_TSENS_TRDY_FIRST_ROUND_COMPLETE_BMSK                                           0x8
#define HWIO_TSENS_TRDY_FIRST_ROUND_COMPLETE_SHFT                                           0x3
#define HWIO_TSENS_TRDY_OSC_CLK_OFF_BMSK                                                    0x4
#define HWIO_TSENS_TRDY_OSC_CLK_OFF_SHFT                                                    0x2
#define HWIO_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_ON_FVAL                                          0x0
#define HWIO_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_OFF_FVAL                                         0x1
#define HWIO_TSENS_TRDY_SLP_CLK_OFF_BMSK                                                    0x2
#define HWIO_TSENS_TRDY_SLP_CLK_OFF_SHFT                                                    0x1
#define HWIO_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_ON_FVAL                                          0x0
#define HWIO_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_OFF_FVAL                                         0x1
#define HWIO_TSENS_TRDY_TRDY_BMSK                                                           0x1
#define HWIO_TSENS_TRDY_TRDY_SHFT                                                           0x0
#define HWIO_TSENS_TRDY_TRDY_TEMPERATURE_MEASUREMENT_IN_PROGRESS_FVAL                       0x0
#define HWIO_TSENS_TRDY_TRDY_TEMPERATURE_READING_IS_READY_FVAL                              0x1

#define HWIO_TSENS_DEBUG_CONTROL_ADDR(x)                                             ((x) + 0x00000130)
#define HWIO_TSENS_DEBUG_CONTROL_RMSK                                                   0x7ffff
#define HWIO_TSENS_DEBUG_CONTROL_IN(x)      \
        in_dword(HWIO_TSENS_DEBUG_CONTROL_ADDR(x))
#define HWIO_TSENS_DEBUG_CONTROL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_DEBUG_CONTROL_ADDR(x), m)
#define HWIO_TSENS_DEBUG_CONTROL_OUT(x, v)      \
        out_dword(HWIO_TSENS_DEBUG_CONTROL_ADDR(x),v)
#define HWIO_TSENS_DEBUG_CONTROL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_DEBUG_CONTROL_ADDR(x),m,v,HWIO_TSENS_DEBUG_CONTROL_IN(x))
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_ID_BMSK                                0x7ff80
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_ID_SHFT                                    0x7
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_SLV_DEBUG_BUS_EN_BMSK                          0x40
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_SLV_DEBUG_BUS_EN_SHFT                           0x6
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_SLV_DEBUG_BUS_EN_DISABLED_FVAL                  0x0
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_SLV_DEBUG_BUS_EN_ENABLED_FVAL                   0x1
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_MSTR_DEBUG_BUS_EN_BMSK                         0x20
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_MSTR_DEBUG_BUS_EN_SHFT                          0x5
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_MSTR_DEBUG_BUS_EN_DISABLED_FVAL                 0x0
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_MSTR_DEBUG_BUS_EN_ENABLED_FVAL                  0x1
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_SEL_BMSK                                  0x1e
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_SEL_SHFT                                   0x1
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_EN_BMSK                                    0x1
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_EN_SHFT                                    0x0
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_EN_DISABLED_FVAL                           0x0
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_EN_ENABLED_FVAL                            0x1

#define HWIO_TSENS_DEBUG_READ_ADDR(x)                                                ((x) + 0x00000134)
#define HWIO_TSENS_DEBUG_READ_RMSK                                                   0xffffffff
#define HWIO_TSENS_DEBUG_READ_IN(x)      \
        in_dword(HWIO_TSENS_DEBUG_READ_ADDR(x))
#define HWIO_TSENS_DEBUG_READ_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_DEBUG_READ_ADDR(x), m)
#define HWIO_TSENS_DEBUG_READ_DEBUG_DATA_READ_BMSK                                   0xffffffff
#define HWIO_TSENS_DEBUG_READ_DEBUG_DATA_READ_SHFT                                          0x0

#define HWIO_TSENS_QDSS_CONTROL_ADDR(x)                                              ((x) + 0x00000138)
#define HWIO_TSENS_QDSS_CONTROL_RMSK                                                     0xffff
#define HWIO_TSENS_QDSS_CONTROL_IN(x)      \
        in_dword(HWIO_TSENS_QDSS_CONTROL_ADDR(x))
#define HWIO_TSENS_QDSS_CONTROL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_QDSS_CONTROL_ADDR(x), m)
#define HWIO_TSENS_QDSS_CONTROL_OUT(x, v)      \
        out_dword(HWIO_TSENS_QDSS_CONTROL_ADDR(x),v)
#define HWIO_TSENS_QDSS_CONTROL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_QDSS_CONTROL_ADDR(x),m,v,HWIO_TSENS_QDSS_CONTROL_IN(x))
#define HWIO_TSENS_QDSS_CONTROL_QDSS_SENSOR_EN_BMSK                                      0xffff
#define HWIO_TSENS_QDSS_CONTROL_QDSS_SENSOR_EN_SHFT                                         0x0

#define HWIO_TSENS_WATCHDOG_LOG_ADDR(x)                                              ((x) + 0x0000013c)
#define HWIO_TSENS_WATCHDOG_LOG_RMSK                                                       0xff
#define HWIO_TSENS_WATCHDOG_LOG_IN(x)      \
        in_dword(HWIO_TSENS_WATCHDOG_LOG_ADDR(x))
#define HWIO_TSENS_WATCHDOG_LOG_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_WATCHDOG_LOG_ADDR(x), m)
#define HWIO_TSENS_WATCHDOG_LOG_WATCHDOG_BARK_COUNT_BMSK                                   0xff
#define HWIO_TSENS_WATCHDOG_LOG_WATCHDOG_BARK_COUNT_SHFT                                    0x0


#endif /* __HALHWIOTSENS_H__ */
