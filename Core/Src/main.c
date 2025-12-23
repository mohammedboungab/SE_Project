/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "dma2d.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "rtc.h"
#include "sdmmc.h"
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

#include "humidity.h"
#include "pression.h"
#include "pluviometre.h"
#include "display_reg.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* System State Definitions */
#define SYSTEM_STATE_ACQUISITION    1  /**< État d'acquisition active - LED verte */
#define SYSTEM_STATE_IDLE           0  /**< État veille écran - LED rouge */
#define SYSTEM_STATE_SAVE_SD        (-1) /**< État sauvegarde carte SD - LED bleue */

/* Maximum number of samples in history buffers */
#define MAX_SAMPLE_COUNT            5000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* External Variables ---------------------------------------------------------*/
extern volatile float pressure_hPa; /**< Pression atmosphérique en hectopascals */
extern volatile WeatherSensorData_t currentSensorData; /**< Données actuelles des capteurs température/humidité */
extern volatile float rainfallAmount; /**< Quantité de pluie cumulée en millimètres */
extern volatile float windSpeedKmph; /**< Vitesse du vent en kilomètres par heure */

/* System State Variables -----------------------------------------------------*/
volatile uint8_t systemState = SYSTEM_STATE_ACQUISITION; /**< État courant du système */
volatile uint8_t page = 0; /**< Page d'affichage courante (0: accueil, 1: capteurs, 2: météo) */

/* Timer Flags ----------------------------------------------------------------*/
volatile uint8_t TimerFlag_ScreenOff = 0;    /**< Drapeau d'interruption timer écran veille (TIM4) */
volatile uint8_t TimerFlag_LED = 0;         /**< Drapeau d'interruption timer LED (TIM7) */
volatile uint8_t TimerFlag_UserButton = 0;   /**< Drapeau d'interruption bouton utilisateur */
volatile uint8_t TimerFlag_SaveSD = 0;      /**< Drapeau d'interruption sauvegarde SD (TIM2) */
volatile uint8_t TimerFlag_Acquisition = 0; /**< Drapeau d'interruption acquisition périodique (TIM5) */

/* Data History Buffers -------------------------------------------------------*/
float tempHistory[MAX_SAMPLE_COUNT];  /**< Historique des températures */
float humHistory[MAX_SAMPLE_COUNT];  /**< Historique des humidités */
float rainHistory[MAX_SAMPLE_COUNT]; /**< Historique des précipitations */
float windHistory[MAX_SAMPLE_COUNT]; /**< Historique des vitesses de vent */
uint16_t sampleIndex = 0;            /**< Index courant dans les buffers d'historique */

/* FatFS Work Buffer ---------------------------------------------------------*/
uint8_t workBuffer[_MAX_SS];



extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

/**
 * @brief Stocke un lot de données capteurs dans la carte SD au format CSV
 * @param tempData Tableau des températures
 * @param humData Tableau des humidités
 * @param rainData Tableau des précipitations
 * @param windData Tableau des vitesses de vent
 * @param sampleCount Nombre d'échantillons à stocker
 */
void StoreSensorBatch(float *tempData, float *humData, float *rainData, float *windData, uint16_t sampleCount);

/**
 * @brief Détecte et calcule la quantité de pluie à partir des interruptions
 */
void detect_pluie();

/**
 * @brief Réinitialise le compteur du timer de veille écran
 * @param htim Handle du timer (non utilisé, utilise htim2 en interne)
 */
