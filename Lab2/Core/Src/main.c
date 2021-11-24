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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "hal_driver.h"
#include <stdio.h>

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

#define KBRD_RD_ADDR 0xE3
#define KBRD_WR_ADDR 0xE2
#define ROW1 0x1E
#define ROW2 0x3D
#define ROW3 0x7B
#define ROW4 0xF7

enum MathOperation {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    EQUAL,
    NONE
};

int mode = 1;
char ch[] = "F";
char message[50] = "";
char mode0[] = "Mode: Preriv \n\r";
char mode1[] = "Mode: Bez Preriv \n\r";
int canReceive = 1;
short isFirst = 1;
short counter = 0;
short firstOperand = 0;
short secondOperand = 0;
enum MathOperation operation = NONE;

typedef enum{
	INPUT_PORT = 0x00, //Read byte XXXX XXXX
	OUTPUT_PORT = 0x01, //Read/write byte 1111 1111
	POLARITY_INVERSION = 0x02, //Read/write byte 0000 0000
	CONFIG = 0x03 //Read/write byte 1111 1111
}pca9538_regs_t;

HAL_StatusTypeDef PCA9538_Read_Register(uint16_t addr, pca9538_regs_t reg, uint8_t* buf) {
	return HAL_I2C_Mem_Read(&hi2c1, addr | 1, reg, 1, buf, 1, 100);
}


HAL_StatusTypeDef PCA9538_Write_Register(uint16_t addr, pca9538_regs_t reg, uint8_t* buf) {
	return HAL_I2C_Mem_Write(&hi2c1, addr & 0xFFFE, reg, 1, buf, 1, 100);
}

HAL_StatusTypeDef PCA9538_Read_Inputs(uint16_t addr, uint8_t* buf) {
	return PCA9538_Read_Register(addr, INPUT_PORT, buf);
}

short getNumber(char ch) {
    short asciNum = ch;
    if (asciNum >= 48 && asciNum <= 57)
        return asciNum - '0';
    return -1;
}

enum MathOperation getMathOperation(char ch) {
    switch (ch) {
        case '+':
            return ADDITION;
        case '-':
            return SUBTRACTION;
        case '*':
            return MULTIPLICATION;
        case '/':
            return DIVISION;
        case '=':
            return EQUAL;
    }
    return NONE;
}

short checkNumbertoOverflow(short fullNumber, short number) {
    if ((fullNumber == 3276 && number >= 7) || fullNumber > 3276)
        return -1;
    return fullNumber * 10 + number;
}

short makeMathOperation(enum MathOperation operation, short firstOperand, short secondOperand) {
    switch (operation) {
        case ADDITION: {
            long testOverflow = firstOperand + secondOperand;
            short result = firstOperand + secondOperand;
            if (result == testOverflow)
                return result;
            else
                return -1;
        }
        case SUBTRACTION:
            return firstOperand - secondOperand;
        case MULTIPLICATION: {
            long testOverflow = firstOperand * secondOperand;
            short result = firstOperand * secondOperand;
            if (result == testOverflow)
                return result;
            else
                return -1;
        }
        case DIVISION: {
            return firstOperand / secondOperand;
        }
    }
    return -1;
}

void
resetCalc() {
    isFirst = 1;
    counter = 0;
    firstOperand = 0;
    secondOperand = 0;
    operation = NONE;
}

char errorMessage[] = "\n\rERROR\n\r";
char errorMessageDiv[] = "\n\rCant divide by zero\n\r";

void sendMessage(char *message, int size) {
	size = strlen(message);
    if (mode)
        HAL_UART_Transmit(&huart6, (uint8_t *) message, size, 100);
    else {
//    	delay(1000);
        long time = getCurrentTime();
        HAL_StatusTypeDef isReaded1 = HAL_BUSY;
        int curtime = getCurrentTime();
        int counter = 0;
        while (counter < 100000 && isReaded1 == HAL_BUSY) {
        	isReaded1 = HAL_UART_Transmit_IT(&huart6, (uint8_t *) message, size);
        	curtime = getCurrentTime();
        	counter++;
                }
    }
}

void sendError() {
    sendMessage(errorMessage, sizeof(errorMessage) - 1);
}

void sendErrorAndReset() {
    sendError();
    resetCalc();
}

