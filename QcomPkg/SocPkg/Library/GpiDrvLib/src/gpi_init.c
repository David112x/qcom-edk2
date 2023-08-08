/**
 * @file  gpi_init.c
 * @brief Implements the GSI initialization 
 */
/*
===============================================================================

                               Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
03/09/18   ah      Hana changes.
04/18/17   ah      File created.

===============================================================================
              Copyright (c) 2017-2018 QUALCOMM Technologies, Inc.  
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/

#include "gpi_utils.h"
#ifdef GPI_DALSYS
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALFramework.h"
#include "DALDeviceId.h"
#elif GPI_XML_PROPS
#include "DALFramework.h"
#endif


static const tgt_gpi_config_type *gpi_tgt_config_ptr = NULL;

#ifdef GPI_XML_PROPS
DALSYS_PROPERTY_HANDLE_DECLARE(hPropGPI);
#endif

uint32 gpi_ee_init_done = 0;

/**
 * Initializes EE Interfaces.
 * Performs any intiialization pertaining to the OS specific APIs.
 *
 * @param    None
 *
 * @return   None.
 */
GPI_RETURN_STATUS gpi_ee_init(void)
{
   if (gpi_ee_init_done)
   {
      gpi_log(GPI_WARNING, 0, "gpi_ee_init - WARN - init already done!\n");
      return GPI_STATUS_SUCCESS;
   }

#ifdef GPI_DALSYS
   DALSYS_InitMod(NULL);
#endif

#ifdef GPI_DEBUGGING
   gpi_ulog_init();
#ifdef ENABLE_GPI_MICRO_ULOG
   gpi_uulog_init();
#endif
#endif

   gpi_ee_init_done = 1;

   return GPI_STATUS_SUCCESS;
}

/**
 * De-initializes OS specific Interfaces.
 *
 * Performs any De-initialization pertaining to the OS specific APIs.
 *
 * @param    None
 *
 * @return   None.
 */
void gpi_ee_deinit(gpi_ctxt *gpi)
{
   if (!gpi_ee_init_done)
   {
      return;
   }

#ifdef GPI_DALSYS
   if (gpi->hGpiDALInterrupt != NULL)
   {
      gpi_dev_detach(gpi->hGpiDALInterrupt);
      gpi->hGpiDALInterrupt = NULL;
   }
   if (gpi->hGpiHWIO != NULL)
   {
      gpi_dev_detach(gpi->hGpiHWIO);
      gpi->hGpiHWIO = NULL;
   }
   DALSYS_DeInitMod();
#endif

#ifdef GPI_DEBUGGING
   gpi_ulog_deinit();
#endif

   gpi_ee_init_done = 0;
}

/**
 * @brief Initializes GPI target info.
 *
 * @param[in,out]    gpi     Pointer to GPI context
 *
 * @return           Success of the operation.
 */
GPI_RETURN_STATUS gpi_init_tgt_info(gpi_ctxt *gpi)
{
   uint8 i;

#ifdef GPI_IRQ_MUXING
   gpi->irq_muxing = TRUE;
#endif

#ifdef GPI_DALSYS
   gpi_int_attach(&gpi->hGpiDALInterrupt);
#endif

#ifdef GPI_XML_PROPS
   DALSYSPropertyVar gpiprop;

#ifdef GPI_OEM
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStrEx("/core/hwengines/gpi", hPropGPI, DEVCFG_PROP_INFO_OEM))
#else
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr("/core/hwengines/gpi", hPropGPI))
#endif
      {
         gpi_log(GPI_ERROR, 0, "init_tgt_info - ERR - GetDALPropertyHandle fail!\n");
         return GPI_STATUS_ERROR;
      }

   if (DAL_SUCCESS != DALSYS_GetPropertyValue(hPropGPI, "tgt_gpi_config", 0, &gpiprop))
   {
      gpi_log(GPI_ERROR, 0, "init_tgt_info - ERR - GetPropertyValue fail!\n");
      return GPI_STATUS_ERROR;
   }
   gpi_tgt_config_ptr = (tgt_gpi_config_type *)gpiprop.Val.pStruct;
#else
   gpi_tgt_config_ptr = gpi_get_tgt_cfg();
#endif

   if (gpi_tgt_config_ptr == NULL)
   {
      gpi_log(GPI_ERROR, 0, "init_tgt_info - ERR - NULL ptr!\n");
      return GPI_STATUS_ERROR;
   }

   for (i = 0; i < MAX_NUM_QUP_BLOCKS; i++)
   {
      gpi->qup[i].active = gpi_tgt_config_ptr[i].active;

      if (gpi->qup[i].active)
      {
         gpi->qup[i].type = (QUPV3_BLOCK_TYPE)gpi_tgt_config_ptr[i].type;
         gpi->qup[i].qup_irq = gpi_tgt_config_ptr[i].irq;
         gpi->qup[i].gsi_pa = gpi_tgt_config_ptr[i].gsi_pa;
      }
   }

   return GPI_STATUS_SUCCESS;
}

/**
 * @brief Retrieves GPII IRQ value.
 *
 * @param[in,out]    gpii     GPII context
 *
 * @return           IRQ value.
 */
uint32 gpii_irq_val(gpii_info *gpii)
{
   return gpi_tgt_config_ptr[gpii->qup_type].gpii_irq[gpii->id];
}

/**
 * Main function to initialize GPI EE, target, and gloabl data structure.
 * To be called from the main task before any GPI request is made.
 * 
 * @param[in]    gpi    GPI context.
 *
 * @return       None.
 */
void gpi_init_all(gpi_ctxt *gpi)
{
   /* Default execution mode */
#ifdef GPI_MT
   gpi->exec_mode = GPI_THREAD;   /* Multi-threaded with interrupts */
#elif GPI_ST
   gpi->exec_mode = GPI_TASK;     /* Single-task, no task preemption, with interrupts */
#else
   gpi->exec_mode = GPI_POLLING;  /* Polling, no tasks, threads, or interrupts (Boot) */
#endif

   gpi->env_64bit = sizeof(U_LONG) > 4 ? TRUE : FALSE;

   if (EFI_SUCCESS != gpi_load_firmware())
   {
      gpi_log(GPI_ERROR, 0, "fwload - ERR - fail!\n");
      return;
   }

   if (GPI_STATUS_SUCCESS != gpi_ee_init())
   {
      gpi_log(GPI_ERROR, 0, "init_all - ERR - ee init fail!\n");
      return;
   }

   /* Initialize target data */
   if (GPI_STATUS_SUCCESS != gpi_init_tgt_info(gpi))
   {
      gpi_log(GPI_ERROR, 0, "init_all - ERR - tgt init fail!\n");
      return;
   }

   if (gpi->exec_mode == GPI_TASK)
   {
      gpi_reg_task(gpi);
   }

   /* Initialize GPI common data, GSI FW should have absolutely been loaded at this point */
   if (GPI_STATUS_SUCCESS != gpi_common_init(gpi))
   {
      gpi_log(GPI_ERROR, 0, "init_all - ERR - common init fail!\n");
      return;
   }
}