void start_again_timer(TIM_HandleTypeDef htim);



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  MX_ADC1_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_TIM7_Init();
  MX_TIM5_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */

  /* Initialisation des périphériques d'affichage et de la dalle tactile */
  BSP_LCD_Init();
  BSP_TS_Init(480, 272);

  /* Configuration du timer de gestion du touchscreen (TIM3) */
  HAL_TIM_Base_Init(&htim3);
  HAL_TIM_Base_Start_IT(&htim3);

  /* Affichage de l'écran d'accueil pour sélection du temps d'acquisition */
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
  BSP_LCD_SetLayerVisible(LTDC_ACTIVE_LAYER, ENABLE);
  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  Home();
  
  /* Attente de la sélection du temps d'acquisition par l'utilisateur */
  while(page == 0);

  /* Initialisation des capteurs environnementaux */
  if(start_sensor_hts221() == -1) {
    printf("Device for sensor hts221 not found!\r\n");
  }
  if(start_sensor_lps22hh() == -1) {
    printf("Device for sensor lps22hh not found!\r\n");
  }

  /* Configuration et affichage de la page principale des capteurs */
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
  show_sensors();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  /* Initialisation des timers périphériques */
  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Init(&htim4);
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_Base_Init(&htim7);

  /* Démarrage du timer d'acquisition de vitesse du vent (TIM1 Input Capture) */
  HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);

  /* Démarrage des timers d'interruption */
  HAL_TIM_Base_Start_IT(&htim4);  /* Timer veille écran */
  HAL_TIM_Base_Start_IT(&htim7);  /* Timer LED état */
  HAL_TIM_Base_Start_IT(&htim2);  /* Timer sauvegarde SD */
  HAL_TIM_Base_Start_IT(&htim5);  /* Timer acquisition périodique */

  /* Configuration des priorités d'interruption des timers */
  HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
  HAL_NVIC_SetPriority(TIM4_IRQn, 2, 0);
  HAL_NVIC_SetPriority(TIM7_IRQn, 1, 0);

  /* Initialisation de la LED verte pour indiquer l'état d'acquisition */
  HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);

  /* Boucle principale de gestion des états du système */
  while (1)
  {
    /* Traitement de l'interruption d'acquisition périodique (TIM5) */
    if(TimerFlag_Acquisition == 1){
      /* Mise à jour de l'affichage selon la page courante */
      if(page == 1) {
        show_sensors();
      }
      else if(page == 2) {
        show_rain();
      }

      /* Stockage des données dans les buffers d'historique */
      if(sampleIndex < MAX_SAMPLE_COUNT){
        tempHistory[sampleIndex] = currentSensorData.temp;
        humHistory[sampleIndex] = currentSensorData.hum;
        rainHistory[sampleIndex] = rainfallAmount;
        windHistory[sampleIndex] = windSpeedKmph;
        sampleIndex++;
      }
      else{
        /* Buffer saturé : réinitialisation pour éviter débordement */
        sampleIndex = 0;
      }
      TimerFlag_Acquisition = 0;
    }
    /* Traitement de l'interruption de veille écran (TIM4) */
    else if(TimerFlag_ScreenOff == 1){
      BSP_LCD_DisplayOff();
      systemState = SYSTEM_STATE_IDLE;
      TimerFlag_ScreenOff = 0;
    }
    /* Traitement de l'interruption de sauvegarde SD (TIM2) */
    else if(TimerFlag_SaveSD == 1){
      /* Sauvegarde périodique des données dans la carte SD */
      StoreSensorBatch(tempHistory, humHistory, rainHistory, windHistory, sampleIndex);
      TimerFlag_SaveSD = 0;
      /* Réinitialisation des buffers après sauvegarde */
      sampleIndex = 0;
    }
    /* Traitement de l'interruption de gestion LED (TIM7) */
    else if(TimerFlag_LED == 1){
      /* Mise à jour de la LED selon l'état du système */
      if(systemState == SYSTEM_STATE_ACQUISITION){
        HAL_GPIO_WritePin(red_led_GPIO_Port, red_led_Pin, GPIO_PIN_RESET);
        HAL_GPIO_TogglePin(green_led_GPIO_Port, green_led_Pin);
      }
      else{
        HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);
        HAL_GPIO_TogglePin(red_led_GPIO_Port, red_led_Pin);
      }
      TimerFlag_LED = 0;
    }
    /* Traitement de l'interruption bouton utilisateur */
    else if(TimerFlag_UserButton == 1){
      /* Réinitialisation du timer de veille écran */
      start_again_timer(htim2);
      /* Réactivation de l'affichage LCD */
      BSP_LCD_DisplayOn();
      /* Retour à l'état d'acquisition */
      systemState = SYSTEM_STATE_ACQUISITION;
      TimerFlag_UserButton = 0;
    }
    else{
      /* Mode veille : entrée en mode SLEEP pour économiser l'énergie */
      HAL_SuspendTick();
      HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
      HAL_ResumeTick();
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


void start_again_timer(TIM_HandleTypeDef htim){
  /* Réinitialisation du compteur du timer de veille écran (TIM2) */
  HAL_TIM_Base_Stop(&htim2);
  __HAL_TIM_SET_COUNTER(&htim2, 0);
  HAL_TIM_Base_Start(&htim2);
}
void StoreSensorBatch(float *tempData, float *humData, float *rainData, float *windData, uint16_t sampleCount) {
    char buffer[100];
    UINT byteswritten;
    FRESULT res;
    uint16_t i;

    /* Mount the file system object to the FatFs module */
    if (f_mount(&SDFatFS, (TCHAR const *)SDPath, 0) != FR_OK) {
        Error_Handler();
        return;
    }

    /* Create a FAT file system (format) on the logical drive */
    /* WARNING: Formatting the uSD card will delete all content on the device */
    printf("1er reussi\r\n");
    if (f_mkfs((TCHAR const *)SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer)) != FR_OK) {
        Error_Handler();
        FATFS_UnLinkDriver(SDPath);
        return;
    }

    /* Create and Open a new text file object with write access */
    if (f_open(&SDFile, "data.csv", FA_OPEN_ALWAYS | FA_WRITE) != FR_OK) {
        Error_Handler();
        FATFS_UnLinkDriver(SDPath);
        return;
    }

    /* Write CSV header */
    snprintf(buffer, sizeof(buffer), "Year/Month/Day;Hour:Minute:Second;Temp;Hum;Pluie;Speed\n");
    res = f_write(&SDFile, buffer, strlen(buffer), (void *)&byteswritten);
    if ((byteswritten == 0) || (res != FR_OK)) {
        Error_Handler();
        f_close(&SDFile);
        FATFS_UnLinkDriver(SDPath);
        return;
    }

    /* Write array data along with RTC time to the CSV file */
    for (i = 0; i < sampleCount; i++) {
        RTC_TimeTypeDef sTime;
        RTC_DateTypeDef sDate;

        /* Fetch RTC time */
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        /* Format data into CSV row */
        snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d;%02d:%02d:%02d;%f;%f;%f;%f\n",
                 2000 + sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds,
                 tempData[i], humData[i], rainData[i], windData[i]);

        res = f_write(&SDFile, buffer, strlen(buffer), (void *)&byteswritten);

        if ((byteswritten == 0) || (res != FR_OK)) {
            Error_Handler();
            break;
        }
    }

    /* Close the open text file */
    f_close(&SDFile);

    /* Unlink the micro SD disk I/O driver */
    FATFS_UnLinkDriver(SDPath);
}




void stop_Mode(){
	HAL_GPIO_WritePin(user_led_GPIO_Port, user_led_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(blue_led_GPIO_Port, blue_led_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(red_led_GPIO_Port, red_led_Pin, GPIO_PIN_RESET);
}






/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
