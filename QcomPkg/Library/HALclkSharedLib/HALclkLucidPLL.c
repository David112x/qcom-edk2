/*
==============================================================================

FILE:         HALclkLucidPLL.c

DESCRIPTION:
This file contains the clock HAL code for the Lucid PLL control.
These PLLs are of the Lucid PLL variety.

==============================================================================

Edit History

$Header: //components/rel/boot.xf/3.2/QcomPkg/Library/HALclkSharedLib/HALclkLucidPLL.c#5 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/27/14   vt     Created

==============================================================================
Copyright (c) 2017- 2019 QUALCOMM Technologies Incorporated.  All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

INCLUDE FILES FOR MODULE

============================================================================*/

#include "stdbool.h"
#include "stdint.h"
#include "HALhwio.h"
#include "HALclkGenericPLL.h"
#include "HALclkLucidPLL.h"

/*============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*
* Source control functions.  These can be used if some of the control
* needs to be overridden by a specific PLL.
*/
static boolean HAL_clk_LucidPLLEnable(HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData);
static void HAL_clk_LucidPLLDisable(HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData);
static boolean HAL_clk_LucidPLLIsEnabled(HAL_clk_SourceDescType *pSource);
static void HAL_clk_LucidPLLConfig(HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig);
static boolean HAL_clk_LucidPLLConfigPLL(HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig, HAL_clk_SourceConfigMode eMode);
static void HAL_clk_LucidPLLDetectPLLConfig(HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig);
static boolean HAL_clk_LucidPLLEnableVote(HAL_clk_SourceDescType *pSource);
static void HAL_clk_LucidPLLDisableVote(HAL_clk_SourceDescType *pSource);
static boolean HAL_clk_LucidPLLIsVoteEnabled(HAL_clk_SourceDescType *pSource);
static boolean HAL_clk_LucidPLLIsPLLConfigEqual(const HAL_clk_PLLConfigType *pConfigA, const HAL_clk_PLLConfigType *pConfigB);
static boolean HAL_clk_LucidPLLIsCalibrated(HAL_clk_SourceDescType *pSource);
static boolean HAL_clk_LucidPLLSetCalConfig(HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig);
static void HAL_clk_LucidPLLSetRegSettings(HAL_clk_SourceDescType *pSource, const HAL_clk_SourceRegSettingsType *pmSettings);

/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
* HAL_CLK_PLL_MODE_*
*
* Definitions for configuring the PLLn_MODE register.
*/
#define HAL_CLK_PLL_MODE_ACTIVE  \
	(HWIO_FMSK(PLL_MODE, PLL_OUTCTRL) | \
	HWIO_FMSK(PLL_MODE, PLL_RESET_N))

/*
* Definitions for configuring the PLL in FSM Mode
*/
#define HAL_CLK_PLL_BIAS_COUNT_VAL   (0x1 << HWIO_SHFT(PLL_MODE, PLL_BIAS_COUNT))
#define HAL_CLK_PLL_LOCK_COUNT_VAL   (0x0 << HWIO_SHFT(PLL_MODE, PLL_LOCK_COUNT))

/*
* Definition for configuring M/N in FRAC_VAL register
* Note: This register will look like one of the following:
*        [b3,b2,b1,b0] - Format
*        [0,0,M,N]     - USER_CTL_U.FRAC_FORMAT_SEL == 1
*        [0,0,a1,a0]   - USER_CTL_U.FRAC_FORMAT_SEL == 0
*/
#define HAL_CLK_PLL_M_N_VAL(m,n) ((( ( m ) & 0xFF ) << 8 ) | ( ( n ) & 0xFF ))

/*
* HAL_CLK_PLL_INIT_TIME_US
*
* Init time in microseconds after de-asserting PLL reset (normal mode).
*/
#define HAL_CLK_PLL_INIT_TIME_US 5


/*
* HAL_CLK_LOCK_TIMEOUT_US
*
* Timeout in US for PLL_LOCK_DET to get set.
*/
#define HAL_CLK_LOCK_TIMEOUT_US 200

