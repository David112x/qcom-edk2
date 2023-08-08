/**
  @file  gpi_threads.c
  @brief GPI Thread Handling.

*/
/*
===============================================================================

                               Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
03/09/18   ah      Hana changes.
02/19/16   ah      File created.

===============================================================================
              Copyright (c) 2016-2018 QUALCOMM Technologies, Inc.  
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/

#include "gpi_utils.h"

/**
 * @brief Handler for MHI command events.
 *
 * @param[in ]        gpi            gpi context
 * @param[in ]        evt            Event to handle
 */
void handle_helper_evt(gpi_ctxt *gpi, gpi_ring_elem *evt)
{
   GPI_IRQ_TYPE helper_action = (GPI_IRQ_TYPE)evt->ctrl;
   gpii_info *gpii;
   qup_instance *qup;
   GPI_CHAN_TYPE chan;
   uint8 state;
   uint32 val;
   ring_info_type *ring_info;
   gpi_result_type result;
   U_LONG wp;

   val = GPI_READ_FIELD(evt->dword_2, LOCAL_EVT_GSI_INST);
   if (val >= MAX_NUM_QUP_BLOCKS)
   {
      gpi_log(GPI_ERROR, 1, "helper_evt - ERROR - invalid QUP: %d \n", val);
      return;
   }
   qup = &gpi->qup[val];

   val = GPI_READ_FIELD(evt->dword_2, LOCAL_EVT_GPII_ID);
   if (val >= NUM_GPII_PER_QUP)
   {
      gpi_log(GPI_ERROR, 1, "helper_evt - ERROR - invalid GPII: %d \n", val);
      return;
   }
   gpii = qup->gpii[val];
   gpi_log(GPI_DEBUG, 1, "helper_evt - GPII: %d \n", gpii->id);

   gpi_syncenter(&qup->qup_mutex);
   if (!qup->active_gpii_bmsk)
   {
      gpi_log(GPI_WARNING, 1, "handle_helper_evt - WARN - No active GPII\n");
      gpi_syncleave(&qup->qup_mutex);
      return;
   }
   gpi_syncleave(&qup->qup_mutex);

   gpi_syncenter(&gpii->gpii_mutex);
   gpi_memset(&result, 0, sizeof(gpi_result_type));

   switch (helper_action)
   {
      case GPI_IRQ_CH_CTRL:
      case GPI_IRQ_EV_CTRL:
         if (helper_action == GPI_IRQ_CH_CTRL)
         {
            chan = (GPI_CHAN_TYPE)GPI_READ_FIELD(evt->dword_2, LOCAL_EVT_CHID);
            result.chan = chan;
            if (chan >= NUM_RINGS_PER_GPII)
            {
               gpi_syncleave(&gpii->gpii_mutex);
               gpi_log(GPI_ERROR, 1, "helper_evt - ERR - unexpected chan: %d\n", chan);
               return;
            }

            ring_info = &gpii->chan_info[chan];

            /* Read the new channel state */
            state = (GPI_CHAN_STATE)GPI_READ_FIELD(evt->dword_2, LOCAL_EVT_CHAN_STATE);
            gpi_log(GPI_DEBUG, 3, "helper_evt - gpii-%d, chan: %d state: %d\n",
                    gpii->id, chan, state);

            if (state == GPI_CHAN_ALLOCATED)
            {
               init_chan_regs(gpii, chan);
            }
         }
         else
         {
            result.chan = GPI_EVT_RING;
            ring_info = &gpii->evt_ring_info;

            /* Only 1 event ring per GPII, no need to read this register to figure out which event state changed */
            /* bmsk = HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_INMI(qup->gsi_pa, gpii->id, HWIO_EE_n_CNTXT_SRC_EV_CH_IRQ_EV_CH_BIT_MAP_BMSK); */

            state = (GPI_EVT_RING_STATE)GPI_READ_FIELD(evt->dword_2, LOCAL_EVT_CHAN_STATE);
            gpi_log(GPI_DEBUG, 2, "helper_evt - gpii-%d evt ring state: %d\n",
                    gpii->id, state);

            if (state == GPI_CHAN_ALLOCATED)
            {
               init_evt_ring_regs(gpii);

               /* Full wp value */
               wp = REVERSE_RING_PTR(gpii->evt_ring_info.base_pa,
                                     get_ring_ptr(gpii, 0, EVT_RING_RP),
                                     gpii->evt_ring_info.num_elem);

               /* Ring evt ring DB with the WP */
               if (gpi->env_64bit)
               {
                  HWIO_EE_n_EV_CH_k_DOORBELL_1_OUTI2(qup->gsi_pa, gpii->id, 0,
                                                     ((uint64)get_system_address(wp) >> 32));
               }
               else
               {
                  HWIO_EE_n_EV_CH_k_DOORBELL_1_OUTI2(qup->gsi_pa, gpii->id, 0, 0);
               }
               HWIO_EE_n_EV_CH_k_DOORBELL_0_OUTI2(qup->gsi_pa, gpii->id, 0,
                                                  (get_system_address(wp) & 0xFFFFFFFF));
            }
            else if (state == GPI_CHAN_DISABLED)
            {
               ring_info->local_rp = ring_info->base_pa;
            }
         }

         ring_info->state = (GPI_CHAN_STATE)state;

         if (gpii->pending_cmd != GPI_CHAN_CMD_NONE)
         {
            result.type = EVT_CMD_COMPLETION;
            result.user_data = gpii->cmd_user_data;
         }
         else
         {
            /* unexpected state change */
            result.type = GPI_EVT_OTHER;
            result.user_data = gpii->user_data;
         }

         gpii->pending_cmd = GPI_CHAN_CMD_NONE;
         result.dword_0 = ring_info->state;

         result.code = EVT_COMPLETION_OTHER;
         result.tre_idx = gpii->id;
         break;

      case GPI_IRQ_GLOB_EE:
      case GPI_IRQ_GENERAL:
         if (helper_action == GPI_IRQ_GLOB_EE)
         {
            val = GPI_READ_FIELD(evt->dword_2, LOCAL_EVT_GLOB);

            if (val == HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_ERROR_INT_BMSK)
            {
               result.type = GPI_EVT_OTHER;
               result.dword_0 = GPI_ERROR_GLOBAL;
               result.dword_1 = gpii->iface_err;
               gpi_log(GPI_DEBUG, 3, "helper_evt - gpii-%d, GPI_IRQ_GLOB_EE - %d, err: %d\n",
                       gpii->id, val, gpii->iface_err);
               gpii->iface_err = 0;
            }
            else if (val == HWIO_EE_n_CNTXT_GLOB_IRQ_STTS_GP_INT1_BMSK)
            {
               result.type = EVT_CMD_COMPLETION;
               result.dword_0 = GENERIC_CMD_IBI;
               result.dword_1 = 0;
               gpi_log(GPI_DEBUG, 2, "helper_evt - gpii-%d, GPI_IRQ_GLOB_EE - %d\n",
                       gpii->id, val);
            }
         }
         else
         {
            result.type = GPI_EVT_OTHER;

            val = GPI_READ_FIELD(evt->dword_2, LOCAL_EVT_INT_TYPE);
            gpi_log(GPI_DEBUG, 2, "helper_evt - gpii-%d, GPI_IRQ_GENERAL, LOCAL_EVT_INT_TYPE: %d\n",
                    gpii->id, val);

            /* Find out the interrupt type */
            /* bit-0 "GSI_BREAK_POINT"       - MCS code break-point */
            if (val & HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_BREAK_POINT_BMSK)
            {
               gpi_log(GPI_DEBUG, 1, "helper_evt - gpii-%d, GPI_IRQ_GENERAL, GSI_BREAK_POINT\n",
                       gpii->id);
               result.dword_0 = BREAK_POINT;
            }

            /* bit-1 "GSI_BUS_ERROR"         - Bus Error interrupt */
            if (val & HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_BUS_ERROR_BMSK)
            {
               gpi_log(GPI_DEBUG, 1, "helper_evt - gpii-%d, GPI_IRQ_GENERAL, GSI_BUS_ERROR\n",
                       gpii->id);
               result.dword_0 = BUS_ERROR;
            }

            /* bit-2 "GSI_CMD_FIFO_OVRFLOW"  - Command Queue Overflow Error */
            if (val & HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_CMD_FIFO_OVRFLOW_BMSK)
            {
               gpi_log(GPI_DEBUG, 1, "helper_evt - gpii-%d, GPI_IRQ_GENERAL, GSI_CMD_FIFO_OVRFLOW\n",
                       gpii->id);
               result.dword_0 = CMD_FIFO_OVRFLOW;
            }

            /* bit-3 "GSI_MCS_STACK_OVRFLOW" - Stack Overflow Error */
            if (val & HWIO_EE_n_CNTXT_GSI_IRQ_STTS_GSI_MCS_STACK_OVRFLOW_BMSK)
            {
               gpi_log(GPI_DEBUG, 1, "helper_evt - gpii-%d, GPI_IRQ_GENERAL, GSI_MCS_STACK_OVRFLOW\n",
                       gpii->id);
               result.dword_0 = MCS_STACK_OVRFLOW;
            }
         }

         result.tre_idx = gpii->id;
         result.user_data = gpii->user_data;
         break;

      default:
         break;
   }

   gpi_syncleave(&gpii->gpii_mutex);

   /* pass result to the client */
   if (gpii->cb != NULL)
   {
      gpii->cb(&result);
      gpi_log(GPI_DEBUG, 1, "helper_evt - gpii-%d, Invoked cb\n", gpii->id);
   }
   else
   {
      gpi_log(GPI_WARNING, 1, "helper_evt - WARN - gpii-%d, cb NULL\n", gpii->id);
   }
}

