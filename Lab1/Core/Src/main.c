/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "hal_driver.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void showCurrentCountOnDiodes(int count) {
    resetAllDioids();
    switch (count) {
        case 1: {
            setLightRedDiode();
            break;
        }
        case 2: {
            setLightGreenDiode();
            break;
        }
        case 3: {
            setLightRedDiode();
            setLightGreenDiode();
            break;
        }
        default: {
            setOffGreenDiode();
            setOffRedDiode();
            break;
        }
    }
}

void showOverflow(int overFlow) {
    resetAllDioids();
    playAnimation();
    for (int i = 0; i < overFlow; i++) {
        blinkGreenDiode();
    }
}

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

	  int shortPressTime = 100;
	    int longPressTime = 2000;
	    int msTick = 0;
	    int state = 0;
	    int overflow = 0;
	    int currentCount = 0;
	    int isBeingProceed = 0;
	    long startTimePressedButton = 0;

	    /* Configure the system clock */
	    SystemClock_Config();

	    /* USER CODE BEGIN SysInit */

	    /* USER CODE END SysInit */

	    /* Initialize all configured peripherals */
	    MX_GPIO_Init();
	    /* USER CODE BEGIN 2 */

	    /* USER CODE END 2 */

	    /* Infinite loop */
	    /* USER CODE BEGIN WHILE */
	    resetAllDioids();
	    while (1)
	    {
	  	  state = getButtonState();
	  	          if (!state) {
	  	              if (!isBeingProceed){
	  	            	  startTimePressedButton = getCurrentTime();
	  	            	  isBeingProceed = 1;
	  	              }
	  	          } else {
	  	              if (isBeingProceed && getCurrentTime() - startTimePressedButton > longPressTime) {
//	  	            	  playAnimation();
	  	                  currentCount--;
	  	                  if (currentCount < 0) {
	  	                      overflow--;
	  	                      if (overflow < 0){
	  	                          overflow = 0;
	  	                          currentCount = 0;
	  	                      }else{
	  	                      showOverflow(overflow);
	  	                      currentCount = 3;
	  	                      }
	  	                  }
	  	              } else {
	  	                  if (isBeingProceed && getCurrentTime() - startTimePressedButton > shortPressTime) {
	  	                      currentCount++;
	  	                      if (currentCount >= 4) {
	  	                          overflow++;
	  	                          showOverflow(overflow);
	  	                          currentCount = 0;
	  	                      }
	  	                  }
	  	              }
	  	            isBeingProceed = 0;
	  	  			showCurrentCountOnDiodes(currentCount);
	  	          }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