/*
* HAL_CLK_UPDATE_TIMEOUT_US
*
* Timeout in US for PLL_UPDATE to get set.
*/
#define HAL_CLK_UPDATE_TIMEOUT_US 200

/*
* HAL_CLK_PLL_OUTCTRL_OFF_US
*
* Wait after PLL_OUTCTRL off before setting freeze bit.
*/
#define HAL_CLK_PLL_OUTCTRL_OFF_US 5


/* ============================================================================
**    Data Declarations
** ==========================================================================*/

/*
* HAL_clk_LucidPLLControl
*
* Control function array for the Lucid PLLs.
*/
HAL_clk_SourceControlType HAL_clk_LucidPLLControl =
{
	.Enable = HAL_clk_LucidPLLEnable,
	.Disable = HAL_clk_LucidPLLDisable,
	.IsEnabled = HAL_clk_LucidPLLIsEnabled,
	.Config = HAL_clk_LucidPLLConfig,
	.ConfigPLL = HAL_clk_LucidPLLConfigPLL,
	.DetectPLLConfig = HAL_clk_LucidPLLDetectPLLConfig,
	.EnableVote = HAL_clk_LucidPLLEnableVote,
	.DisableVote = HAL_clk_LucidPLLDisableVote,
	.IsVoteEnabled = HAL_clk_LucidPLLIsVoteEnabled,
	.IsPLLConfigEqual = HAL_clk_LucidPLLIsPLLConfigEqual,
	.IsCalibrated = HAL_clk_LucidPLLIsCalibrated,
	.SetCalConfig = HAL_clk_LucidPLLSetCalConfig,
	.SetRegSettings = HAL_clk_LucidPLLSetRegSettings,
};


/*============================================================================

FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_LucidPLLEnableVote
**
** ======================================================================== */

static boolean HAL_clk_LucidPLLEnableVote
(
HAL_clk_SourceDescType *pSource
)
{
	uint32_t nVal;
	boolean     bResult;

	/*
	* Set the appropriate bit.
	*/
	nVal = inpdw(pSource->VoterRegister.nAddr) | pSource->VoterRegister.nMask;
	outpdw(pSource->VoterRegister.nAddr, nVal);

	/*
	* Wait for the PLL to go active during the 15 us timeout period.
	*/
	bResult =
		HAL_clk_ValidateRegSetTimeout(
		HAL_CLK_PLL_HWIO_ADDR(PLL_MODE, pSource),
		HWIO_FMSK(PLL_MODE, PLL_ACTIVE_FLAG),
		HAL_CLK_LOCK_TIMEOUT_US);

	return bResult;

} /* END HAL_clk_LucidPLLEnableVote */


/* ===========================================================================
**  HAL_clk_LucidPLLDisableVote
**
** ======================================================================== */

static void HAL_clk_LucidPLLDisableVote
(
HAL_clk_SourceDescType *pSource
)
{
	uint32_t nVal;

	/*
	* Clear the appropriate bit.
	*/
	nVal = inpdw(pSource->VoterRegister.nAddr) & ~pSource->VoterRegister.nMask;
	outpdw(pSource->VoterRegister.nAddr, nVal);

} /* END HAL_clk_LucidPLLDisableVote */


/* ===========================================================================
**  HAL_clk_LucidPLLIsVoteEnabled
**
** ======================================================================== */

static boolean HAL_clk_LucidPLLIsVoteEnabled
(
HAL_clk_SourceDescType *pSource
)
{
	/*
	* Check the appropriate bit.
	*/
	if (inpdw(pSource->VoterRegister.nAddr) & pSource->VoterRegister.nMask)
	{
		return TRUE;
	}

	return FALSE;

} /* END HAL_clk_LucidPLLIsVoteEnabled */


/* ===========================================================================
**  HAL_clk_LucidPLLEnable
**
**  The appropriate enable mode is executed based on the eMode parameter.
**
**  Returns:
**    bool - TRUE if successfully enabled. FALSE otherwise.
** ======================================================================== */

