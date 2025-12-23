/*
 * humidity.c
 *
 *  Created on: Dec 23, 2025
 *      Author: imejnoun
 */

#include "pression.h"
#include <stdio.h>
#include <string.h>

#include "hts221_reg.h"
#include "humidity.h"


static int16_t data_raw_humidity;
static int16_t data_raw_temperature;
static float humidity_perc;
static float temperature_degC;
static uint8_t whoamI;
//static uint8_t tx_buffer[1000];

static stmdev_ctx_t dev_ctx;
static lin_t lin_hum;
static lin_t lin_temp;

float linear_interpolation(lin_t *lin, int16_t x)
{
  return ((lin->y1 - lin->y0) * x + ((lin->x1 * lin->y0) -
                                     (lin->x0 * lin->y1)))
         / (lin->x1 - lin->x0);
}

volatile WeatherSensorData_t currentSensorData;

uint8_t start_sensor_hts221(){
	//Cette fonction initialise le driver de capteurs temp & humidité

  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  //dev_ctx.mdelay = platform_delay;
  dev_ctx.handle = &SENSOR_BUS;


  whoamI = 0;
  hts221_device_id_get(&dev_ctx, &whoamI);

  if ( whoamI != HTS221_ID ) return -1;
	// while (1); /*manage here device not found */
	/* Read humidity calibration coefficient */
	hts221_hum_adc_point_0_get(&dev_ctx, &lin_hum.x0);
	hts221_hum_rh_point_0_get(&dev_ctx, &lin_hum.y0);
	hts221_hum_adc_point_1_get(&dev_ctx, &lin_hum.x1);
	hts221_hum_rh_point_1_get(&dev_ctx, &lin_hum.y1);
	/* Read temperature calibration coefficient */
	hts221_temp_adc_point_0_get(&dev_ctx, &lin_temp.x0);
	hts221_temp_deg_point_0_get(&dev_ctx, &lin_temp.y0);
	hts221_temp_adc_point_1_get(&dev_ctx, &lin_temp.x1);
	hts221_temp_deg_point_1_get(&dev_ctx, &lin_temp.y1);
	/* Enable Block Data Update */
	hts221_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
	/* Set Output Data Rate */
	hts221_data_rate_set(&dev_ctx, HTS221_ODR_1Hz);
	/* Device power on */
	hts221_power_on_set(&dev_ctx, PROPERTY_ENABLE);

	return -1;
}

void get_grandeur_values_sensor_hts221(){
	//Cette fonction recupère les données issus du capteurs temp-hum
	hts221_reg_t reg;
    hts221_status_get(&dev_ctx, &reg.status_reg);

	if (reg.status_reg.h_da) {
	   /* Read humidity data */
	   memset(&data_raw_humidity, 0x00, sizeof(int16_t));
	   hts221_humidity_raw_get(&dev_ctx, &data_raw_humidity);
	   humidity_perc = linear_interpolation(&lin_hum, data_raw_humidity);

	   if (humidity_perc < 0) {
		 humidity_perc = 0;
	   }

	   if (humidity_perc > 100) {
		 humidity_perc = 100;
	   }

	   currentSensorData.hum = humidity_perc;
	   /*printf("******\r\n");
	   printf((char *)tx_buffer);
	   snprintf((char *)tx_buffer, sizeof(tx_buffer), "Humidity [%%]:%3.2f\r\n", humidity_perc);
	   printf((char *)tx_buffer);*/
	 }

	 if (reg.status_reg.t_da) {
	   /* Read temperature data */
	   memset(&data_raw_temperature, 0x00, sizeof(int16_t));
	   hts221_temperature_raw_get(&dev_ctx, &data_raw_temperature);
	   temperature_degC = linear_interpolation(&lin_temp,
											   data_raw_temperature);

	   currentSensorData.temp = temperature_degC;

	 }
}


static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
	 /* Write multiple command */
	  reg |= 0x80;
	  HAL_I2C_Mem_Write(handle, HTS221_I2C_ADDRESS, reg,
	                    I2C_MEMADD_SIZE_8BIT, (uint8_t*) bufp, len, 1000);
	  return 0;
}

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
	reg |= 0x80;
	  HAL_I2C_Mem_Read(handle, HTS221_I2C_ADDRESS, reg,
	                   I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
	  return 0;

}

static void platform_delay(uint32_t ms)
{
  HAL_Delay(ms);

}

//////////////////////////////////////////////////////////////////////
