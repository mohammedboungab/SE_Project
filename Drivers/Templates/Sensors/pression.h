/*
 * pression.h
 *
 *  Created on: Dec 23, 2025
 *      Author: imejnoun
 */

#ifndef TEMPLATES_PRESSION_H_
#define TEMPLATES_PRESSION_H_

#include "main.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//Ecran include
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
//
#include "stm32f7xx_hal.h"


#include <stdio.h>
#include <string.h>

#include "lps22hh_reg.h"

#define SENSOR_BUS hi2c1

#define    BOOT_TIME        5 //ms

#define TX_BUF_DIM          1000

static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);

uint8_t start_sensor_lps22hh(void);
void get_values_pressure_sensor_lps22hh(void);
static void platform_delay(uint32_t ms);



#endif /* TEMPLATES_PRESSION_H_ */