static boolean HAL_clk_LucidPLLEnable
(
HAL_clk_SourceDescType        *pSource,
HAL_clk_SourceDisableModeType  eMode,
void                          *pData
)
{
	boolean bResult;

	/*
	* Set desired operational mode.
	*/
	HAL_CLK_PLL_HWIO_OUT(PLL_OPMODE, pSource, HAL_PLL_OPMODE_RUN);

	/*
	* Take PLL out of reset, which will switch to configured OPMODE.
	*/
	HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, PLL_RESET_N, 1);

	/*
	* Wait for the PLL to lock.
	*/
	bResult =
		HAL_clk_ValidateRegSetTimeout(
		HAL_CLK_PLL_HWIO_ADDR(PLL_MODE, pSource),
		HWIO_FMSK(PLL_MODE, PLL_LOCK_DET),
		HAL_CLK_LOCK_TIMEOUT_US);
	if (!bResult)
	{
		return FALSE;
	}

	/*
	* Enable the PLL outputs.
	*/
	HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, PLL_OUTCTRL, 1);
	return TRUE;

} /* END HAL_clk_LucidPLLEnable */


/* ===========================================================================
**  HAL_clk_LucidPLLDisable
**
**  The appropriate disable mode is executed based on the eMode argument.
**
** ======================================================================== */

static void HAL_clk_LucidPLLDisable
(
HAL_clk_SourceDescType        *pSource,
HAL_clk_SourceDisableModeType  eMode,
void                          *pData
)
{
	/*
	* Disable the PLL outputs.
	*/
	HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, PLL_OUTCTRL, 0);

	/*
	* Wait 2 ticks of the output main clock.
	*/
	HAL_clk_BusyWait(1);

	/*
	* Disable the PLL.
	*/
	if (eMode == HAL_CLK_SOURCE_DISABLE_MODE_STANDBY)
	{
		HAL_CLK_PLL_HWIO_OUT(PLL_OPMODE, pSource, HAL_PLL_OPMODE_STANDBY);
	}
	else
	{
		HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, PLL_RESET_N, 0);
	}

} /* END HAL_clk_LucidPLLDisable */


/* ===========================================================================
**  HAL_clk_LucidPLLIsEnabled
**
** ======================================================================== */

static boolean HAL_clk_LucidPLLIsEnabled
(
HAL_clk_SourceDescType *pSource
)
{
	uint32_t nModeVal;
	/*
	* Read current mode value.
	*/
	nModeVal = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource);

	/*
	* Check whether the PLL is enabled.
	*/
	return ((nModeVal & HAL_CLK_PLL_MODE_ACTIVE) == HAL_CLK_PLL_MODE_ACTIVE);

} /* END HAL_clk_LucidPLLIsEnabled */


/* ===========================================================================
**  HAL_clk_LucidPLLConfig
**
** ======================================================================== */

static void HAL_clk_LucidPLLConfig
(
HAL_clk_SourceDescType   *pSource,
HAL_clk_SourceConfigType  eConfig
)
{
	/*
	* Take action.
	*/
	switch (eConfig)
	{
	case HAL_CLK_CONFIG_PLL_FSM_MODE_ENABLE:
		HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, PLL_VOTE_FSM_ENA, 1);
		break;

	case HAL_CLK_CONFIG_PLL_FSM_MODE_DISABLE:
		HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, PLL_VOTE_FSM_ENA, 0);
		break;

	case HAL_CLK_CONFIG_PLL_EVEN_OUTPUT_ENABLE:
		HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_EVEN, 1);
		break;

	case HAL_CLK_CONFIG_PLL_EVEN_OUTPUT_DISABLE:
		HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_EVEN, 0);
		break;

	case HAL_CLK_CONFIG_PLL_ODD_OUTPUT_ENABLE:
		HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_ODD, 1);
		break;

	case HAL_CLK_CONFIG_PLL_ODD_OUTPUT_DISABLE:
		HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_ODD, 0);
		break;

	case HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_ENABLE:
		HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_MAIN, 1);
		break;

	case HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_DISABLE:
		HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_MAIN, 0);
		break;
	default:
		break;
	}

} /* END HAL_clk_LucidPLLConfig */