/**
 * @brief Checks event rings for new events
 *
 * @param[in ]        gpi           GPI context
 * @param[in ]        gpii          GPI instance
 * 
 * @return            0 if no events are found, 1 otherwise.
 */
uint32 handle_ring_evts(gpi_ctxt *gpi, gpii_info *gpii)
{
   gpi_ring_elem *evt;
   U_LONG rp;
   uint8 chan;
   U_LONG ptr;
   gpi_result_type result;
   qup_instance *qup = &gpi->qup[gpii->qup_type];

   rp = get_ring_ptr(gpii, 0, EVT_RING_RP);

   if (gpii->evt_ring_info.local_rp == rp)
   {
      gpi_log(GPI_WARNING, 1, "ring_evts - WARN - evt ring %d empty\n", gpii->id);
      return 0;
   }

   while (gpii->evt_ring_info.local_rp != rp)
   {
      gpi_syncenter(&gpii->gpii_mutex);

      evt = (gpi_ring_elem *)GET_RING_VIRT_ADDR(&gpii->evt_ring_info, gpii->evt_ring_info.local_rp);

#ifndef GPI_ISLAND_MODE_ENABLE
      gpi_cachecmd(GPI_CACHE_INVALIDATE, (U_LONG)evt, sizeof(gpi_ring_elem));
#endif

      chan = GPI_READ_FIELD(evt->ctrl, EVT_CHID);
      gpi_log(GPI_DEBUG, 6, "ring_evts - gpii-%d chan %d, dword_0: 0x%x, dword_1: 0x%x, dword_2: 0x%x, ctrl: 0x%x\n",
              gpii->id, chan, evt->dword_0, evt->dword_1, evt->dword_2, evt->ctrl);
      if (chan >= NUM_RINGS_PER_GPII)
      {
         gpi_syncleave(&gpii->gpii_mutex);
         gpi_log(GPI_ERROR, 1, "ring_evts - ERR - unexpected chan: %d\n", chan);
         return 0;
      }

      gpi_memset(&result, 0, sizeof(gpi_result_type));

      result.type = (GPI_EVT_TYPE)GPI_READ_FIELD(evt->ctrl, EVT_TYPE);
      result.chan = (GPI_CHAN_TYPE)chan;

      switch (result.type)
      {
         case EVT_XFER_COMPLETION:
            result.status = GPI_READ_FIELD(evt->ctrl, EVT_STATUS);
            ptr = evt->dword_0;
            if (gpi->env_64bit)
            {
               ptr |= (uint64)evt->dword_1 << 32;
            }
            ptr = get_local_address(ptr);

            gpi_log(GPI_DEBUG, 5, "ring_evts - gpii-%d, chan-%d, base_pa: 0x%x, ptr: 0x%x, idx: %d\n",
                    gpii->id,
                    chan,
                    gpii->chan_info[chan].base_pa,
                    ptr,
                    GET_RING_ELEMENT_INDEX(gpii->chan_info[chan].base_pa, ptr));
            result.user_data = gpii->transf_ctxt[chan][GET_RING_ELEMENT_INDEX(gpii->chan_info[chan].base_pa, ptr)];
            gpi_log(GPI_DEBUG, 2, "ring_evts - gpii: %d, result.user_data: 0x%llx\n",
                    gpii->id, (uint64)result.user_data);

            result.code = (GPI_EVT_CODE)GPI_READ_FIELD(evt->dword_2, EVT_CODE);
            result.length = GPI_READ_FIELD(evt->dword_2, EVT_XFER_LEN);
            break;

         case EVT_IMM_DATA:
         case EVT_QUP_NOTIF:
            if (result.type == EVT_IMM_DATA)
            {
               result.status = GPI_READ_FIELD(evt->ctrl, EVT_STATUS);
               result.length = GPI_READ_FIELD(evt->dword_2, EVT_IMM_DATA_LEN);
               result.tre_idx = GPI_READ_FIELD(evt->dword_2, EVT_IMM_DATA_TRE_IDX);
            }
            else
            {
               result.length = GPI_READ_FIELD(evt->dword_2, EVT_QUP_COUNT);
            }

            result.code = (GPI_EVT_CODE)GPI_READ_FIELD(evt->dword_2, EVT_CODE);
            result.dword_0 = evt->dword_0;
            result.dword_1 = evt->dword_1;
            result.tre_idx = gpii->id;
            result.user_data = gpii->user_data;
            break;

         case EVT_I3C_IBI_OWN_CHG:
            result.dword_0 = GPI_READ_FIELD(evt->dword_0, I3C_IBI_PREV_OWN);
            result.dword_1 = GPI_READ_FIELD(evt->dword_1, I3C_IBI_CURR_OWN);
            result.user_data = gpii->user_data;
            break;

         case EVT_I3C_IBI_REC:
            result.dword_0 = evt->dword_0;  /* IBI Info Status */
            result.dword_1 = evt->dword_1;  /* IBI Data        */
            result.tre_idx = evt->dword_2;  /* Timestamp       */
            result.user_data = gpii->user_data;
            break;

         default:
            /* EVT_COMPLETION_STATUS_INVALID / EVT_COMPLETION_STATUS_UNDEFINED / EVT_COMPLETION_STATUS_MALFORMED */
            gpi_log(GPI_ERROR, 3, "ring_evts - ERR - default: %d, code: %d, chan: %d\n",
                    (uint32)result.type, (uint32)result.code, chan);
            break;
      }

      /* Consumed one evt, recycle one */
      if (gpi->env_64bit)
      {
         HWIO_EE_n_EV_CH_k_DOORBELL_1_OUTI2(qup->gsi_pa, gpii->id, 0,
                                            (uint32)((uint64)get_system_address(gpii->evt_ring_info.local_rp) >> 32));
      }
      else
      {
         HWIO_EE_n_EV_CH_k_DOORBELL_1_OUTI2(qup->gsi_pa, gpii->id, 0, 0);
      }
      HWIO_EE_n_EV_CH_k_DOORBELL_0_OUTI2(qup->gsi_pa, gpii->id, 0,
                                         (uint32)(get_system_address(gpii->evt_ring_info.local_rp) & 0xFFFFFFFF));

      ADVANCE_RING_PTR(gpii->evt_ring_info.base_pa,
                       &gpii->evt_ring_info.local_rp,
                       gpii->evt_ring_info.num_elem);

      gpi_syncleave(&gpii->gpii_mutex);

      /* Check for new events */
      rp = get_ring_ptr(gpii, 0, EVT_RING_RP);

      /* Client callback */
      if (gpii->cb != NULL)
      {
         gpii->cb(&result);
         gpi_log(GPI_DEBUG, 2, "ring_evts - gpii-%d, chan-%d, Invoked cb\n", gpii->id, chan);
      }
      else
      {
         gpi_log(GPI_WARNING, 1, "ring_evts - WARN - gpii-%d, cb NULL\n", gpii->id);
      }

      gpi_log(GPI_DEBUG, 3, "ring_evts - gpii-%d, local evt rp: 0x%llx, evt rp: 0x%llx\n",
              gpii->id, (uint64)gpii->evt_ring_info.local_rp, (uint64)rp);
   }

   return 1;
}

