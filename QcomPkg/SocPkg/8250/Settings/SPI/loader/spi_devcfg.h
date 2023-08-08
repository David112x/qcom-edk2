/**
    @file  spi_devcfg.h
    @brief device configuration implementation
 */
/*=============================================================================
            Copyright (c) 2020 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_DEVCFG_H__
#define __SPI_DEVCFG_H__

#include "spi_config.h"
//#include "spi_driver.h"
//#include "spi_gpi.h"

//
// CONFIGURATION START ============================================
//

// enable the cores that are used on this image
//#define SSC_USE_SPI_15

// north [se0 - se7] [0:7]
#define QUPV3_0_CORE_BASE_ADDRESS        0x00900000
#define QUPV3_0_CORE_COMMON_BASE_ADDRESS 0x009C0000

// east [se0 - se5] [8:13]
#define QUPV3_1_CORE_BASE_ADDRESS        0x00A00000
#define QUPV3_1_CORE_COMMON_BASE_ADDRESS 0x00AC0000

// south [se0 - se5] [14:19]
#define QUPV3_2_CORE_BASE_ADDRESS        0x00800000
#define QUPV3_2_CORE_COMMON_BASE_ADDRESS 0x008C0000

#define TLMM_GPIO_CFG(gpio, func, dir, pull, drive) \
                          (((gpio) & 0x3FF) << 4  | \
                           ((func) & 0xF  ) << 0  | \
                           ((dir)  & 0x1  ) << 14 | \
                           ((pull) & 0x3  ) << 15 | \
                           ((drive)& 0xF  ) << 17)

#define TLMM_GPIO_INPUT      0x0
#define TLMM_GPIO_OUTPUT     0x1
#define TLMM_GPIO_NO_PULL    0x0
#define TLMM_GPIO_PULL_DOWN  0x1
#define TLMM_GPIO_PULL_UP    0x3
#define TLMM_GPIO_6MA        0x2


// set GPIOs for the defined cores
// TLMM_GPIO_CFG(gpio, func, dir, pull, drive)
#define SPI_01_MISO TLMM_GPIO_CFG(28,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_01_MOSI TLMM_GPIO_CFG(29,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_01_CLK  TLMM_GPIO_CFG(30,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_01_CS_0 TLMM_GPIO_CFG(31,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_01_MISO_SLEEP TLMM_GPIO_CFG(28,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_01_MOSI_SLEEP TLMM_GPIO_CFG(29,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_01_CLK_SLEEP  TLMM_GPIO_CFG(30,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_01_CS_0_SLEEP TLMM_GPIO_CFG(31,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_02_MISO TLMM_GPIO_CFG(4,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_02_MOSI TLMM_GPIO_CFG(5,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_02_CLK  TLMM_GPIO_CFG(6,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_02_CS_0 TLMM_GPIO_CFG(7,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_02_MISO_SLEEP TLMM_GPIO_CFG(4,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_02_MOSI_SLEEP TLMM_GPIO_CFG(5,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_02_CLK_SLEEP  TLMM_GPIO_CFG(6,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_02_CS_0_SLEEP TLMM_GPIO_CFG(7,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_03_MISO TLMM_GPIO_CFG(115,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_03_MOSI TLMM_GPIO_CFG(116,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_03_CLK  TLMM_GPIO_CFG(117,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_03_CS_0 TLMM_GPIO_CFG(118,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_03_MISO_SLEEP TLMM_GPIO_CFG(115,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_03_MOSI_SLEEP TLMM_GPIO_CFG(116,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_03_CLK_SLEEP  TLMM_GPIO_CFG(117,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_03_CS_0_SLEEP TLMM_GPIO_CFG(118,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_04_MISO TLMM_GPIO_CFG(119,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_04_MOSI TLMM_GPIO_CFG(120,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_04_CLK  TLMM_GPIO_CFG(121,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_04_CS_0 TLMM_GPIO_CFG(122,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_04_MISO_SLEEP TLMM_GPIO_CFG(119,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_04_MOSI_SLEEP TLMM_GPIO_CFG(120,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_04_CLK_SLEEP  TLMM_GPIO_CFG(121,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_04_CS_0_SLEEP TLMM_GPIO_CFG(122,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_05_MISO TLMM_GPIO_CFG(8,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_05_MOSI TLMM_GPIO_CFG(9,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_05_CLK  TLMM_GPIO_CFG(10, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_05_CS_0 TLMM_GPIO_CFG(11, 1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_05_MISO_SLEEP TLMM_GPIO_CFG(8,   0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_05_MOSI_SLEEP TLMM_GPIO_CFG(9,   0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_05_CLK_SLEEP  TLMM_GPIO_CFG(10,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_05_CS_0_SLEEP TLMM_GPIO_CFG(11,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_06_MISO TLMM_GPIO_CFG(12,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_06_MOSI TLMM_GPIO_CFG(13,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_06_CLK  TLMM_GPIO_CFG(14,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_06_CS_0 TLMM_GPIO_CFG(15,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_06_MISO_SLEEP TLMM_GPIO_CFG(12,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_06_MOSI_SLEEP TLMM_GPIO_CFG(13,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_06_CLK_SLEEP  TLMM_GPIO_CFG(14,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_06_CS_0_SLEEP TLMM_GPIO_CFG(15,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_07_MISO TLMM_GPIO_CFG(16,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_07_MOSI TLMM_GPIO_CFG(17,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_07_CLK  TLMM_GPIO_CFG(18,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_07_CS_0 TLMM_GPIO_CFG(19,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_07_MISO_SLEEP TLMM_GPIO_CFG(16,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_07_MOSI_SLEEP TLMM_GPIO_CFG(17,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_07_CLK_SLEEP  TLMM_GPIO_CFG(18,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_07_CS_0_SLEEP TLMM_GPIO_CFG(19,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_08_MISO TLMM_GPIO_CFG(20,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_08_MOSI TLMM_GPIO_CFG(21,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_08_CLK  TLMM_GPIO_CFG(22,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_08_CS_0 TLMM_GPIO_CFG(23,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_08_MISO_SLEEP TLMM_GPIO_CFG(20, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_08_MOSI_SLEEP TLMM_GPIO_CFG(21, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_08_CLK_SLEEP  TLMM_GPIO_CFG(22, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_08_CS_0_SLEEP TLMM_GPIO_CFG(23, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_09_MISO TLMM_GPIO_CFG(24,  2, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_09_MOSI TLMM_GPIO_CFG(25,  2, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_09_CLK  TLMM_GPIO_CFG(26,  2, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_09_CS_0 TLMM_GPIO_CFG(27,  2, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_09_MISO_SLEEP TLMM_GPIO_CFG(24,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_09_MOSI_SLEEP TLMM_GPIO_CFG(25,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_09_CLK_SLEEP  TLMM_GPIO_CFG(26,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_09_CS_0_SLEEP TLMM_GPIO_CFG(27,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_10_MISO TLMM_GPIO_CFG(125,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_10_MOSI TLMM_GPIO_CFG(126,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_10_CLK  TLMM_GPIO_CFG(127,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_10_CS_0 TLMM_GPIO_CFG(128,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_10_MISO_SLEEP TLMM_GPIO_CFG(125,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_10_MOSI_SLEEP TLMM_GPIO_CFG(126,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_10_CLK_SLEEP  TLMM_GPIO_CFG(127,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_10_CS_0_SLEEP TLMM_GPIO_CFG(128,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_11_MISO TLMM_GPIO_CFG(129,  2, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_11_MOSI TLMM_GPIO_CFG(130,  4, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_11_CLK  TLMM_GPIO_CFG(131,  2, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_11_CS_0 TLMM_GPIO_CFG(132,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_11_MISO_SLEEP TLMM_GPIO_CFG(129, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_11_MOSI_SLEEP TLMM_GPIO_CFG(130, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_11_CLK_SLEEP  TLMM_GPIO_CFG(131, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_11_CS_0_SLEEP TLMM_GPIO_CFG(132, 0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_12_MISO TLMM_GPIO_CFG(60,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_12_MOSI TLMM_GPIO_CFG(61,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_12_CLK  TLMM_GPIO_CFG(62,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_12_CS_0 TLMM_GPIO_CFG(63,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_12_MISO_SLEEP TLMM_GPIO_CFG(60,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_12_MOSI_SLEEP TLMM_GPIO_CFG(61,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_12_CLK_SLEEP  TLMM_GPIO_CFG(62,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_12_CS_0_SLEEP TLMM_GPIO_CFG(63,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_13_MISO TLMM_GPIO_CFG(32,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_13_MOSI TLMM_GPIO_CFG(33,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_13_CLK  TLMM_GPIO_CFG(34,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_13_CS_0 TLMM_GPIO_CFG(35,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_13_MISO_SLEEP TLMM_GPIO_CFG(32,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_13_MOSI_SLEEP TLMM_GPIO_CFG(33,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_13_CLK_SLEEP  TLMM_GPIO_CFG(34,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_13_CS_0_SLEEP TLMM_GPIO_CFG(35,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_14_MISO TLMM_GPIO_CFG(36,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_14_MOSI TLMM_GPIO_CFG(37,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_14_CLK  TLMM_GPIO_CFG(38,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_14_CS_0 TLMM_GPIO_CFG(39,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_14_MISO_SLEEP TLMM_GPIO_CFG(36,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_14_MOSI_SLEEP TLMM_GPIO_CFG(37,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_14_CLK_SLEEP  TLMM_GPIO_CFG(38,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_14_CS_0_SLEEP TLMM_GPIO_CFG(39,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_15_MISO TLMM_GPIO_CFG(40,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_15_MOSI TLMM_GPIO_CFG(41,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_15_CLK  TLMM_GPIO_CFG(42,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_15_CS_0 TLMM_GPIO_CFG(43,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_15_MISO_SLEEP TLMM_GPIO_CFG(40,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_15_MOSI_SLEEP TLMM_GPIO_CFG(41,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_15_CLK_SLEEP  TLMM_GPIO_CFG(42,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_15_CS_0_SLEEP TLMM_GPIO_CFG(43,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_16_MISO TLMM_GPIO_CFG(44,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_16_MOSI TLMM_GPIO_CFG(45,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_16_CLK  TLMM_GPIO_CFG(46,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_16_CS_0 TLMM_GPIO_CFG(47,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_16_MISO_SLEEP TLMM_GPIO_CFG(44,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_16_MOSI_SLEEP TLMM_GPIO_CFG(45,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_16_CLK_SLEEP  TLMM_GPIO_CFG(46,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_16_CS_0_SLEEP TLMM_GPIO_CFG(47,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_17_MISO TLMM_GPIO_CFG(48,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_17_MOSI TLMM_GPIO_CFG(49,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_17_CLK  TLMM_GPIO_CFG(50,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_17_CS_0 TLMM_GPIO_CFG(51,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_17_MISO_SLEEP TLMM_GPIO_CFG(48,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_17_MOSI_SLEEP TLMM_GPIO_CFG(49,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_17_CLK_SLEEP  TLMM_GPIO_CFG(50,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_17_CS_0_SLEEP TLMM_GPIO_CFG(51,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_18_MISO TLMM_GPIO_CFG(52,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_18_MOSI TLMM_GPIO_CFG(53,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_18_CLK  TLMM_GPIO_CFG(54,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_18_CS_0 TLMM_GPIO_CFG(55,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_18_MISO_SLEEP TLMM_GPIO_CFG(52,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_18_MOSI_SLEEP TLMM_GPIO_CFG(53,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_18_CLK_SLEEP  TLMM_GPIO_CFG(54,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_18_CS_0_SLEEP TLMM_GPIO_CFG(55,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_19_MISO TLMM_GPIO_CFG(56,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_19_MOSI TLMM_GPIO_CFG(57,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_19_CLK  TLMM_GPIO_CFG(58,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_19_CS_0 TLMM_GPIO_CFG(59,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_19_MISO_SLEEP TLMM_GPIO_CFG(56,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_19_MOSI_SLEEP TLMM_GPIO_CFG(57,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_19_CLK_SLEEP  TLMM_GPIO_CFG(58,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_19_CS_0_SLEEP TLMM_GPIO_CFG(59,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)

#define SPI_20_MISO TLMM_GPIO_CFG(0,  1, TLMM_GPIO_INPUT,  TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_20_MOSI TLMM_GPIO_CFG(1,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)
#define SPI_20_CLK  TLMM_GPIO_CFG(2,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_NO_PULL,   TLMM_GPIO_6MA)
#define SPI_20_CS_0 TLMM_GPIO_CFG(3,  1, TLMM_GPIO_OUTPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_6MA)

#define SPI_20_MISO_SLEEP TLMM_GPIO_CFG(0,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_20_MOSI_SLEEP TLMM_GPIO_CFG(1,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_20_CLK_SLEEP  TLMM_GPIO_CFG(2,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)
#define SPI_20_CS_0_SLEEP TLMM_GPIO_CFG(3,  0, TLMM_GPIO_INPUT, TLMM_GPIO_PULL_DOWN, TLMM_GPIO_2MA)



// CONFIGURATION END   ============================================
//
/*
#define TX_RING_ALIGN (MAX_TX_TRE_LIST_SIZE_PER_CORE << 4)
#define RX_RING_ALIGN (MAX_RX_TRE_LIST_SIZE_PER_CORE << 4)
*/