/* ===========================================================================
**  HAL_clk_LucidPLLConfigPLLSlew
**
**  Returns:
**    bool - TRUE if successfully configured.
**              FALSE if unable to detect that the PLL successfully locked.
** ======================================================================== */

static boolean HAL_clk_LucidPLLConfigPLLSlew
(
HAL_clk_SourceDescType      *pSource,
const HAL_clk_PLLConfigType *pConfig,
boolean                         bTriggerUpdate
)
{
	uint32_t nModeVal,nUserUVal;
	boolean     bResult;

	/*
	* Program L value.
	*/
	HAL_CLK_PLL_HWIO_OUT(PLL_L_VAL, pSource, pConfig->nL);

	/*
	* Program fractional value.
	*/
	nUserUVal = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL_U, pSource);
	if (pConfig->eFracMode == HAL_CLK_PLL_FRAC_MODE_MN)
	{
		nUserUVal |= HWIO_FMSK(PLL_USER_CTL_U, FRAC_FORMAT_SEL);
	}
	else
	{
		/* Default to Alpha */
		nUserUVal &= ~HWIO_FMSK(PLL_USER_CTL_U, FRAC_FORMAT_SEL);
	}

	HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL_U, pSource, nUserUVal);
	HAL_CLK_PLL_HWIO_OUT(PLL_FRAC_VAL, pSource, pConfig->nFracVal);

	/*
	* Check if the update will be triggered later.
	*/
	if (bTriggerUpdate != TRUE)
	{
		return TRUE;
	}
	
   /*
   * Latch in the new configuration values.
   */
    nModeVal = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource);
    nModeVal |= HWIO_FMSK(PLL_MODE, PLL_UPDATE);
    HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);
	
   /*
   * Wait for PLL_UPDATE to be cleared during the timeout period.
   */
   bResult =
    HAL_clk_ValidateRegClearTimeout(
        HAL_CLK_PLL_HWIO_ADDR(PLL_MODE, pSource),
        HWIO_FMSK(PLL_MODE, PLL_UPDATE),
        HAL_CLK_UPDATE_TIMEOUT_US);

   /*
   * Return immediately if update failed.
   */
   if (!bResult)
   {
     return FALSE;
   }

  /*
   * Wait 2 ticks for the PLL to change.  This is required since
   * PLL_LOCK_DET may not transition to 0 if the change is small.
   */
   HAL_clk_BusyWait(1);
	
	/*
	* Wait for the PLL to lock during the timeout period.
	*/
	bResult =
		HAL_clk_ValidateRegSetTimeout(
		HAL_CLK_PLL_HWIO_ADDR(PLL_MODE, pSource),
		HWIO_FMSK(PLL_MODE, PLL_LOCK_DET),
		HAL_CLK_LOCK_TIMEOUT_US);

	return bResult;

} /* END HAL_clk_LucidPLLConfigPLLSlew */


/* ===========================================================================
**  HAL_clk_LucidPLLConfigPLLNormal
**
**  Returns:
**    bool - TRUE if successfully configured. FALSE otherwise.
** ======================================================================== */