/**
 * @brief Handles all GPI events
 *
 * @param[in ]        gpi           GPI context
 */
void handle_gpi_evts(gpi_ctxt *gpi)
{
   uint32 i, j;
   gpi_ring_elem evt;

   /* Handle transfer completion events */
   for (i = 0; i < MAX_NUM_QUP_BLOCKS; i++)
   {
      if (gpi->qup[i].active)
      {
         while (gpi->qup[i].evt_ring_bmsk)
         {
            for (j = 0; j < NUM_GPII_PER_QUP; j++)
            {
               if (gpi->qup[i].evt_ring_bmsk & (1 << j))
               {
                  gpi_syncenter(&gpi->qup[i].qup_mutex);
                  gpi->qup[i].evt_ring_bmsk &= ~(1 << j);

                  if (!gpi->qup[i].active_gpii_bmsk)
                  {
                     gpi_log(GPI_WARNING, 0, "handle_gpi_evts - WARN - xfer No active GPII\n");
                     gpi_syncleave(&gpi->qup[i].qup_mutex);
                     return;
                  }
                  gpi_syncleave(&gpi->qup[i].qup_mutex);

                  if (0 == handle_ring_evts(gpi, gpi->qup[i].gpii[j]))
                  {
                     gpi_log(GPI_WARNING, 0, "gpi_evts - WARN -  INT with no events!!\n");
                     /* ERROR handling, check for ERRORs... */
                  }
               }
            }
         }
      }
   }

   /* Handle non-transfer events if any are queued */
   while (FALSE != de_q(&gpi->gpi_thread.q, &evt))
   {
      gpi_log(GPI_DEBUG, 4, "gpi_evts - evt.dword_0: 0x%x, evt.dword_1: 0x%x, evt.dword_2: 0x%x, evt.ctrl: 0x%x\n",
              evt.dword_0, evt.dword_1, evt.dword_2, evt.ctrl);
      handle_helper_evt(gpi, &evt);
   }
}

