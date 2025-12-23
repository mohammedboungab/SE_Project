/*
 * display_reg.c
 *
 *  Created on: Dec 23, 2025
 *      Author: imejnoun
 */

#include "display_reg.h"
#include "pression.h"
#include "humidity.h"
#include "pluviometre.h"
#include "tim.h"
#include <stdio.h>
#include <string.h>

static uint8_t tx_buffer[1000];
extern volatile float pressure_hPa;
extern volatile float rainfallAmount, windSpeedKmph;
extern volatile WeatherSensorData_t currentSensorData;
extern volatile uint8_t page;
extern char *direction;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim2;

/* External Functions ---------------------------------------------------------*/
extern void start_again_timer(TIM_HandleTypeDef htim);




void show_sensors(){
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    
    // Configuration des couleurs et police
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    
    // Acquisition des données
    get_values_pressure_sensor_lps22hh();
    get_grandeur_values_sensor_hts221();
    
    // Affichage en liste simple alignée à gauche
    snprintf((char *)tx_buffer, sizeof(tx_buffer), "Temperature: %.2f C", currentSensorData.temp);
    BSP_LCD_DisplayStringAt(20, 40, (uint8_t *)tx_buffer, LEFT_MODE);
    
    snprintf((char *)tx_buffer, sizeof(tx_buffer), "Humidite: %.2f %%", currentSensorData.hum);
    BSP_LCD_DisplayStringAt(20, 80, (uint8_t *)tx_buffer, LEFT_MODE);
    
    snprintf((char *)tx_buffer, sizeof(tx_buffer), "Pression: %.2f hPa", pressure_hPa);
    BSP_LCD_DisplayStringAt(20, 120, (uint8_t *)tx_buffer, LEFT_MODE);
    
    // Bouton Suivant en bas à droite
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(350, 240, (uint8_t *)"[ SUIVANT > ]", LEFT_MODE);
}

void show_rain(){
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    
    // Configuration des couleurs et police
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font16);
    
    // Acquisition des données
    Get_Wind_Speed();
    detect_pluie();
    Read_ADC2_Channel1();
    
    // Affichage en liste simple alignée à gauche
    snprintf((char *)tx_buffer, sizeof(tx_buffer), "Pluie: %.2f mm", rainfallAmount);
    BSP_LCD_DisplayStringAt(20, 40, (uint8_t *)tx_buffer, LEFT_MODE);
    
    snprintf((char *)tx_buffer, sizeof(tx_buffer), "Vent: %.2f km/h", windSpeedKmph);
    BSP_LCD_DisplayStringAt(20, 80, (uint8_t *)tx_buffer, LEFT_MODE);
    
    snprintf((char *)tx_buffer, sizeof(tx_buffer), "Direction: %s", direction);
    BSP_LCD_DisplayStringAt(20, 120, (uint8_t *)tx_buffer, LEFT_MODE);
    
    // Bouton Suivant en bas à droite
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(350, 240, (uint8_t *)"[ SUIVANT > ]", LEFT_MODE);
}


// Fonctions obsolètes supprimées : setDrawText, base_screen, sensors_screen, raie_screen, DrawBlock
void Home(void){
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    
    // Configuration des couleurs et police
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);
    
    // Menu vertical centré
    BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"-> 5 Secondes", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 140, (uint8_t *)"-> 10 Minutes", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 200, (uint8_t *)"-> 1 Heure", CENTER_MODE);
}


void error(uint8_t * message) {
    // Initialisation du LCD
    BSP_LCD_Init();

    // Initialisation des couches LCD
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

    // Configuration de la police, des couleurs et du fond
    BSP_LCD_SetFont(&Font16); // Choisissez une police lisible
    BSP_LCD_SetBackColor(LCD_COLOR_RED); // Fond rouge
    BSP_LCD_Clear(LCD_COLOR_RED); // Efface l'écran avec le fond rouge
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE); // Texte blanc

    // Affichage du message
    BSP_LCD_DisplayStringAt(10, 50, message, LEFT_MODE);
}

void TouchScreen(){
	    TS_StateTypeDef ts = {0};
	    static TS_StateTypeDef prev_state = {0};
	    static uint8_t is_touching = 0;

	    BSP_TS_GetState(&ts);


	    if (ts.touchDetected) {
	        // If a touch is detected and no touch was previously active
	    	start_again_timer(htim2);
	        if (!is_touching) {
	            is_touching = 1;  // Mark as touching
	            if(page == 0){
	            	// Zone tactile pour "-> 5 Secondes" (Y entre 70-110, large zone horizontale)
	            	if ((ts.touchX[0] >= 50 && ts.touchX[0] <= 430) &&
	            	    (ts.touchY[0] >= 70 && ts.touchY[0] <= 110))
	            	{
						 HAL_TIM_Base_Stop(&htim5);
						__HAL_TIM_SET_PRESCALER(&htim5, 9999);
						__HAL_TIM_SET_AUTORELOAD(&htim5, 49999);
						HAL_TIM_Base_Start(&htim5);
	            	    page = 1;
	            	}
	            	// Zone tactile pour "-> 10 Minutes" (Y entre 130-170, large zone horizontale)
	            	else if ((ts.touchX[0] >= 50 && ts.touchX[0] <= 430) &&
	            		    (ts.touchY[0] >= 130 && ts.touchY[0] <= 170))
	            		{
	            		 	HAL_TIM_Base_Stop(&htim5);
	            		    __HAL_TIM_SET_PRESCALER(&htim5, 9999);
	            		    __HAL_TIM_SET_AUTORELOAD(&htim5, 5999999);
	            		    HAL_TIM_Base_Start(&htim5);
							page = 1;
	            		}
	            	// Zone tactile pour "-> 1 Heure" (Y entre 190-230, large zone horizontale)
	            	else if ((ts.touchX[0] >= 50 && ts.touchX[0] <= 430) &&
	            		    (ts.touchY[0] >= 190 && ts.touchY[0] <= 230))
	            		{
							 HAL_TIM_Base_Stop(&htim5);
							__HAL_TIM_SET_PRESCALER(&htim5, 9999);
							__HAL_TIM_SET_AUTORELOAD(&htim5, 35999999);
							HAL_TIM_Base_Start(&htim5);
							htim5.Instance->PSC = 9999;
							htim5.Instance->ARR = 35999999;
							page = 1;
	            		}

	            }
	            else{
		            // Zone tactile pour le bouton "Suivant" en bas à droite (X > 340, Y > 230)
		            if ((ts.touchX[0] >= 340 && ts.touchX[0] <= 480) &&
		            	(ts.touchY[0] >= 230 && ts.touchY[0] <= 272)) {
		            	if(page == 1){
		            		show_rain();
		            	    page = 2;
		            	}
		            	else if(page == 2){
		            		show_sensors();
		            	    page = 1;
		            	}
		            }
		        }}
	        // Update previous state
	        prev_state.touchX[0] = ts.touchX[0];
	        prev_state.touchY[0] = ts.touchY[0];
	    } else {
	        // If no touch is detected, reset the touch state
	        is_touching = 0;
	    }
}