static boolean HAL_clk_LucidPLLConfigPLLNormal
(
HAL_clk_SourceDescType      *pSource,
const HAL_clk_PLLConfigType *pConfig
)
{
	uint32_t nModeVal, nUserVal, nUserUVal;
	uint32_t nPLLInFSMMode = 0;
	uint32_t nOddDiv, nEvenDiv;
	boolean     bPLLEnabled = FALSE;

	/*
	* Check PLL mode.
	*/
	nPLLInFSMMode = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource) &
		HWIO_FMSK(PLL_MODE, PLL_VOTE_FSM_ENA);
	bPLLEnabled = HAL_clk_LucidPLLIsEnabled(pSource);

	/*
	* Disable the PLL.
	*/
	if (bPLLEnabled)
	{
		HAL_clk_LucidPLLDisable(pSource, HAL_CLK_SOURCE_DISABLE_MODE_NORMAL, NULL);
	}

	/*
	* Read the PLLx_Mode value
	*/
	nModeVal = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource);

	/*
	* Disable FSM Mode
	*/
	if (nPLLInFSMMode != 0)
	{
		nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_VOTE_FSM_ENA);
		HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);
	}

	/*
	* Program L value.
	*/
	HAL_CLK_PLL_HWIO_OUT(PLL_L_VAL, pSource, pConfig->nL);

	/*
	* Program fractional value.
	*/
	nUserUVal = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL_U, pSource);
	if (pConfig->eFracMode == HAL_CLK_PLL_FRAC_MODE_MN)
	{
		nUserUVal |= HWIO_FMSK(PLL_USER_CTL_U, FRAC_FORMAT_SEL);
	}
	else
	{
		/* Default to Alpha. */
		nUserUVal &= ~HWIO_FMSK(PLL_USER_CTL_U, FRAC_FORMAT_SEL);
	}

	HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL_U, pSource, nUserUVal);
	HAL_CLK_PLL_HWIO_OUT(PLL_FRAC_VAL, pSource, pConfig->nFracVal);

	/*
	* Get the CONFIG value and clear out fields we will configure.
	*/
	nUserVal = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pSource);
	nUserVal &= ~(HWIO_FMSK(PLL_USER_CTL, PRE_DIV_RATIO) |
		HWIO_FMSK(PLL_USER_CTL, POST_DIV_RATIO_ODD) |
		HWIO_FMSK(PLL_USER_CTL, POST_DIV_RATIO_EVEN));

	/*
	* Program the pre-div value (div-1 to div-8).
	*/
	if (pConfig->nPreDiv <= 8 && pConfig->nPreDiv > 0)
	{
		nUserVal |= HWIO_FVAL(PLL_USER_CTL, PRE_DIV_RATIO, (pConfig->nPreDiv - 1));
	}

	/*
	* Program the post-div values.
	*/
	nOddDiv = pConfig->PostDivs.Lucid.nOdd;
	nEvenDiv = pConfig->PostDivs.Lucid.nEven;
	if (nEvenDiv > 0)
	{
		nEvenDiv--;
	}
	nUserVal |= HWIO_FVAL(PLL_USER_CTL, POST_DIV_RATIO_ODD, nOddDiv);
	nUserVal |= HWIO_FVAL(PLL_USER_CTL, POST_DIV_RATIO_EVEN, nEvenDiv);

	/*
	* Finally program the USER_CTL register.
	*/
	HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL, pSource, nUserVal);

	/*
	* Set behavior of PLL_LOCK_DET to always use fine grained.
	*/
	HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL_U, pSource, USE_ONLY_FINE_LOCK_DET, 1);

	/*
	* If there is an address for a voting register, then the PLL is in
	* FSM voteable mode. In this case, the enable sequence is handled in
	* hardware, guided by the BIAS and LOCK counts.
	*/
	if (nPLLInFSMMode)
	{
		/*
		* Put the FSM in reset.
		*/
		nModeVal |= HWIO_FMSK(PLL_MODE, PLL_VOTE_FSM_RESET);
		HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

		/*
		* Program the FSM portion of the mode register.
		*/
		nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_BIAS_COUNT);
		nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_LOCK_COUNT);

		nModeVal |= HAL_CLK_PLL_BIAS_COUNT_VAL;
		nModeVal |= HAL_CLK_PLL_LOCK_COUNT_VAL;

		nModeVal |= HWIO_FMSK(PLL_MODE, PLL_VOTE_FSM_ENA);

		/*
		* Use the new FSM enable/disable logic. The legacy sequence requires
		* SW to set OPMODE to 1 and the FSM will control RESET_N, which means
		* the PLL will transition to OFF when no votes. The new logic requires
		* SW to set RESET_N and the FSM will control OPMODE, which means
		* the PLL will transition to STANDBY when no votes. The new logic
		* reduces lock time and has no power impact.
		*/
		nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_FSM_LEGACY_MODE);
		nModeVal |= HWIO_FMSK(PLL_MODE, PLL_RESET_N);

		/*
		* Finally program the Mode register.
		*/
		HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

		/*
		* Take the FSM out of reset.
		*/
		nModeVal &= ~HWIO_FMSK(PLL_MODE, PLL_VOTE_FSM_RESET);
		HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);
	}
	else
	{
		/*
		* When not using the FSM, FSM_LEGACY_MODE must be set to 1.
		*/
		HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, PLL_FSM_LEGACY_MODE, 1);
	}

	/*
	* Enable the PLL if it was on before and not in FSM mode (in FSM
	* mode this function is not used).
	*/
	if (bPLLEnabled && nPLLInFSMMode == 0)
	{
		HAL_clk_LucidPLLEnable(pSource, HAL_CLK_SOURCE_DISABLE_MODE_NORMAL, NULL);
	}

	return TRUE;

} /* END HAL_clk_LucidPLLConfigPLLNormal */


