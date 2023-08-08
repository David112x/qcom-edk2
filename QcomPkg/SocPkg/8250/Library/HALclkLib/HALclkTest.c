/*
==============================================================================

FILE:         HALclkTest.c

DESCRIPTION:
  Auto-generated file containing custom debug mux control functions.

==============================================================================

$Header: //components/rel/boot.xf/3.2/QcomPkg/SocPkg/8250/Library/HALclkLib/HALclkTest.c#5 $

==============================================================================
            Copyright (c) 2019 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================

*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "HALclkInternal.h"
#include "HALclkHWIO.h"
#include "HALhwio.h"

/*=========================================================================
      Definitions
==========================================================================*/

#define HAL_CLK_FRQ_MEASURE_CNT_SHORT  0x0100
#define HAL_CLK_FRQ_MEASURE_CNT_LONG   0x8000


/*=========================================================================
      APSS_CC Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxAPSSCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32_t                   nMuxSel
)
{
  HWIO_OUTM(APSS_GLB_CLK_DIAG, 0x7F0, nMuxSel << 4);
  HWIO_OUTF(APSS_GLB_CLK_DIAG, DBG_CDIV, 0);
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlAPSSCC =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxAPSSCC,
};


/*=========================================================================
      CAM_CC Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxCAMCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32_t                   nMuxSel
)
{
  HWIO_OUTF(CAM_CC_DEBUG_MUX_MUXR, MUX_SEL, nMuxSel);
  HWIO_OUTF(CAM_CC_DEBUG_DIV_CDIVR, CLK_DIV, 3);
  HWIO_OUTF(CAM_CC_DEBUG_CBCR, CLK_ENABLE, 1);
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlCAMCC =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxCAMCC,
};


/*=========================================================================
      DISP_CC Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxDISPCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32_t                   nMuxSel
)
{
  HWIO_OUTF(DISP_CC_DEBUG_MUX_MUXR, MUX_SEL, nMuxSel);
  HWIO_OUTF(DISP_CC_DEBUG_DIV_CDIVR, CLK_DIV, 3);
  HWIO_OUTF(DISP_CC_DEBUG_CBCR, CLK_ENABLE, 1);
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlDISPCC =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxDISPCC,
};


/*=========================================================================
      GCC Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxGCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32_t                   nMuxSel
)
{
  HWIO_OUTF(GCC_DEBUG_MUX_MUXR, MUX_SEL, nMuxSel);
  HWIO_OUTF(GCC_DEBUG_DIV_CDIVR, CLK_DIV, 1);
  HWIO_OUTF(GCC_DEBUG_CBCR, CLK_ENABLE, 1);
}

static uint32_t HAL_clk_RunFreqCounterGCC
(
  uint32_t nTermCount
)
{
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, CNT_EN, 0);
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, XO_DIV4_TERM_CNT, nTermCount);
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, CNT_EN, 1);
  HAL_clk_BusyWait(5);
  while (HWIO_INF(GCC_CLOCK_FRQ_MEASURE_STATUS, XO_DIV4_CNT_DONE) == 0);
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, CNT_EN, 0);
  return HWIO_INF(GCC_CLOCK_FRQ_MEASURE_STATUS, MEASURE_CNT);
}

static void HAL_clk_CalcDebugMuxFreqGCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32_t                  *pnFreqHz
)
{
  uint64_t nNumerator;
  uint32_t nDenominator;
  uint32_t nShortCount, nLongCount;

  /*
   * Enable counter reference clock.
   */
  HWIO_OUTF(GCC_FREQ_MEASUREMENT_REF_CBCR, CLK_ENABLE, 1);

  /*
   * The counter only resets after a new clock edge comes in, so if the
   * clock under test is off there will be no edges and the counter
   * will retain the result from the previous measurement. The only way
   * to detect a disabled clock is to perform two measurements of
   * different length, since if the clock is on the counts will always be
   * different.
   */
  nShortCount = HAL_clk_RunFreqCounterGCC(HAL_CLK_FRQ_MEASURE_CNT_SHORT);
  nLongCount = HAL_clk_RunFreqCounterGCC(HAL_CLK_FRQ_MEASURE_CNT_LONG);
  if (nShortCount == nLongCount)
  {
    *pnFreqHz = 0;
  }
  else
  {
    /*
     * Calculate the frequency.  Function is provided by
     * Power Control 42.8.1.2 Measurement technique
     * f ring = f tcxo/4 * (Nring + 1.5) / (TCtcxo + 3.5)
     *
     * The formula below is the equivalent one converted
     * to integer arithmetic.
     *
     * We add half of the denominator value to the dividend
     * to guard against rounding errors inherent to
     * integer arithmetic.
     */
    nNumerator = 9600000 * (uint64_t)nLongCount + 14400000;
    nDenominator = HAL_CLK_FRQ_MEASURE_CNT_LONG * 2 + 7;
    *pnFreqHz = (uint32_t)((nNumerator + (nDenominator / 2)) / nDenominator);
  }
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlGCC =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxGCC,
  .CalcFreq   = HAL_clk_CalcDebugMuxFreqGCC,
};


/*=========================================================================
      GPU_CC Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxGPUCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32_t                   nMuxSel
)
{
  HWIO_OUTF(GPUCC_GPU_CC_DEBUG_MUX_MUXR, MUX_SEL, nMuxSel);
  HWIO_OUTF(GPUCC_GPU_CC_DEBUG_DIV_CDIVR, CLK_DIV, 1);
  HWIO_OUTF(GPUCC_GPU_CC_DEBUG_CBCR, CLK_ENABLE, 1);
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlGPUCC =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxGPUCC,
};


/*=========================================================================
      NPU_CC Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxNPUCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32_t                   nMuxSel
)
{
  HWIO_OUTF(NPU_CC_DEBUG_MUX_MUXR, MUX_SEL, nMuxSel);
  HWIO_OUTF(NPU_CC_DEBUG_DIV_CDIVR, CLK_DIV, 1);
  HWIO_OUTF(NPU_CC_DEBUG_CBCR, CLK_ENABLE, 1);
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlNPUCC =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxNPUCC,
};


/*=========================================================================
      NPU_CC_Q6 Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxNPUCCQ6
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32_t                   nMuxSel
)
{
  HWIO_OUTF(NPU_QDSP6SS_TEST_CLK_CTL, DBG_CLK_SEL, nMuxSel);
  HWIO_OUTF(NPU_QDSP6SS_TEST_CLK_CTL, Q6_DBG_CLK_EN, 1);
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlNPUCCQ6 =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxNPUCCQ6,
};


/*=========================================================================
      VIDEO_CC Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxVIDEOCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32_t                   nMuxSel
)
{
  HWIO_OUTF(CVP_VIDEO_CC_DEBUG_MUX_MUXR, MUX_SEL, nMuxSel);
  HWIO_OUTF(CVP_VIDEO_CC_DEBUG_DIV_CDIVR, CLK_DIV, 2);
  HWIO_OUTF(CVP_VIDEO_CC_DEBUG_CBCR, CLK_ENABLE, 1);
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlVIDEOCC =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxVIDEOCC,
};