/*
#ifdef SSC_USE_SPI_01
static gpi_ring_elem gpi_rx_tre_list_base_01 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (TX_RING_ALIGN)));
static gpi_ring_elem gpi_tx_tre_list_base_01 [MAX_TX_TRE_LIST_SIZE_PER_CORE] __attribute__ ((aligned (RX_RING_ALIGN)));
#endif
*/

//#define NUM_SPI_CORES_IN_USE (sizeof(spi_device_config)/sizeof(spi_plat_device_config))

//static spi_plat_device_config spi_device_config[] =

const char *common_clock_str_0 [] =
{
    "gcc_qupv3_wrap0_core_2x_clk",
    "gcc_qupv3_wrap0_core_clk",
    "gcc_qupv3_wrap_0_s_ahb_clk",
    "gcc_qupv3_wrap_0_m_ahb_clk",
    NULL,
};

const char *common_clock_str_1 [] =
{
    "gcc_qupv3_wrap1_core_2x_clk",
    "gcc_qupv3_wrap1_core_clk",
    "gcc_qupv3_wrap_1_s_ahb_clk",
    "gcc_qupv3_wrap_1_m_ahb_clk",
    NULL,
};

const char *common_clock_str_2 [] =
{
    "gcc_qupv3_wrap2_core_2x_clk",
    "gcc_qupv3_wrap2_core_clk",
    "gcc_qupv3_wrap_2_s_ahb_clk",
    "gcc_qupv3_wrap_2_m_ahb_clk",
    NULL,
};