/* ===========================================================================
**  HAL_clk_LucidPLLConfigPLL
**
**  Returns:
**    bool - TRUE if successfully configured. FALSE otherwise.
** ======================================================================== */

static boolean HAL_clk_LucidPLLConfigPLL
(
HAL_clk_SourceDescType      *pSource,
const HAL_clk_PLLConfigType *pConfig,
HAL_clk_SourceConfigMode     eMode
)
{
	boolean bResult = FALSE;

	/*
	* Program the appropriate config sequence.
	*/
	if (eMode == HAL_CLK_SOURCE_CONFIG_MODE_NORMAL)
	{
		bResult = HAL_clk_LucidPLLConfigPLLNormal(pSource, pConfig);
	}
	else if (eMode == HAL_CLK_SOURCE_CONFIG_MODE_SLEW)
	{
		bResult = HAL_clk_LucidPLLConfigPLLSlew(pSource, pConfig, TRUE);
	}
	else if (eMode == HAL_CLK_SOURCE_CONFIG_MODE_PENDING)
	{
		bResult = HAL_clk_LucidPLLConfigPLLSlew(pSource, pConfig, FALSE);
	}

	return bResult;

} /* END HAL_clk_LucidPLLConfigPLL */


/* ===========================================================================
**  HAL_clk_LucidPLLDetectPLLConfig
**
** ======================================================================== */

static void HAL_clk_LucidPLLDetectPLLConfig
(
HAL_clk_SourceDescType *pSource,
HAL_clk_PLLConfigType  *pConfig
)
{
	uint32_t nRegVal;

	/*
	* Clear all config values not modified in this function.
	*/
	pConfig->nFracVal = 0;

	/*
	* The Lucid architecture abstracts the VCO logic away from the user so just
	* default this to the first VCO for initialization purposes.
	*/
	pConfig->nMode = HAL_CLK_PLL_HWIO_INF(PLL_USER_CTL_U, pSource, LOW_JITTER_MODE_EN);

	/*
	* Read L value.
	*/
	pConfig->nL = HAL_CLK_PLL_HWIO_IN(PLL_L_VAL, pSource);

	/*
	* Read fractional value.
	*/
	nRegVal = HAL_CLK_PLL_HWIO_INF(PLL_FRAC_VAL, pSource, PLL_FRAC_VAL);
	pConfig->nFracVal = nRegVal;

	/*
	* Detect fractional mode.
	*/
	if (HAL_CLK_PLL_HWIO_INF(PLL_USER_CTL_U, pSource, FRAC_FORMAT_SEL))
	{
		pConfig->eFracMode = HAL_CLK_PLL_FRAC_MODE_MN;
	}
	else
	{
		pConfig->eFracMode = HAL_CLK_PLL_FRAC_MODE_ALPHA;
	}

	/*
	* No reference clock mux, so default to 0.
	*/
	pConfig->nSource = 0;

	/*
	* Read USER_CTL register.
	*/
	nRegVal = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pSource);

	/*
	* Get the pre divider.
	*/
	pConfig->nPreDiv =
		HAL_CLK_PLL_HWIO_INFV(PLL_USER_CTL, pSource, PRE_DIV_RATIO, nRegVal) + 1;

	/*
	* Get the post divider.
	*/
	pConfig->PostDivs.Lucid.nEven =
		HAL_CLK_PLL_HWIO_INFV(PLL_USER_CTL, pSource, POST_DIV_RATIO_EVEN, nRegVal)
		+ 1;

	pConfig->PostDivs.Lucid.nOdd =
		HAL_CLK_PLL_HWIO_INFV(PLL_USER_CTL, pSource, POST_DIV_RATIO_ODD, nRegVal);

	/*
	* Add 1 for case where field is 0.  Otherwise leave as is.
	*/
	if (pConfig->PostDivs.Lucid.nOdd == 0)
	{
		pConfig->PostDivs.Lucid.nOdd++;
	}

} /* END HAL_clk_LucidPLLDetectPLLConfig */