void handle_calc() {
    sendMessage(ch, 1);
    enum MathOperation operationBuf = getMathOperation(ch[0]);
    if (counter > 0) {
        if (isFirst && operationBuf != NONE && operationBuf != EQUAL) {
            operation = operationBuf;
            isFirst = 0;
            counter = 0;
            return;
        }
        if (!isFirst && operationBuf != NONE && operationBuf == EQUAL) {
            long testOverflow;
            short res;
            switch (operation) {
                case ADDITION: {
                    testOverflow = firstOperand + secondOperand;
                    res = firstOperand + secondOperand;
                    break;
                }
                case SUBTRACTION: {
                    testOverflow = firstOperand - secondOperand;
                    res = firstOperand - secondOperand;
                    break;
                }
                case MULTIPLICATION: {
                    testOverflow = firstOperand * secondOperand;
                    res = firstOperand * secondOperand;
                    break;
                }
                case DIVISION: {
                    if (secondOperand == 0) {
                        //sprintf(message, "\n\rCant divide by zero\n\r");
                        sendMessage(errorMessageDiv, sizeof(errorMessageDiv) - 1);
                        resetCalc();
                        return;
                    }
                    testOverflow = firstOperand / secondOperand;
                    res = firstOperand / secondOperand;
                    break;
                }
            }
            if (res == testOverflow){
                sprintf(message, "%d\n\r", res);
                sendMessage(message, sizeof(message) - 1);
            }
            else{
                sendError();
            }
            resetCalc();
            return;
        }
    }
    if (operationBuf != NONE || counter > 4) {
        sendErrorAndReset();
        return;
    }


    short number = getNumber(ch[0]);
    if (number != -1) {
        if (isFirst) {
            short numberBuf = checkNumbertoOverflow(firstOperand, number);
            if (numberBuf == -1) {
                sendErrorAndReset();
                return;
            }
            firstOperand = numberBuf;
        } else {
            short numberBuf = checkNumbertoOverflow(secondOperand, number);
            if (numberBuf == -1) {
                sendErrorAndReset();
                return;
            }
            secondOperand = numberBuf;
        }
        counter++;
    } else {
        sendErrorAndReset();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	canReceive=0;
}

HAL_StatusTypeDef Set_Keyboard( void ) {
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t buf;

	buf=0x70;
	ret = PCA9538_Write_Register(KBRD_WR_ADDR, CONFIG, &buf);
	if( ret != HAL_OK ) {
		//sendMessage("Error write config\n",1);
		goto exit;
	}

	buf = 0;
	ret = PCA9538_Write_Register(KBRD_WR_ADDR, OUTPUT_PORT, &buf);
	if( ret != HAL_OK ) {
		//sendMessage("Error write config\n",1);
	}

exit:
	return ret;
}

uint8_t Check_Row( uint8_t  Nrow ) {
	uint8_t Nkey = 0x00;
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t buf[4]={0,0,0,0};
	uint8_t kbd_in;

	ret = Set_Keyboard();
//	if( ret != HAL_OK ) {
//		sendMessage("Error write config\n",1);
//	}

	buf[0] = Nrow;

	ret = PCA9538_Write_Register(KBRD_WR_ADDR, OUTPUT_PORT, buf);
//	if( ret != HAL_OK ) {
//		sendMessage("Error write output\n",1);
//	}

	buf[0] = 0;
	ret = PCA9538_Read_Inputs(KBRD_RD_ADDR, buf);
//	if( ret != HAL_OK ) {
//		sendMessage("Read Error\n",1);
//	}
	kbd_in = buf[0] & 0x70;
	Nkey = kbd_in;
	if( kbd_in != 0x70) {
		if( !(kbd_in & 0x10) ) {
			switch (Nrow) {
				case ROW1:
					Nkey = 0x04;
					break;
				case ROW2:
					Nkey = 0x04;
					break;
				case ROW3:
					Nkey = 0x04;
					break;
				case ROW4:
					Nkey = 0x04;
					break;
			}
		}
		if( !(kbd_in & 0x20) ) {
			switch (Nrow) {
				case ROW1:
					Nkey = 0x02;
					break;
				case ROW2:
					Nkey = 0x02;
					break;
				case ROW3:
					Nkey = 0x02;
					break;
				case ROW4:
					Nkey = 0x02;
					break;
			}
		}
		if( !(kbd_in & 0x40) ) {
			switch (Nrow) {
				case ROW1:
					Nkey = 0x01;
					break;
				case ROW2:
					Nkey = 0x01;
					break;
				case ROW3:
					Nkey = 0x01;
					break;
				case ROW4:
					Nkey = 0x01;
					break;
			}
		}
	}
	else Nkey = 0x00;

	return Nkey;
}

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
//	canTransmit = 0;
//}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    HAL_StatusTypeDef isReaded;
    int shortPressTime = 200;
    int isBeingProceed = 0;
    long startTimePressedButton = 0;
    char chtest[] = "F\n\r";
    uint8_t R = 0, C = 0, L = 0, Row[4] = {0xF7, 0x7B, 0x3D, 0x1E}, Key, OldKey, OLED_Keys[12] = {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};


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
  MX_USART6_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1) {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    	//sendMessage(chtest, sizeof(chtest)-1);
    	for ( int i = 0; i < 4; i++ ) {
    			while( !( R && C && L ) ) {
    				OldKey = Key;
    				Key = Check_Row( Row[i] );
    				if ( Key == 0x01 && Key != OldKey) {
    					sendMessage("Right pressed\n",1);
    					R = 1;
    				} else if ( Key == 0x02 && Key != OldKey) {
    					sendMessage("Center pressed\n",1);
    					C = 1;
    				} else if ( Key == 0x04 && Key != OldKey) {
    					sendMessage("Left pressed\n",1);
    					L = 1;
    				}
    			}
    			R = C = L = 0;
    			HAL_Delay(25);
    		}
    	sendMessage("KB test complete\n",1);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
    while (1) {
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
