/*
===========================================================================
*/
/**
  @file ClockBIST.c

  Interface implementation file for the clock device driver
  Built-In-Self-Test (BIST).
*/
/*
  ====================================================================

  Copyright (c) 2011-2018 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/boot.xf/3.2/QcomPkg/Drivers/ClockDxe/ClockBIST.c#5 $
  $DateTime: 2019/07/24 03:12:02 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"
#include "HALclk.h"
#include "busywait.h"
#include <Library/DebugLib.h>

/*=========================================================================
      Definitions
==========================================================================*/

/*
 * The measured clock frequency must match the desired frequency within
 * the following error margin expressed in PPM of the desired
 * frequency, with a minimum error tolerance of 1 kHz.
 */
#define CLOCK_BIST_ERROR_MARGIN_PPM         400     /* Expressed in PPM */
#define CLOCK_BIST_ERROR_MINIMUM_TOLERANCE  5000    /* Expressed in Hz */

/*
 * Macro to return absolute value of the difference between two values
 */
#define ABS_DIFF(v1, v2)  ((v1 >= v2) ? (v1 - v2) : (v2 - v1))

/*
 * This is the maximum number of delay attempts that will be made when
 * waiting for a clock change (config, enable, disable, etc) to propagate
 * to the status register.
 */
#define CLOCK_BIST_CONFIG_DELAY_ATTEMPTS    250    /* Expressed in iterations */

/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_InitBISTLog
** =========================================================================*/
/**
  See ClockDriver.h
*/