/* ===========================================================================
**  HAL_clk_LucidPLLIsPLLConfigEqual
**
** ======================================================================== */

static boolean HAL_clk_LucidPLLIsPLLConfigEqual
(
const HAL_clk_PLLConfigType *pConfigA,
const HAL_clk_PLLConfigType *pConfigB
)
{
	uint32_t nEvenDivA, nEvenDivB;
	uint32_t nOddDivA, nOddDivB;

	if (HAL_clk_GenericIsPLLConfigEqual(pConfigA, pConfigB))
	{
		/* Treat div-0 and div-1 as the same thing. */
		nEvenDivA = MAX(1, pConfigA->PostDivs.Lucid.nEven);
		nEvenDivB = MAX(1, pConfigB->PostDivs.Lucid.nEven);
		nOddDivA = MAX(1, pConfigA->PostDivs.Lucid.nOdd);
		nOddDivB = MAX(1, pConfigB->PostDivs.Lucid.nOdd);

		if (nEvenDivA == nEvenDivB && nOddDivA == nOddDivB)
		{
			return TRUE;
		}
	}

	return FALSE;

} /* END HAL_clk_LucidPLLIsPLLConfigEqual */


/* ===========================================================================
**  HAL_clk_LucidPLLIsCalibrated
**
** ======================================================================== */

static boolean HAL_clk_LucidPLLIsCalibrated
(
HAL_clk_SourceDescType *pSource
)
{
	uint32_t nVal;

	nVal = HAL_CLK_PLL_HWIO_INF(PLL_MODE, pSource, PCAL_DONE);
	return nVal ? TRUE : FALSE;

} /* HAL_clk_LucidPLLIsCalibrated */


/* ===========================================================================
**  HAL_clk_LucidPLLSetCalConfig
**
** ======================================================================== */

static boolean HAL_clk_LucidPLLSetCalConfig
(
HAL_clk_SourceDescType      *pSource,
const HAL_clk_PLLConfigType *pConfig
)
{
	HAL_CLK_PLL_HWIO_OUT(PLL_CAL_L_VAL, pSource, pConfig->nL);
	return TRUE;

} /* HAL_clk_LucidPLLSetCalConfig */


/* ===========================================================================
**  HAL_clk_LucidPLLSetRegSettings
**
** ======================================================================== */

static void HAL_clk_LucidPLLSetRegSettings
(
HAL_clk_SourceDescType              *pSource,
const HAL_clk_SourceRegSettingsType *pmSettings
)
{
	HAL_CLK_PLL_HWIO_OUT(PLL_CONFIG_CTL, pSource, pmSettings->nConfigCtl);
	HAL_CLK_PLL_HWIO_OUT(PLL_CONFIG_CTL_U, pSource, pmSettings->nConfigCtl1);
	HAL_CLK_PLL_HWIO_OUT(PLL_CONFIG_CTL_U1, pSource, pmSettings->nConfigCtl2);
	HAL_CLK_PLL_HWIO_OUT(PLL_TEST_CTL, pSource, pmSettings->nTestCtl);
	HAL_CLK_PLL_HWIO_OUT(PLL_TEST_CTL_U, pSource, pmSettings->nTestCtl1);
	HAL_CLK_PLL_HWIO_OUT(PLL_TEST_CTL_U1, pSource, pmSettings->nTestCtl2);
	HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL, pSource, pmSettings->nUserCtl);
	HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL_U, pSource, pmSettings->nUserCtl1);
	HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL_U1, pSource, pmSettings->nUserCtl2);

} /* END HAL_clk_LucidPLLSetRegSettings */




