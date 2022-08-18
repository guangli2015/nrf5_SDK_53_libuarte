/**
 * Copyright (c) 2014 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup uart_example_main main.c
 * @{
 * @ingroup uart_example
 * @brief UART Example Application main file.
 *
 * This file contains the source code for a sample application using UART.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif
#include <nrfx_timer.h>
#include <nrfx_dppi.h>
#include <nrfx_gpiote.h>
#include <hal/nrf_gpiote.h>
#include <hal/nrf_timer.h>
	#define PIN  28
#define DK_BOARD_LED_2  29
#define DK_BOARD_LED_3  30
#define DK_BOARD_LED_4  31
//#define ENABLE_LOOPBACK_TEST  /**< if defined, then this example will be a loopback test, which means that TX should be connected to RX to get data loopback. */

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


#ifdef ENABLE_LOOPBACK_TEST
/* Use flow control in loopback test. */
#define UART_HWFC APP_UART_FLOW_CONTROL_ENABLED

/** @brief Function for setting the @ref ERROR_PIN high, and then enter an infinite loop.
 */
static void show_error(void)
{

    bsp_board_leds_on();
    while (true)
    {
        // Do nothing.
    }
}


/** @brief Function for testing UART loop back.
 *  @details Transmitts one character at a time to check if the data received from the loopback is same as the transmitted data.
 *  @note  @ref TX_PIN_NUMBER must be connected to @ref RX_PIN_NUMBER)
 */
static void uart_loopback_test()
{
    uint8_t * tx_data = (uint8_t *)("\r\nLOOPBACK_TEST\r\n");
    uint8_t   rx_data;

    // Start sending one byte and see if you get the same
    for (uint32_t i = 0; i < MAX_TEST_DATA_BYTES; i++)
    {
        uint32_t err_code;
        while (app_uart_put(tx_data[i]) != NRF_SUCCESS);

        nrf_delay_ms(10);
        err_code = app_uart_get(&rx_data);

        if ((rx_data != tx_data[i]) || (err_code != NRF_SUCCESS))
        {
            show_error();
        }
    }
    return;
}
#else
/* When UART is used for communication with the host do not use flow control.*/
#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED
#endif
static const nrfx_timer_t m_timer0 = NRFX_TIMER_INSTANCE(0);
static const nrfx_timer_t m_timer1 = NRFX_TIMER_INSTANCE(1);
static const nrfx_timer_t m_timer2 = NRFX_TIMER_INSTANCE(2);
static void dppi_init(void)
{
	uint8_t dppi_ch_1, dppi_ch_2;
	uint32_t err = nrfx_dppi_channel_alloc(&dppi_ch_1);
	if (err != NRFX_SUCCESS) {
		printf("Err %d\n", err);
		return;
	}

	//err = nrfx_dppi_channel_alloc(&dppi_ch_2);
	//if (err != NRFX_SUCCESS) {
	//	printk("Err %d\n", err);
	//	return;
	//}

	

	//err = nrfx_dppi_channel_enable(dppi_ch_2);
	//if (err != NRFX_SUCCESS) {
	//	printk("Err %d\n", err);
	//	return;
	//}

	/* Tie it all together */
	//nrf_gpiote_publish_set(NRF_GPIOTE, NRF_GPIOTE_EVENT_IN_0, dppi_ch_1);
	//nrf_gpiote_publish_set(NRF_GPIOTE, NRF_GPIOTE_EVENT_IN_1, dppi_ch_2);
	nrf_timer_subscribe_set(m_timer0.p_reg, NRF_TIMER_TASK_START, dppi_ch_1);
        nrf_timer_publish_set(m_timer1.p_reg,NRF_TIMER_EVENT_COMPARE0,dppi_ch_1);
	//nrf_timer_subscribe_set(timer.p_reg, NRF_TIMER_TASK_STOP, dppi_ch_2);
	//nrf_timer_subscribe_set(timer.p_reg, NRF_TIMER_TASK_CAPTURE0,
	//			dppi_ch_2);

        nrf_gpiote_subscribe_set( NRF_GPIOTE_TASKS_OUT_0, dppi_ch_1);
       err = nrfx_dppi_channel_enable(dppi_ch_1);
	if (err != NRFX_SUCCESS) {
		printf("Err %d\n", err);
		return;
	}
}
static volatile uint32_t m_counter=0;

static void timer0_event_handler(nrf_timer_event_t event_type, void * p_context)
{
    ++m_counter;
   //  printf("mcount%d, ",m_counter);
}
/* Timer event handler. Not used since Timer1 and Timer2 are used only for PPI. */
static void empty_timer_handler(nrf_timer_event_t event_type, void * p_context)
{
printf("t1, ");
}
/** @brief Function for Timer 0 initialization.
 *  @details Timer 0 will be stopped and started by Timer 1 and Timer 2 respectively using PPI.
 *           It is configured to generate an interrupt every 100ms.
 */
static void timer0_init(void)
{
    // Check TIMER0 configuration for details.
    nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
    timer_cfg.frequency = NRF_TIMER_FREQ_31250Hz;
    ret_code_t err_code = nrfx_timer_init(&m_timer0, &timer_cfg, timer0_event_handler);
    APP_ERROR_CHECK(err_code);

    nrfx_timer_extended_compare(&m_timer0,
                                   NRF_TIMER_CC_CHANNEL0,
                                   nrfx_timer_ms_to_ticks(&m_timer0,
                                                             1000),
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   true);
}