#ifdef GPI_MT
/**
 * @brief Terminates a given thread.
 *
 * @param[in ]        Thread            Executing Thread
 */
void terminate_thread(gpi_thread_type *gpi_thread)
{
   gpi_log(GPI_DEBUG, 0, "GPI Thread Terminating\n");
   gpi_thread->state = GPI_THREAD_INITIAL;
}


/**
 * @brief Re-Entrant Thread handler.
 *
 * @param[in,out ]    gpi_ctxt           GPI context 
 */
void gpi_worker_thread(void *ctxt)
{
   gpi_ctxt *gpi;
   gpi_thread_type *gpi_thread;
   unsigned int sigs;

   if (ctxt == NULL)
   {
      gpi_log(GPI_ERROR, 0, "worker_thread - ERR - bad params\n");
      return;
   }

   gpi = (gpi_ctxt *)ctxt;
   gpi_thread = &gpi->gpi_thread;

   gpi_eventreset(&gpi_thread->thread.hEvent);

   /* Wait for event */
   for (;;)
   {
      sigs = gpi_eventwait(&gpi_thread->thread.hEvent);

      if (sigs & GPI_SIG_MASK)
      {
         /* Reset the event to allow it to be retriggered */
         gpi_eventreset(&gpi_thread->thread.hEvent);

         if (gpi_thread->state == GPI_THREAD_TERMINATING)
         {
            gpi_log(GPI_DEBUG, 0, "worker_thread - getting terminated\n");
            terminate_thread(gpi_thread);
         }
         else
         {
            if (gpi_thread->state != GPI_THREAD_STOPPED)
            {
               gpi_thread->state = GPI_THREAD_RUNNING;
               handle_gpi_evts(gpi);
            }

            if (gpi_thread->state == GPI_THREAD_TERMINATING)
            {
               gpi_log(GPI_DEBUG, 0, "worker_thread - Terminating!\n");
               terminate_thread(gpi_thread);
               break;
            }
            else if (gpi_thread->state != GPI_THREAD_STOPPED)
            {
               gpi_thread->state = GPI_THREAD_WAITING;
            }
         }
      }
   }
}


