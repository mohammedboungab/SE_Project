#include "pression.h"
#include <stdio.h>
#include <string.h>

#include "lps22hh_reg.h"

static uint32_t data_raw_pressure;
volatile float pressure_hPa;
static uint8_t whoamI, rst;
//static uint8_t tx_buffer[TX_BUF_DIM];

static stmdev_ctx_t dev_ctx;
static lps22hh_reg_t reg;


uint8_t start_sensor_lps22hh(void){
	/* Initialize mems driver interface */
	  dev_ctx.write_reg = platform_write;
	  dev_ctx.read_reg = platform_read;
	  dev_ctx.mdelay = platform_delay;
	  dev_ctx.handle = &SENSOR_BUS;
	  /* Initialize platform specific hardware */
	  /* Wait sensor boot time */
	  platform_delay(800);
	  /* Check device ID */
	  whoamI = 0;
	  lps22hh_device_id_get(&dev_ctx, &whoamI);

	  if ( whoamI != LPS22HH_ID )
	    return -1; /*manage here device not found */

	  /* Restore default configuration */
	  lps22hh_reset_set(&dev_ctx, PROPERTY_ENABLE);

	  do {
	    lps22hh_reset_get(&dev_ctx, &rst);
	  } while (rst);

	  /* Enable Block Data Update */
	  lps22hh_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
	  /* Set Output Data Rate */
	  lps22hh_data_rate_set(&dev_ctx, LPS22HH_10_Hz_LOW_NOISE);

	  return -1;

}
void get_values_pressure_sensor_lps22hh(void)
{

  /* Read samples in polling mode (no int) */
    /* Read output only if new value is available */
    lps22hh_read_reg(&dev_ctx, LPS22HH_STATUS, (uint8_t *)&reg, 1);

    if (reg.status.p_da) {
      memset(&data_raw_pressure, 0x00, sizeof(uint32_t));
      lps22hh_pressure_raw_get(&dev_ctx, &data_raw_pressure);
      pressure_hPa = lps22hh_from_lsb_to_hpa( data_raw_pressure);
      //snprintf((char *)tx_buffer, sizeof(tx_buffer), "pressure [hPa]:%6.2f\r\n", pressure_hPa);
  }
}


static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
  HAL_I2C_Mem_Write(handle, LPS22HH_I2C_ADD_H, reg,
                    I2C_MEMADD_SIZE_8BIT, (uint8_t*) bufp, len, 1000);

  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{

  HAL_I2C_Mem_Read(handle, LPS22HH_I2C_ADD_H, reg,
                   I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);

  return 0;
}

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  tx_buffer     buffer to transmit
 * @param  len           number of byte to send
 *
 */


/*
 * @brief  platform specific delay (platform dependent)
 *
 * @param  ms        delay in ms
 *
 */
static void platform_delay(uint32_t ms)
{
  HAL_Delay(ms);

}