DALResult Clock_InitBISTLog
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ULogResult eResult;

  eResult = ULogFront_RealTimeInit(&pDrvCtxt->hClockBISTLog, "Clock BIST Log",
    pDrvCtxt->nBISTLogSize, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  eResult = ULogCore_HeaderSet(pDrvCtxt->hClockBISTLog,
    "Content-Type: text/tagged-log-1.0;");
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* END Clock_InitBISTLog */



/* =========================================================================
**  Function : Clock_CheckBistExceptions
** =========================================================================*/
/**
  Check the given clock name against the clock names in the BIST exceptions
  clock name array.

  Some clocks are expected to fail the BIST under certain conditions. The
  names of these clocks are listed in an array together with other BSP data.
  This function searches that array for the given name and returns the
  result of that search.

  @param *pBISTExceptions [in] -- Handle to the BIST exceptions list
  @param *szName [in] -- Name of the clock to check.

  @return
  TRUE -- The clock name was found in the BIST exceptions array.
  FALSE -- The clock name is not in the array.

  @dependencies
  None.
*/

static boolean Clock_CheckBistExceptions
(
  ClockNameListType *pBISTExceptions,
  const char    *szName
)
{
  uint32 nIndex;

  for (nIndex = 0; pBISTExceptions[nIndex].szName != NULL; nIndex++)
  {
    if (strcmp(pBISTExceptions[nIndex].szName, szName) == 0)
    {
      return TRUE;
    }
  }

  return FALSE;

} /* END Clock_CheckBistExceptions */



/* =========================================================================
**  Function : Clock_CheckIfExcludedFromBist
** =========================================================================*/
/**
  Check the given clock name against the clock names in the list of clocks
  excluded from the BIST clock name array.

  Some clocks cannot be tested by the BIST under certain conditions. The
  names of these clocks are listed in an array together with other BSP data.
  This function searches that array for the given name and returns the
  result of that search.

  @param *pExcludeFromBist [in] -- Pointer to the list structure.
  @param *szName [in] -- Name of the clock to check.

  @return
  TRUE -- The clock name was found in the array.
  FALSE -- The clock name is not in the array.

  @dependencies
  None.
*/

static boolean Clock_CheckIfExcludedFromBist
(
  ClockNameListType *pExcludeFromBist,
  const char        *szName
)
{
  uint32 nIndex;

  for (nIndex = 0; pExcludeFromBist[nIndex].szName != NULL; nIndex++)
  {
    if (strcmp(pExcludeFromBist[nIndex].szName, szName) == 0)
    {
      return TRUE;
    }
  }

  return FALSE;

} /* END Clock_CheckIfExcludedFromBist */



/* =========================================================================
**  Function : Clock_TestClockFunctions
** =========================================================================*/
/**
  Perform a non-destructive built-in self test of all clock functions on the
  specified clock.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.
  @param ClockId [in] -- ID of the particular clock being tested.
  @param bIsClockSupportedIsOn [in] -- Specifies whether the clock supports
                                       "IsOn" API functionality
  @param bIsClockSupportedCalcFreq [in] -- Specifies whether the clock supports
                                           "CalcFreq" API functionality

  @return
  TRUE -- BIST was successful.
  FALSE -- BIST failed.

  @dependencies
  Core and image initialization must be complete prior to running the BIST.
*/

static boolean Clock_TestClockFunctions
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  boolean        bIsClockSupportedIsOn,
  boolean        bIsClockSupportedCalcFreq
)
{
  DALResult            eResult;
  boolean              bStatus;
  uint32               nFreqHz;
  uint32               nFreqError;
  uint32               nFreqErrorMargin;
  ClockMuxConfigType  *pBSPConfig;
  boolean              bBISTPassed = TRUE;
  uint32               nTimeout;
  uint32               i;

  /*-----------------------------------------------------------------------*/
  /* Set frequency if clock has not been initialized.                      */
  /*-----------------------------------------------------------------------*/

  if (pClock != NULL &&
      pClock->pDomain != NULL &&
      pClock->pDomain->aBSP != NULL &&
      pClock->pDomain->pActiveMuxConfig == NULL)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Initializing clock domain");

    /* Loop over all configurations */
    for (i = 0; i < pClock->pDomain->nBSPLen; i++)
    {
      pBSPConfig = &pClock->pDomain->aBSP[i];

      /* Skip configurations that aren't for this chip or version. */
      if ( !Clock_IsBSPSupported(&pBSPConfig->HWVersion) )
      {
        continue;
      }

      /* Set the new frequency */
      eResult = Clock_SetClockFrequency (pDrvCtxt,
                                         pClock,
                                         pBSPConfig->nFreqHz,
                                         CLOCK_FREQUENCY_HZ_EXACT,
                                         &nFreqHz);

      /* Check results of the operation */
      if (eResult != DAL_SUCCESS)
      {
        bBISTPassed = FALSE;
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_SetClockFrequency: ERROR");
        DEBUG((EFI_D_WARN,  "Clock_SetClockFrequency: ERROR \n"));

      }
      else
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_SetClockFrequency:  %d Hz", nFreqHz);
      }

      /* The returned frequency must match the desired frequency exactly */
      if (pBSPConfig->nFreqHz != nFreqHz)
      {
        bBISTPassed = FALSE;
        ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                          "   Frequency mismatch -- wanted: %d Hz vs got: %d Hz",
                          pBSPConfig->nFreqHz, nFreqHz);
        DEBUG((EFI_D_WARN,  "Frequency mismatch -- wanted: %d Hz vs got: %d Hz for clock: %a.\n", pBSPConfig->nFreqHz, nFreqHz ,pClock->szName));
      }

      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Turn on the clock.                                                    */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_EnableClock (pDrvCtxt, pClock);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_EnableClock: ERROR");
    DEBUG((EFI_D_WARN,  "Clock_EnableClock: ERROR .\n"));

    /* If this operation failed, there is no point in continuing the test */
    return FALSE;
  }
  else
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_EnableClock: SUCCESS");
  }

  /*-----------------------------------------------------------------------*/
  /* Test whether the clock was enabled successfully.                      */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_IsClockEnabled (pDrvCtxt, pClock, &bStatus);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockEnabled: ERROR");
    DEBUG((EFI_D_WARN,  "Clock_IsClockEnabled: ERROR.\n"));
  }
  else
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockEnabled: %d", bStatus);
    if(!bStatus)
    {
      DEBUG((EFI_D_WARN,  "Clock_IsClockEnabled: ERROR, clock not turning on even after enable clock API \n"));
    }
  }

  /* If this operation failed, we need to clean up before returning */
  if (eResult != DAL_SUCCESS || bStatus == 0)
  {
    bBISTPassed = FALSE;
  }
  else
  {
    /* Verify the clock is "on" if this function is supported */
    if (bIsClockSupportedIsOn)
    {
      eResult = Clock_IsClockOn (pDrvCtxt, pClock, &bStatus);

      if (eResult != DAL_SUCCESS)
      {
        bBISTPassed = FALSE;
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: ERROR");
        DEBUG((EFI_D_WARN,  "Clock_IsClockOn: ERROR.\n"));
      }
      else
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: %d", bStatus);

        /*
         * Since the clock is enabled and IsOn API is supported,
         * if it's not on, we have an error.
         */
        if (!bStatus)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: status mismatch");
          DEBUG((EFI_D_WARN,  "Clock_IsClockOn: status mismatch .\n"));
        }
      }
    }
    else
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: NOT SUPPORTED");
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Test all defined frequencies for this clock if possible.              */
  /*-----------------------------------------------------------------------*/

  if (bBISTPassed == TRUE)
  {
    /* Make sure the clock handle is valid and supports frequency configuration */
    if (pClock == NULL || pClock->pDomain == NULL ||
        pClock->pDomain->aBSP == NULL ||
        (pClock && pClock->pDomain && CLOCK_FLAG_IS_SET(pClock->pDomain, DFS_CONTROLLED)))
    {
      if (pClock && pClock->pDomain && CLOCK_FLAG_IS_SET(pClock->pDomain, DFS_CONTROLLED))
      {
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   SetClockFrequency: Skipping due to DFS control");
      }
      else
      {
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   SetClockFrequency: NOT SUPPORTED");
      }

      /* Just calculate the current clock frequency and report it */
      if (bIsClockSupportedCalcFreq)
      {
        eResult = Clock_CalcClockFrequency (pDrvCtxt, pClock, &nFreqHz);
        if (eResult != DAL_SUCCESS)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_CalcClockFrequency: ERROR");
          DEBUG((EFI_D_WARN,  "Clock_CalcClockFrequency: ERROR .\n"));
        }
        else
        {
          ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_CalcClockFrequency: %d Hz", nFreqHz);
        }
      }
      else
      {
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_CalcClockFrequency: NOT SUPPORTED");
      }
    }
    else
    {
      /* Loop over all configurations */
      for (i = 0; i < pClock->pDomain->nBSPLen; i++)
      {
        pBSPConfig = &pClock->pDomain->aBSP[i];

        /* Skip configurations that aren't for this chip or version. */
        if ( !Clock_IsBSPSupported(&pBSPConfig->HWVersion) )
        {
          continue;
        }

        /* Select the new frequency to test */
        eResult = Clock_SetClockFrequency (pDrvCtxt,
                                           pClock,
                                           pBSPConfig->nFreqHz,
                                           CLOCK_FREQUENCY_HZ_EXACT,
                                           &nFreqHz);

        /* Check results of the operation */
        if (eResult != DAL_SUCCESS)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_SetClockFrequency: ERROR");
          DEBUG((EFI_D_WARN,  "  Clock_SetClockFrequency: ERROR \n"));
        }
        else
        {
          ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_SetClockFrequency:  %d Hz", nFreqHz);
        }

        /* The returned frequency must match the desired frequency exactly */
        if (pBSPConfig->nFreqHz != nFreqHz)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                            "   Frequency mismatch -- wanted: %d Hz vs got: %d Hz",
                            pBSPConfig->nFreqHz, nFreqHz);
          DEBUG((EFI_D_WARN,  "Frequency mismatch -- wanted: %d Hz vs got: %d Hz for clock: %a.\n", pBSPConfig->nFreqHz, nFreqHz ,pClock->szName));
        }

        /*
         * Allow sufficient time for the clock status to propagate after
         * applying a new configuration.  This is needed due to issues
         * seen on 9x15 where 0 MHz was measured on clocks after a slow
         * speed was configured but not fully propagated.
         */
        if (bIsClockSupportedIsOn)
        {
          nTimeout = 0;

          do
          {
            busywait(1);
            eResult = Clock_IsClockOn(pDrvCtxt, pClock, &bStatus);

            /* Check results of the operation */
            if (eResult != DAL_SUCCESS)
            {
              ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: ERROR");
              DEBUG((EFI_D_WARN,  "Clock_IsClockOn: ERROR\n"));
            }

          } while ( (eResult == DAL_SUCCESS) &&
                    (!bStatus              ) &&
                    (nTimeout++ < CLOCK_BIST_CONFIG_DELAY_ATTEMPTS) );

          /* Log a warning if the clock does not turn ON */
          if (eResult != DAL_SUCCESS || !bStatus)
          {
            bBISTPassed = FALSE;

            ULOG_RT_PRINTF_1(pDrvCtxt->hClockBISTLog,
                             "   Unable to turn ON clock: %s.",
                             pClock->szName);
            DEBUG((EFI_D_WARN,  "Unable to turn ON clock: %a.\n",pClock->szName));
          }
        }

        /* Measure the frequency of the clock */
        if (bIsClockSupportedCalcFreq)
        {
          eResult = Clock_CalcClockFrequency (pDrvCtxt, pClock, &nFreqHz);

          /* Check results of the operation */
          if (eResult != DAL_SUCCESS)
          {
            bBISTPassed = FALSE;
            ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_CalcClockFrequency: ERROR");
            DEBUG((EFI_D_WARN,  "Clock_CalcClockFrequency: ERROR \n"));
          }
          else
          {
            ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_CalcClockFrequency: %d Hz", nFreqHz);
          }

          /* Compute the error rate between the desired and the measured values */
          nFreqError = ABS_DIFF(pBSPConfig->nFreqHz, nFreqHz);
          nFreqErrorMargin = pBSPConfig->nFreqHz / 1000000 * CLOCK_BIST_ERROR_MARGIN_PPM;

          /* Ignore any error within the permitted tolerance value */
          if (nFreqError > CLOCK_BIST_ERROR_MINIMUM_TOLERANCE)
          {
            /* The calculated frequency must match the desired frequency to some degree */
            if (nFreqError > nFreqErrorMargin)
            {
              bBISTPassed = FALSE;
              ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                                "   Frequency error %d Hz exceeds error margin of %d Hz",
                                nFreqError, nFreqErrorMargin);
              DEBUG((EFI_D_WARN,  "Frequency error %d Hz exceeds error margin of %d Hz\n",nFreqError, nFreqErrorMargin));
            }
            else
            {
              ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                                "   Frequency error %d Hz is within error margin of %d Hz",
                                nFreqError, nFreqErrorMargin);
            }
          }
          else
          {
            ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                              "   Frequency error %d Hz is within error tolerance of %d Hz",
                              nFreqError, CLOCK_BIST_ERROR_MINIMUM_TOLERANCE);
          }
        }
        else
        {
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   CalcClockFrequency: NOT SUPPORTED");
        }
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Turn off the clock after completing the test.                         */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_DisableClock (pDrvCtxt, pClock);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_DisableClock: ERROR");
    DEBUG((EFI_D_WARN,  "Clock_DisableClock: ERROR \n"));

    return FALSE;
  }
  else
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_DisableClock: SUCCESS");
  }

  /*-----------------------------------------------------------------------*/
  /* Test whether the clock was disabled successfully.                     */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_IsClockEnabled (pDrvCtxt, pClock, &bStatus);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockEnabled: ERROR");
    DEBUG((EFI_D_WARN,  "Clock_IsClockEnabled: ERROR \n"));
  }
  else
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockEnabled: %d", bStatus);
  }

  /* Update the test result to be returned */
  if (eResult != DAL_SUCCESS || bStatus == 1)
  {
    bBISTPassed = FALSE;
  }
  else
  {
    /*
     * Allow sufficient time for clocks to turn off.  This is needed
     * due to issues seen on 9x15 where clocks with slow configurations
     * took longer for the off status to propagate.
     */
    if (bIsClockSupportedIsOn)
    {
      nTimeout = 0;

      do
      {
        busywait(1);
        eResult = Clock_IsClockOn (pDrvCtxt, pClock, &bStatus);

        /* Check results of the operation */
        if (eResult != DAL_SUCCESS)
        {
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: ERROR");
          DEBUG((EFI_D_WARN,  "Clock_IsClockOn: ERROR \n"));
        }

      } while ( (eResult == DAL_SUCCESS) &&
                (bStatus               ) &&
                (nTimeout++ < CLOCK_BIST_CONFIG_DELAY_ATTEMPTS) );

      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: %d", bStatus);

      /* Log a warning if the clock does not turn OFF */
      if (eResult != DAL_SUCCESS || bStatus)
      {
        bBISTPassed = FALSE;
        if (pClock)
        {
          ULOG_RT_PRINTF_1(pDrvCtxt->hClockBISTLog,
                           "   Unable to turn OFF clock: %s.",
                           pClock->szName);
          DEBUG((EFI_D_WARN,  "Unable to turn OFF clock: %a.\n",pClock->szName));
        }
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return final result.                                                  */
  /*-----------------------------------------------------------------------*/

  return bBISTPassed;

} /* END Clock_TestClockFunctions */



/* =========================================================================
**  Function : Clock_TestClock
** =========================================================================*/
/**
  See ClockDriver.h
*/

DALResult Clock_TestClock
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock
)
{
  DALResult      eResult;
  boolean        bStatus;
  uint32         nFreqHz;
  boolean        bIsClockSupportedEnable;
  boolean        bIsClockSupportedIsOn;
  boolean        bIsClockSupportedCalcFreq;
  boolean        bBISTPassed = TRUE;

  /*-----------------------------------------------------------------------*/
  /* Initialize the BIST log.                                              */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->hClockBISTLog == NULL)
  {
    Clock_InitBISTLog(pDrvCtxt);
  }

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog,
                      "   INVALID CONFIGURATION DETECTED -- test aborted");
    return DAL_ERROR;
  }

  if (CLOCK_FLAG_IS_SET(pClock, EXPLICIT_ACCESS_ONLY))
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Identify the clock in the log by name and run the available tests.    */
  /*-----------------------------------------------------------------------*/

  ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog,
                    "Testing %s...", pClock->szName);
  DEBUG((EFI_D_WARN,  "\nTesting %a ...\n",pClock->szName));

  /*
   * Determine whether the clock supports "Enable" API functionality
   */

  bIsClockSupportedEnable = HAL_clk_IsClockSupported (&pClock->HALClock,
                                                      HAL_CLK_CLOCK_API_ENABLE);

  /*
   * Determine whether the clock supports "IsOn" API functionality
   */

  bIsClockSupportedIsOn = HAL_clk_IsClockSupported (&pClock->HALClock,
                                                    HAL_CLK_CLOCK_API_ISON);

  /*
   * Determine whether the clock supports "CalcFreq" API functionality
   */

  bIsClockSupportedCalcFreq = HAL_clk_IsClockSupported (&pClock->HALClock,
                                                        HAL_CLK_CLOCK_API_CALCFREQ);

  /*
   * Determine if the clock is already enabled
   */

  eResult = Clock_IsClockEnabled (pDrvCtxt, pClock, &bStatus);

  if (eResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockEnabled: ERROR");
    DEBUG((EFI_D_WARN,  "Clock_IsClockEnabled: ERROR.\n"));

    /* We must terminate the test here */
    return DAL_ERROR;
  }

  /*
   * If the clock is enabled, we'll assume it's in use and we can only verify
   * that it's on and check its frequency.
   */

  if (bStatus)
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockEnabled: %d", bStatus);

    /* Verify the clock is "on" if this function is supported */
    if (bIsClockSupportedIsOn)
    {
      eResult = Clock_IsClockOn (pDrvCtxt, pClock, &bStatus);

      if (eResult != DAL_SUCCESS)
      {
        bBISTPassed = FALSE;
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: ERROR");
        DEBUG((EFI_D_WARN,  "Clock_IsClockOn: ERROR.\n"));
      }
      else
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: %d", bStatus);

        /*
         * Since the clock is enabled and IsOn API is supported,
         * if it's not on, we have an error.
         */
        if (!bStatus)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: status mismatch");
          DEBUG((EFI_D_WARN,  "Clock_IsClockOn: status mismatch \n"));
        }
      }
    }
    else
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: NOT SUPPORTED");
    }

    if (bIsClockSupportedCalcFreq)
    {
      eResult = Clock_CalcClockFrequency (pDrvCtxt, pClock, &nFreqHz);
      if (eResult != DAL_SUCCESS)
      {
        bBISTPassed = FALSE;
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_CalcClockFrequency: ERROR");
        DEBUG((EFI_D_WARN,  "Clock_CalcClockFrequency: ERROR \n"));
      }
      else
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_CalcClockFrequency: %d Hz", nFreqHz);
      }
    }
    else
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_CalcClockFrequency: NOT SUPPORTED");
    }
  }
  else
  {
    /*
     * If the clock is disabled and it supports the "Enable" API, then it's not in use
     * and we can perform a complete test. Verify one more time that the clock is truly
     * "off" before proceeding to the complete test just in case the "Enable" API is
     * returning an erroneous result.
     */
    if (bIsClockSupportedEnable)
    {
      /* Verify the clock is truly "off" if this function is supported */
      if (bIsClockSupportedIsOn)
      {
        eResult = Clock_IsClockOn (pDrvCtxt, pClock, &bStatus);

        if (eResult != DAL_SUCCESS)
        {
          bBISTPassed = FALSE;
          ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: ERROR");
          DEBUG((EFI_D_WARN,  "Clock_IsClockOn: ERROR\n"));
        }
        else
        {
          ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock_IsClockOn: %d", bStatus);

          /*
           * If the clock is "on", we cannot continue with the complete test;
           * we must stop the test, but we cannot return failure because for
           * some clocks this is a legitimate state because these clocks may
           * be voted for by different masters. Therefore, we may think that
           * the clock is disabled because we voted for it to be disabled, but
           * in fact, the clock is "on" because one or more of the other masters
           * has voted for this clock to remain "on".
           */
          if (bStatus)
          {
            ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog,
                              "   Clock_IsClockOn: status mismatch -- test aborted");
            DEBUG((EFI_D_WARN,  "Clock_IsClockOn: status mismatch -- test aborted as one or more of the other masters has voted for this clock to remain on"));
            return DAL_SUCCESS;
          }
        }
      }

      /* Continue with the complete test if passed all the safety checks. */
      if (bBISTPassed)
      {
        bBISTPassed = Clock_TestClockFunctions (pDrvCtxt,
                                                pClock,
                                                bIsClockSupportedIsOn,
                                                bIsClockSupportedCalcFreq);
      }
    }
    else
    {
      ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "   Clock_EnableClock: NOT SUPPORTED");
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Log final result.                                                     */
  /*-----------------------------------------------------------------------*/

  if (bBISTPassed)
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock %s BIST Passed!",
                      pClock->szName);
    DEBUG((EFI_D_WARN,  "Clock %a BIST Passed!",pClock->szName));
    return DAL_SUCCESS;
  }
  else
  {
    ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "   Clock %s BIST Failed!",
                      pClock->szName);
    DEBUG((EFI_D_WARN,  "Clock %a BIST Failed!",pClock->szName));
    return DAL_ERROR;
  }

} /* END Clock_TestClock */