/** @brief Function for Timer 1 initialization.
 *  @details Initializes TIMER1 peripheral to generate an event every 2 seconds. The events are
 *           generated at even numbers of seconds after starting the example (2, 4, 6 ...) and they
 *           are used to stop TIMER0 via PPI: TIMER1->EVENT_COMPARE[0] triggers TIMER0->TASK_STOP.
 */
static void timer1_init(void)
{
    // Check TIMER1 configuration for details.
    nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
    timer_cfg.frequency = NRF_TIMER_FREQ_31250Hz;
    ret_code_t err_code = nrfx_timer_init(&m_timer1, &timer_cfg, empty_timer_handler);
    APP_ERROR_CHECK(err_code);

    nrfx_timer_extended_compare(&m_timer1,
                                   NRF_TIMER_CC_CHANNEL0,
                                   nrfx_timer_ms_to_ticks(&m_timer1,
                                                             3000),
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   true);
}

void gpiote_init(void)
{
   

    nrfx_err_t err;

    err = nrfx_gpiote_init();
    if (err != NRFX_SUCCESS) {
       
        return;
    }

    nrfx_gpiote_out_config_t const out_config = NRFX_GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);

    /* Initialize output pin. SET task will turn the LED on,
     * CLR will turn it off and OUT will toggle it.
     */
    err = nrfx_gpiote_out_init(PIN, &out_config);
    if (err != NRFX_SUCCESS )
    {
        //LOG_ERR("nrfx_gpiote_out_init error: %08x", err);
    }

    //nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
    //timer_cfg.mode = NRF_TIMER_MODE_TIMER;
    //timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
    //err = nrfx_timer_init(&timer1, &timer_cfg, timer1_evt_handler);

    //nrfx_timer_extended_compare(&timer1, NRF_TIMER_CC_CHANNEL0, DIVIDER_COUNT/2, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

    //uint8_t channel;
    //err = nrfx_dppi_channel_alloc(&channel);
    //nrfx_gpiote_pin_t gpiote_mclk_sync_pin_out = PIN;

   // nrfx_gppi_channel_endpoints_setup(channel, nrfx_timer_event_address_get(&timer1, NRF_TIMER_EVENT_COMPARE0), nrfx_gpiote_out_task_addr_get(gpiote_mclk_sync_pin_out));
    //nrfx_gppi_channels_enable(BIT(channel));
    nrfx_gpiote_out_task_enable(PIN);
    //nrfx_timer_enable(&timer1);
}
/** @brief Function for Timer 2 initialization.
 *  @details Initializes TIMER2 peripheral to generate an event every 2 seconds. The events are
 *           generated at odd numbers of seconds after starting the example (3, 5, 7 ...) and they
 *           are used to start TIMER0 via PPI: TIMER2->EVENT_COMPARE[0] triggers TIMER0->TASK_START.
 */
//static void timer2_init(void)
//{
//    // Check TIMER2 configuration for details.
//    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
//    timer_cfg.frequency = NRF_TIMER_FREQ_31250Hz;
//    ret_code_t err_code = nrf_drv_timer_init(&m_timer2, &timer_cfg, empty_timer_handler);
//    APP_ERROR_CHECK(err_code);

//    nrf_drv_timer_extended_compare(&m_timer2,
//                                   NRF_TIMER_CC_CHANNEL0,
//                                   nrf_drv_timer_ms_to_ticks(&m_timer2,
//                                                             PPI_EXAMPLE_TIMER2_INTERVAL),
//                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
//                                   false);
//}
/**
 * @brief Function for main application entry.
 */
int main(void)
{
    uint32_t err_code;

  //  bsp_board_init(BSP_INIT_LEDS);
dppi_init();
    timer0_init(); // Timer used to increase m_counter every 100ms.
    timer1_init();
    gpiote_init();
    const app_uart_comm_params_t comm_params =
      {
          RX_PIN_NUMBER,
          TX_PIN_NUMBER,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          UART_HWFC,
          false,
#if defined (UART_PRESENT)
          NRF_UART_BAUDRATE_115200
#else
          NRF_UARTE_BAUDRATE_115200
#endif
      };

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    APP_ERROR_CHECK(err_code);

#ifndef ENABLE_LOOPBACK_TEST
    printf("\r\nUART example started.\r\n");
 nrfx_timer_enable(&m_timer0);
  nrfx_timer_enable(&m_timer1);
    while (true)
    {
        uint8_t cr;
        //bsp_board_led_invert(0);
        while (app_uart_get(&cr) != NRF_SUCCESS);
       // while (app_uart_put(cr) != NRF_SUCCESS);
       
        if (cr == 'q' || cr == 'Q')
        {
            printf(" \r\nExit!\r\n");
            printf("mcount%d, ",m_counter);
            //while (true)
            //{
            //    // Do nothing.
            //}
        }
    }
#else

    // This part of the example is just for testing the loopback .
    while (true)
    {
        uart_loopback_test();
    }
#endif
}


/** @} */
