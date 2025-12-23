/*
 * pluviometre.c
 *
 *  Created on: Dec 23, 2025
 *      Author: imejnoun
 *      but: Implimente des fonctions d'acquistion du pluviomettre
 */
#include "pluviometre.h"
#include <stdio.h>
#include <string.h>

volatile float rainfallAmount = 0.0, windSpeedKmph = 0.0, voltage = 0.0;
volatile uint32_t interrupt_count = 0;
volatile float tick_count;
volatile  char* direction;
extern ADC_HandleTypeDef hadc2;

void detect_pluie()
    {
        //Exemple d'utilisation du compteur d'interruptions
         rainfallAmount = interrupt_count * 0.2794; // Calcul de la pluie en mm
    }
//////////////////////////////////////////////////////////////////////
void Get_Wind_Speed() {
   // Récupération des ticks capturés
     float ticks_per_second = tick_count;

   // Réinitialisation du compteur pour la prochaine mesure
   tick_count = 0;

   // Conversion des ticks/s en vitesse (km/h)
   windSpeedKmph = (ticks_per_second/5) * 2.4;

   }

///////////////////////////////////////////////////////////////////////////////
 char* getWindDirection(float voltage) {
	  if ((voltage >=0.7) & (voltage <= 0.9)) return "OUEST";
    else if ((voltage > 0.9) & (voltage <= 1.35)) return "NORD-OUEST";
    else if ((voltage >1.35) & (voltage <= 1.55)) return "NORD";
    else if ((voltage >2.2) &(voltage <= 2.4)) return "NORD-EST";
    else if ((voltage >3.03) & (voltage <= 3.23)) return "EST";
    else if ((voltage >2.86) & (voltage <= 3.02)) return "SUD-EST";
    else if ((voltage > 2.66) & (voltage <= 2.86)) return "SUD";
    else if ((voltage > 1.8) & (voltage <= 2.0)) return "SUD-OUEST";
    else return "Inconnu";
}

void Read_ADC2_Channel1() {
    HAL_ADC_Start(&hadc2);
    if (HAL_ADC_PollForConversion(&hadc2, 1000) == HAL_OK) {
        uint32_t adcValue = HAL_ADC_GetValue(&hadc2);
        voltage = (adcValue/ 4095.0)*3.3;  // Conversion en volts
        HAL_ADC_Stop(&hadc2);
    }
    direction = getWindDirection(voltage);
}
