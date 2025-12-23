/*
 * display_reg.h
 *
 *  Created on: Dec 23, 2025
 *      Author: imejnoun
 */

#ifndef TEMPLATES_SCREENS_DISPLAY_REG_H_
#define TEMPLATES_SCREENS_DISPLAY_REG_H_

#include "main.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

#include "humidity.h"
#include "pression.h"



void show_sensors();
void error(uint8_t *message);
void TouchScreen();
void show_rain();
void Home();
#endif /* TEMPLATES_SCREENS_DISPLAY_REG_H_ */
