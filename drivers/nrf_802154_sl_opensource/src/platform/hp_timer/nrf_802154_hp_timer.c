/*
 * Copyright (c) 2018 - 2020, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file contains implementation of the nRF 802.15.4 high precision timer abstraction.
 *
 * This implementation is built on top of the TIMER peripheral.
 * If SoftDevice RAAL is in use the TIMER peripheral is shared between RAAL and this module.
 *
 */

#include "platform/hp_timer/nrf_802154_hp_timer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "hal/nrf_timer.h"

#include "nrf_802154_sl_config.h"
#include "nrf_802154_sl_periphs.h"

/**@brief Timer instance. */
#define TIMER                 NRF_802154_HIGH_PRECISION_TIMER_INSTANCE

/**@brief Timer compare channel definitions. */
#define TIMER_CC_CAPTURE      NRF_TIMER_CC_CHANNEL1
#define TIMER_CC_CAPTURE_TASK NRF_TIMER_TASK_CAPTURE1

#define TIMER_CC_SYNC         NRF_TIMER_CC_CHANNEL2
#define TIMER_CC_SYNC_TASK    NRF_TIMER_TASK_CAPTURE2
#define TIMER_CC_SYNC_EVENT   NRF_TIMER_EVENT_COMPARE2
#define TIMER_CC_SYNC_INT     NRF_TIMER_INT_COMPARE2_MASK

#define TIMER_CC_EVT          NRF_TIMER_CC_CHANNEL3
#define TIMER_CC_EVT_TASK     NRF_TIMER_TASK_CAPTURE3
#define TIMER_CC_EVT_INT      NRF_TIMER_INT_COMPARE3_MASK

/**@brief Unexpected value in the sync compare channel. */
static uint32_t m_unexpected_sync;

/**@brief Get current time on the Timer. */
static inline uint32_t timer_time_get(void)
{
    nrf_timer_task_trigger(TIMER, TIMER_CC_CAPTURE_TASK);
    return nrf_timer_cc_get(TIMER, TIMER_CC_CAPTURE);
}

void nrf_802154_hp_timer_init(void)
{
    // Intentionally empty
}

void nrf_802154_hp_timer_deinit(void)
{
    // Intentionally empty
}

void nrf_802154_hp_timer_start(void)
{
    // Intentionally empty
}

void nrf_802154_hp_timer_stop(void)
{
    // Intentionally empty
}

uint32_t nrf_802154_hp_timer_sync_task_get(void)
{
    return nrf_timer_task_address_get(TIMER, TIMER_CC_SYNC_TASK);
}

void nrf_802154_hp_timer_sync_prepare(void)
{
    uint32_t past_time = timer_time_get() - 1;

    m_unexpected_sync = past_time;
    nrf_timer_cc_set(TIMER, TIMER_CC_SYNC, past_time);
}

bool nrf_802154_hp_timer_sync_time_get(uint32_t * p_timestamp)
{
    bool     result    = false;
    uint32_t sync_time = nrf_timer_cc_get(TIMER, TIMER_CC_SYNC);

    assert(p_timestamp != NULL);

    if (sync_time != m_unexpected_sync)
    {
        *p_timestamp = sync_time;
        result       = true;
    }

    return result;
}

uint32_t nrf_802154_hp_timer_timestamp_task_get(void)
{
    return nrf_timer_task_address_get(TIMER, TIMER_CC_EVT_TASK);
}

uint32_t nrf_802154_hp_timer_timestamp_get(void)
{
    return nrf_timer_cc_get(TIMER, TIMER_CC_EVT);
}

uint32_t nrf_802154_hp_timer_current_time_get(void)
{
    return timer_time_get();
}