const char *se_clock_str_0 [] =
{
    "gcc_qupv3_wrap0_s0_clk",
    "gcc_qupv3_wrap0_s1_clk",
    "gcc_qupv3_wrap0_s2_clk",
    "gcc_qupv3_wrap0_s3_clk",
    "gcc_qupv3_wrap0_s4_clk",
    "gcc_qupv3_wrap0_s5_clk",
    "gcc_qupv3_wrap0_s6_clk",
    "gcc_qupv3_wrap0_s7_clk",
};

const char *se_clock_str_1 [] =
{
    "gcc_qupv3_wrap1_s0_clk",
    "gcc_qupv3_wrap1_s1_clk",
    "gcc_qupv3_wrap1_s2_clk",
    "gcc_qupv3_wrap1_s3_clk",
    "gcc_qupv3_wrap1_s4_clk",
    "gcc_qupv3_wrap1_s5_clk",
};

const char *se_clock_str_2 [] =
{
    "gcc_qupv3_wrap2_s0_clk",
    "gcc_qupv3_wrap2_s1_clk",
    "gcc_qupv3_wrap2_s2_clk",
    "gcc_qupv3_wrap2_s3_clk",
    "gcc_qupv3_wrap2_s4_clk",
    "gcc_qupv3_wrap2_s5_clk",
};