/**
 * @brief Spawn GPI Threads.
 *
 * @param[in,out ]     gpi_ctxt           GPI context 
 */
GPI_RETURN_STATUS spawn_threads(gpi_ctxt *gpi)
{
   if (GPI_STATUS_SUCCESS != gpi_spawn_thread(&gpi->gpi_thread.thread,
                                              "gpi",
                                              &gpi_worker_thread,
                                              (void *)gpi,
                                              gpi->gpi_thread.priority))
   {
      gpi_log(GPI_ERROR, 0, "spawn_threads - ERR - spawn thread fail!");
      return GPI_STATUS_ERROR;
   }

   return GPI_STATUS_SUCCESS;
}


/**
 * @brief Stop GPI threads
 *
 * @param[in,out ]     gpi_ctxt           GPI context 
 * @param[in ]         terminate          Terminate threads if TRUE, otherwise stops them 
 * @param[in ]         wait               Wait time in msec 
 *
 * @return       GPI_RETURN_STATUS     GPI_STATUS_SUCCESS for success, GPI_STATUS_ERROR otherwise.
 *
 */
GPI_RETURN_STATUS stop_thread(gpi_ctxt *gpi, boolean terminate, uint32 wait)
{
   uint32 timeout;

   if (gpi->gpi_thread.state != GPI_THREAD_INITIAL)
   {
      if (terminate)
      {
         gpi->gpi_thread.state = GPI_THREAD_TERMINATING;
         gpi_eventsignal(&gpi->gpi_thread.thread.hEvent);
      }
      else
      {
         gpi->gpi_thread.state = GPI_THREAD_STOPPED;
      }
   }

   if (terminate)
   {
      /* Bail if not done within "wait" msec */
      timeout = 0;
      while (timeout < wait)
      {
         gpi_sleep(5000);
         if (gpi->gpi_thread.state == GPI_THREAD_INITIAL)
         {
            gpi_log(GPI_DEBUG, 0, "stop_thread - Thread terminated\n");
            break;
         }
         timeout += 5;
      }

      if (timeout >= wait)
      {
         gpi_log(GPI_WARNING, 0, "stop_thread - WARN - Failed to terminate thread\n");
      }
   }

   return GPI_STATUS_SUCCESS;
}

#endif
