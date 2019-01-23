/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
/**
  ******************************************************************************
  * @file    hw.c
  * @author  MCU China FAE team
  * @version 1.0
  * @date    23/11/2018
  * @brief   aos porting layer
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  ******************************************************************************
  */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <k_api.h>
#include <aos/log.h>
#include <hal/soc/soc.h>
#include <hal/soc/timer.h>
#include <hal/base.h>


#include "stm32l0xx_hal_cortex.h"


#define TAG "hw"

#define us2tick(us) \
    ((us * RHINO_CONFIG_TICKS_PER_SECOND + 999999) / 1000000)

	


void hal_reboot(void)
{
    HAL_NVIC_SystemReset();
}

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC>0)
static void _timer_cb(void *timer, void *arg)
{
    timer_dev_t *tmr = arg;
    tmr->config.cb(tmr->config.arg);
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), us2tick(tim->config.period), tim, 0);
    }
    else {
        krhino_timer_dyn_create((ktimer_t **)&tim->priv, "hwtmr", _timer_cb,
                                us2tick(tim->config.period), 0, tim, 0);
    }
}

int32_t hal_timer_start(timer_dev_t *tmr)
{
    return krhino_timer_start(tmr->priv);
}


void hal_timer_stop(timer_dev_t *tmr)
{
    krhino_timer_stop(tmr->priv);
    krhino_timer_dyn_del(tmr->priv);
    tmr->priv = NULL;
}
#else
void log_no_cli_init(void)
{
	/*to pass keil compliation in limited memrory 32K Flash + 8K RAM*/
}
#endif


void hw_start_hal(void)
{
    printf("start-----------hal\n");

}