spi_plat_clock_config clock_cfg[] = 
{
	{19200000  , 0 , 0 } ,
		
	{32000000  , 0 , 0 } ,
	
	{38400000  , 0 , 0 } ,
	
	{48000000  , 0 , 0 } ,
	
	{64000000  , 0 , 0 } ,
	
	{96000000  , 0 , 0 } ,
	
	{100000000 , 0 , 0 } ,
};	
									
#ifdef SSC_USE_SPI_01
spi_plat_device_config spi_device_config_01 =
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00080000,
        .core_index                  = 0,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_01_MISO,
        .mosi_encoding                = SPI_01_MOSI,
        .clk_encoding                 = SPI_01_CLK,
        .cs_encoding                  = SPI_01_CS_0,
        
        .sleep_miso_encoding          = SPI_01_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_01_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_01_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_01_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_0,
		.se_clock                    = (uint8 **) (se_clock_str_0 + 0),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_02
spi_plat_device_config spi_device_config_02 =
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00084000,
        .core_index                  = 1,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_02_MISO,
        .mosi_encoding                = SPI_02_MOSI,
        .clk_encoding                 = SPI_02_CLK,
        .cs_encoding                  = SPI_02_CS_0,
        
        .sleep_miso_encoding          = SPI_02_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_02_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_02_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_02_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_0,
		.se_clock                    = (uint8 **) (se_clock_str_0 + 1),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_03
