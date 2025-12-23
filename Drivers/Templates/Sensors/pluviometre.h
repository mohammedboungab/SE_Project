/*
 * pluviometre.h
 *
 *  Created on: Dec 23, 2025
 *      Author: imejnoun
 *      but: Définition les prototypes des fonctions d'acquistion du pluviomettre
 */

#ifndef TEMPLATES_SENSORS_PLUVIOMETRE_H_
#define TEMPLATES_SENSORS_PLUVIOMETRE_H_

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
//
#include "stm32f7xx_hal.h"


#include <stdio.h>
#include <string.h>

//Prototypes

void detect_pluie();
void Get_Wind_Speed();
char* getWindDirection(float voltage);
void Read_ADC2_Channel1();

#endif /* TEMPLATES_SENSORS_PLUVIOMETRE_H_ */