/* =========================================================================
**  Function : Clock_BIST
** =========================================================================*/
/**
  See ClockDriver.h
*/

DALResult Clock_BIST
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult                  eResult;
  uint32                     nClockIndex;
  ClockNodeType             *pClock;
  ClockIdType                ClockId;
  boolean                    bBISTPassed = TRUE;
  uint32                     i, nFailedTests = 0;
  ClockPowerDomainNodeType  *pPowerDomain;
  void* pExcludeFromBist = NULL;
  void* pBISTDependencies = NULL;
  void* pExcludedPowerDomains = NULL;
  void* pBISTExceptions = NULL;

  /*-----------------------------------------------------------------------*/
  /* Initialize the BIST log.                                              */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->hClockBISTLog == NULL)
  {
    Clock_InitBISTLog(pDrvCtxt);
  }

  /* Get the handle for the lists */
  eResult = Clock_GetPropertyHandle("BISTDependencies",&pBISTDependencies);
  eResult = Clock_GetPropertyHandle("ExcludeFromBist",&pExcludeFromBist);
  eResult = Clock_GetPropertyHandle("ExcludedPowerDomains",&pExcludedPowerDomains);
  eResult = Clock_GetPropertyHandle("BISTExceptions",&pBISTExceptions);

  /*
   * Enable any clocks that need to be enabled prior to running the BIST.
   */
  if (pBISTDependencies != NULL)
  {
    for (i = 0; ((ClockNameListType*)pBISTDependencies)[i].szName != NULL; i++)
    {
      if (Clock_GetClockId(pDrvCtxt, ((ClockNameListType*)pBISTDependencies)[i].szName, &ClockId) == DAL_SUCCESS)
      {
        pClock = Clock_GetClockNode(pDrvCtxt, ClockId);
        if (pClock == NULL)
        {
          ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog, "Failed to retrieve clock pointer for ID %d for BIST dependency clock %s",
            (int)ClockId, ((ClockNameListType*)pBISTDependencies)[i].szName);
          DEBUG((EFI_D_WARN, "Failed to retrieve clock pointer for ID %d for BIST dependency clock %a\n",(int)ClockId,((ClockNameListType*)pBISTDependencies)[i].szName));
        }
        else
        {
          if (Clock_EnableClock(pDrvCtxt, pClock) != DAL_SUCCESS)
          {
            ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "Failed to enable BIST dependency clock %s",
              ((ClockNameListType*)pBISTDependencies)[i].szName);
            DEBUG((EFI_D_WARN, "Failed to enable BIST dependency clock %a\n",((ClockNameListType*)pBISTDependencies)[i].szName));
          }
        }
      }
      else
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "Failed to retrieve ID for BIST dependency clock %s",
          ((ClockNameListType*)pBISTDependencies)[i].szName);
        DEBUG((EFI_D_WARN, "Failed to retrieve ID for BIST dependency clock %a\n",((ClockNameListType*)pBISTDependencies)[i].szName));
      }
    }
  }

  /*
   * Enable the power domains so that clocks turns on.
   */
  for (i = 0; i < pDrvCtxt->pBSP->nNumPowerDomains; i++)
  {
    pPowerDomain = &pDrvCtxt->pBSP->aPowerDomains[i];

    /*
     * Check the power domain name against the list of power domains that should not be
     * enabled before running the BIST and skip this power domain if it is in the list.
     */
    if (pExcludedPowerDomains != NULL)
    {
      if (Clock_CheckIfExcludedFromBist((ClockNameListType*)pExcludedPowerDomains, pPowerDomain->szName) == TRUE)
      {
        continue;
      }
    }

    /*
     * Do not enable power domains which are already turned on,
     * and do not disable them at the end.
     */

    if (!HAL_clk_IsPowerDomainOn(&pPowerDomain->HALPowerDomain))
    {
      if (Clock_EnablePowerDomain(pDrvCtxt, pPowerDomain) != DAL_SUCCESS)
      {
        ULOG_RT_PRINTF_1 (pDrvCtxt->hClockBISTLog, "Failed to enable %s",
          pPowerDomain->szName);
        DEBUG((EFI_D_WARN,  "Failed to enable %a",pPowerDomain->szName));
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Test each clock node.                                                 */
  /*-----------------------------------------------------------------------*/

  for (nClockIndex = 0; nClockIndex < pDrvCtxt->pBSP->nNumClocks; nClockIndex++)
  {
    pClock = &pDrvCtxt->pBSP->aClocks[nClockIndex];

    /*
     * Check the clock name against the list of clocks that should not
     * be running the BIST and skip this clock if it is in the list.
     */
    if (pExcludeFromBist != NULL)
    {
      if (Clock_CheckIfExcludedFromBist((ClockNameListType*)pExcludeFromBist, pClock->szName) == TRUE)
      {
        continue;
      }
    }

    /*
     * Check the clock name against the list of clocks that are expected
     * to fail BIST under certain conditions and mark this clock
     * appropriately.
     */
    if (pBISTExceptions != NULL)
    {
      if (Clock_CheckBistExceptions((ClockNameListType*)pBISTExceptions, pClock->szName) == TRUE)
      {
        CLOCK_FLAG_SET(pClock, BIST_FAILURE_ANTICIPATED);
      }
    }

    eResult = Clock_TestClock (pDrvCtxt, pClock);

    if (eResult != DAL_SUCCESS)
    {
      /*
       * Determine whether the BIST for this clock was expected to fail.
       * If so, don't count the failure against the rest of the test.
       */
      if (CLOCK_FLAG_IS_SET(pClock, BIST_FAILURE_ANTICIPATED))
      {
        ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog,
                          "   Failure Anticipated -- Ignoring Result.");
      }
      else
      {
        bBISTPassed = FALSE;
        nFailedTests++;
      }
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Log final result.                                                     */
  /*-----------------------------------------------------------------------*/

  if (bBISTPassed)
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockBISTLog, "Clock BIST Passed!");
    DEBUG((EFI_D_WARN,  "Clock BIST Passed!"));
    return DAL_SUCCESS;
  }
  else
  {
    ULOG_RT_PRINTF_2 (pDrvCtxt->hClockBISTLog,
                      "Clock BIST Failed %d out of %d tests!",
                      nFailedTests, nClockIndex);
    DEBUG((EFI_D_WARN,  "Clock BIST Failed %d out of %d tests\n!",nFailedTests,nClockIndex));
    return DAL_ERROR;
  }

} /* END Clock_BIST */