spi_plat_device_config spi_device_config_03 =
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00088000,
        .core_index                  = 2,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_03_MISO,
        .mosi_encoding                = SPI_03_MOSI,
        .clk_encoding                 = SPI_03_CLK,
        .cs_encoding                  = SPI_03_CS_0,
        
        .sleep_miso_encoding          = SPI_03_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_03_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_04_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_04_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_0,
		.se_clock                    = (uint8 **) (se_clock_str_0 + 2),
		.clock_config				 = clock_cfg,

};
#endif
#ifdef SSC_USE_SPI_04
spi_plat_device_config spi_device_config_04 =
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x0008C000,
        .core_index                  = 3,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_04_MISO,
        .mosi_encoding                = SPI_04_MOSI,
        .clk_encoding                 = SPI_04_CLK,
        .cs_encoding                  = SPI_04_CS_0,
        
        .sleep_miso_encoding          = SPI_04_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_04_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_04_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_04_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_0,
		.se_clock                    = (uint8 **) (se_clock_str_0 + 3),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_05
spi_plat_device_config spi_device_config_05 =
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00090000,
        .core_index                  = 4,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_05_MISO,
        .mosi_encoding                = SPI_05_MOSI,
        .clk_encoding                 = SPI_05_CLK,
        .cs_encoding                  = SPI_05_CS_0,
        
        .sleep_miso_encoding          = SPI_05_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_05_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_05_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_05_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_0,
		.se_clock                    = (uint8 **) (se_clock_str_0 + 4),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_06
spi_plat_device_config spi_device_config_06 =
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00094000,
        .core_index                  = 5,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_06_MISO,
        .mosi_encoding                = SPI_06_MOSI,
        .clk_encoding                 = SPI_06_CLK,
        .cs_encoding                  = SPI_06_CS_0,
        
        .sleep_miso_encoding          = SPI_06_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_06_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_06_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_06_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_0,
		.se_clock                    = (uint8 **) (se_clock_str_0 + 5),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_07
spi_plat_device_config spi_device_config_07 =
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00098000,
        .core_index                  = 6,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_07_MISO,
        .mosi_encoding                = SPI_07_MOSI,
        .clk_encoding                 = SPI_07_CLK,
        .cs_encoding                  = SPI_07_CS_0,
        
        .sleep_miso_encoding          = SPI_07_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_07_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_07_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_07_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_0,
		.se_clock                    = (uint8 **) (se_clock_str_0 + 6),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_08
spi_plat_device_config spi_device_config_08 =
    {
        .core_base_addr              = (uint8 *) QUPV3_0_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_0_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x0009C000,
        .core_index                  = 7,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_08_MISO,
        .mosi_encoding                = SPI_08_MOSI,
        .clk_encoding                 = SPI_08_CLK,
        .cs_encoding                  = SPI_08_CS_0,
        
        .sleep_miso_encoding          = SPI_08_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_08_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_08_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_08_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_0,
		.se_clock                    = (uint8 **) (se_clock_str_0 + 7),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_09
spi_plat_device_config spi_device_config_09 =
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00080000,
        .core_index                  = 8,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_09_MISO,
        .mosi_encoding                = SPI_09_MOSI,
        .clk_encoding                 = SPI_09_CLK,
        .cs_encoding                  = SPI_09_CS_0,
        
        .sleep_miso_encoding          = SPI_01_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_01_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_01_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_01_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_1,
		.se_clock                    = (uint8 **) (se_clock_str_1 + 0),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_10
spi_plat_device_config spi_device_config_10 =
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00084000,
        .core_index                  = 9,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_10_MISO,
        .mosi_encoding                = SPI_10_MOSI,
        .clk_encoding                 = SPI_10_CLK,
        .cs_encoding                  = SPI_10_CS_0,
        
        .sleep_miso_encoding          = SPI_10_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_10_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_10_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_10_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_1,
		.se_clock                    = (uint8 **) (se_clock_str_1 + 1),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_11
spi_plat_device_config spi_device_config_11 =
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00088000,
        .core_index                  = 10,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_11_MISO,
        .mosi_encoding                = SPI_11_MOSI,
        .clk_encoding                 = SPI_11_CLK,
        .cs_encoding                  = SPI_11_CS_0,
        
        .sleep_miso_encoding          = SPI_11_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_11_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_11_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_11_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_1,
		.se_clock                    = (uint8 **) (se_clock_str_1 + 2),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_12
spi_plat_device_config spi_device_config_12 =
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x0008C000,
        .core_index                  = 11,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_12_MISO,
        .mosi_encoding                = SPI_12_MOSI,
        .clk_encoding                 = SPI_12_CLK,
        .cs_encoding                  = SPI_12_CS_0,
        
        .sleep_miso_encoding          = SPI_12_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_12_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_12_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_12_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_1,
		.se_clock                    = (uint8 **) (se_clock_str_1 + 3),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_13
spi_plat_device_config spi_device_config_13 =
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00090000,
        .core_index                  = 12,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_13_MISO,
        .mosi_encoding                = SPI_13_MOSI,
        .clk_encoding                 = SPI_13_CLK,
        .cs_encoding                  = SPI_13_CS_0,
        
        .sleep_miso_encoding          = SPI_13_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_13_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_13_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_13_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_1,
		.se_clock                    = (uint8 **) (se_clock_str_1 + 4),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_14
spi_plat_device_config spi_device_config_14 =
    {
        .core_base_addr              = (uint8 *) QUPV3_1_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_1_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00094000,
        .core_index                  = 13,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_14_MISO,
        .mosi_encoding                = SPI_14_MOSI,
        .clk_encoding                 = SPI_14_CLK,
        .cs_encoding                  = SPI_14_CS_0,
        
        .sleep_miso_encoding          = SPI_14_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_14_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_14_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_14_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_1,
		.se_clock                    = (uint8 **) (se_clock_str_1 + 5),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_15
spi_plat_device_config spi_device_config_15 =
    {
        .core_base_addr              = (uint8 *) QUPV3_2_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_2_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00080000,
        .core_index                  = 14,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_15_MISO,
        .mosi_encoding                = SPI_15_MOSI,
        .clk_encoding                 = SPI_15_CLK,
        .cs_encoding                  = SPI_15_CS_0,
        
        .sleep_miso_encoding          = SPI_15_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_15_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_15_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_15_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_2,
		.se_clock                    = (uint8 **) (se_clock_str_2 + 0),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_16
spi_plat_device_config spi_device_config_16 =
    {
        .core_base_addr              = (uint8 *) QUPV3_2_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_2_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00084000,
        .core_index                  = 15,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_16_MISO,
        .mosi_encoding                = SPI_16_MOSI,
        .clk_encoding                 = SPI_16_CLK,
        .cs_encoding                  = SPI_16_CS_0,
        
        .sleep_miso_encoding          = SPI_16_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_16_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_16_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_16_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_2,
		.se_clock                    = (uint8 **) (se_clock_str_2 + 1),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_17
spi_plat_device_config spi_device_config_17 =
    {
        .core_base_addr              = (uint8 *) QUPV3_2_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_2_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00088000,
        .core_index                  = 16,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_17_MISO,
        .mosi_encoding                = SPI_17_MOSI,
        .clk_encoding                 = SPI_17_CLK,
        .cs_encoding                  = SPI_17_CS_0,
        
        .sleep_miso_encoding          = SPI_17_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_17_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_17_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_17_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_2,
		.se_clock                    = (uint8 **) (se_clock_str_2 + 2),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_18
spi_plat_device_config spi_device_config_18 =
    {
        .core_base_addr              = (uint8 *) QUPV3_2_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_2_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x0008C000,
        .core_index                  = 17,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_18_MISO,
        .mosi_encoding                = SPI_18_MOSI,
        .clk_encoding                 = SPI_18_CLK,
        .cs_encoding                  = SPI_18_CS_0,
        
        .sleep_miso_encoding          = SPI_18_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_18_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_18_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_18_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_2,
		.se_clock                    = (uint8 **) (se_clock_str_2 + 3),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_19
spi_plat_device_config spi_device_config_19 =
    {
        .core_base_addr              = (uint8 *) QUPV3_2_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_2_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00090000,
        .core_index                  = 18,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_19_MISO,
        .mosi_encoding                = SPI_19_MOSI,
        .clk_encoding                 = SPI_19_CLK,
        .cs_encoding                  = SPI_19_CS_0,
        
        .sleep_miso_encoding          = SPI_19_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_19_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_19_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_19_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_2,
		.se_clock                    = (uint8 **) (se_clock_str_2 + 4),
		.clock_config				 = clock_cfg,

};
#endif

#ifdef SSC_USE_SPI_20
spi_plat_device_config spi_device_config_20 =
    {
        .core_base_addr              = (uint8 *) QUPV3_2_CORE_BASE_ADDRESS,
		.common_base_addr			 = (uint8 *) QUPV3_2_CORE_COMMON_BASE_ADDRESS,
        .core_offset                 = 0x00094000,
        .core_index                  = 19,
        .core_irq                    = 0, //not used, we use GSI callback
        .polled_mode                 = TRUE,
        .min_data_length_for_dma     = 0,

        .gpi_index                   = 0, //See "GPII_Mappings_Sample.xlsx"
        .se_clock_dfs_index          = 1,
        .se_clock_frequency          = 19200,

        .miso_encoding                = SPI_20_MISO,
        .mosi_encoding                = SPI_20_MOSI,
        .clk_encoding                 = SPI_20_CLK,
        .cs_encoding                  = SPI_20_CS_0,
        
        .sleep_miso_encoding          = SPI_20_MISO_SLEEP,
        .sleep_mosi_encoding          = SPI_20_MOSI_SLEEP,
        .sleep_clk_encoding           = SPI_20_CLK_SLEEP,
        .sleep_cs_encoding            = SPI_20_CS_0_SLEEP,

        .tcsr_base_addr              = (uint8 *) 0x01FC0000,
        .tcsr_reg_offset             = 0x00000000,
        .tcsr_reg_value              = 0x00000000,
		.common_clocks               = (uint8 **) common_clock_str_2,
		.se_clock                    = (uint8 **) (se_clock_str_2 + 5),
		.clock_config				 = clock_cfg,

};
#endif
/*
#define NUM_SPI_CLIENTS 4

static spi_hw_ctxt          spi_bus_hw_ctxt [NUM_SPI_CORES_IN_USE];
//static qurt_mutex_t         spi_bus_crit_sec[NUM_SPI_CORES_IN_USE];
//static qurt_signal_t        spi_bus_signal[NUM_SPI_CORES_IN_USE];

static spi_gpi_ctxt         spi_bus_gpi_ctxt[NUM_SPI_CORES_IN_USE];
static spi_client_ctxt      spi_bus_client_ctxt [NUM_SPI_CORES_IN_USE * NUM_SPI_CLIENTS];
static spi_gpi_io_ctxt      spi_bus_gpi_io_ctxt [NUM_SPI_CORES_IN_USE * NUM_SPI_CLIENTS];
*/
#endif /*__SPI_DEVCFG_H__*/
