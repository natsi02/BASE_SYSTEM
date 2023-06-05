/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ModBusRTU.h"
#include "I2C_EndEffector.h"
#include "string.h"
#include "math.h"
#include "Joystick.h"
#define BUFFER_SIZE 4
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
ModbusHandleTypedef hmodbus;
u16u8_t registerFrame[200];
int reset;
int start;
int pick;
int place;
int stop;
int test;
int stoptest;
uint8_t RecieveData;
char Data[8];
int lastTest;
int lastRun;
int lastPick;
uint8_t Result;
//int flag;
//Joystick//
int receivedByte[BUFFER_SIZE];
int SetTrayPos;

typedef struct PositionStructure
{
	int tray_x[3];
	int tray_y[3];

	int position_x;
	int position_y;
}PositionStructureTypedef;
PositionStructureTypedef PosData = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void UARTInterruptConfig();
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
  MX_USART2_UART_Init();
  MX_TIM11_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  hmodbus.huart = &huart2;
  hmodbus.htim = &htim11;
  hmodbus.slaveAddress = 0x15;
  hmodbus.RegisterSize =200;
  Modbus_init(&hmodbus, registerFrame);
  UARTInterruptConfig();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Modbus_Protocal_Worker();
	  static uint32_t timestamp = 0;
	  if (HAL_GetTick() >= timestamp){
	  	timestamp = HAL_GetTick() + 200;
	  	registerFrame[00].U16 = 22881;
//		HAL_I2C_Master_Receive(&hi2c1, 0x15<<1, &RecieveData, 1, HAL_MAX_DELAY);
//	  decimalToBinary(registerFrame[2].U16,Data);
//	  	Transmit_To_Basesystem(&registerFrame[2].U16,&hi2c1);
	  }
	  I2C_TO_BASESYSTEM(&registerFrame[2].U16,&hi2c1);
//	  if(Data[0] == '1' && lastTest == 0)
//	  {
//		  ENDEFF_TEST_MODE(&hi2c1);
//		  lastTest = 1;
//	  }
//	  else if(Data[0] == '0' && lastTest == 1)
//	  {
//		  ENDEFF_TEST_MODE_QUIT(&hi2c1);
//		  lastTest = 0;
//	  }
//	  else if(Data[1] == '1' && lastRun == 0)
//	  {
//		  ENDEFF_GRIPPER_RUNMODE(&hi2c1);
//		  lastRun = 1;
//	  }
//	  else if(Data[1] == '0' && lastRun == 1)
//	  {
//		  ENDEFF_GRIPPER_IDLE(&hi2c1);
//		  lastRun = 0;
//	  }
//	  else if(Data[2] == '1' && Data[1] == '1' && lastPick == 0)
//	  {
//		  ENDEFF_GRIPPER_PICK(&hi2c1);
//		  registerFrame[2].U16 = 0b0010;
//		  lastPick = 1;
//	  }
//	  else if(Data[3] == '1' && Data[1] == '1' && lastPick == 1)
//	  {
//		  ENDEFF_GRIPPER_PLACE(&hi2c1);
//		  registerFrame[2].U16 = 0b0010;
//		  lastPick = 0;
//	  }

//	  if(flag == 1)
//	  {
//		  char NewData[8];
//		  Result = 0;
//		  for(int i = 0; i < 4;i++)
//		  {
//			  if(i == 0) NewData[2] = Data[i];
//			  else if(i == 1) NewData[3] = Data[i];
//			  else if(i == 2) NewData[1] = Data[i];
//			  else if(i == 3) NewData[0] = Data[i];
//		  }
//
//		  for(int j = 0; j<sizeof(NewData);j++)
//		  {
//			  if(NewData[j] == '\0') j = sizeof(NewData);
//			  else Result += ((int)NewData[j]-'0')*pow(2,j);
//		  }
//		  registerFrame[2].U16 = Result;
//		  flag = 0;
//	  }

 	  if(reset == 1)
	  {
		  ENDEFF_SOFT_RESET(&hi2c1);
		  reset = 0;
		  start = 0;
		  test = 0;
		  pick = 0;
		  place = 0;
		  stoptest = 0;
//		  HAL_I2C_Master_Receive(&hi2c1, 0x15<<1, &RecieveData, 1, HAL_MAX_DELAY);
//		  decimalToBinary(RecieveData,Data);
//		  flag = 1;
	  }
	  if(test == 1)
	  {
		  ENDEFF_TEST_MODE(&hi2c1);
		  HAL_I2C_Master_Receive(&hi2c1, 0x15<<1, &RecieveData, 1, HAL_MAX_DELAY);
//		  decimalToBinary(RecieveData,Data);
		  test = 0;
//		  flag = 1;
	  }
	  else if(stoptest == 1)
	  {
		  ENDEFF_TEST_MODE_QUIT(&hi2c1);
		  HAL_I2C_Master_Receive(&hi2c1, 0x15<<1, &RecieveData, 1, HAL_MAX_DELAY);
//		  decimalToBinary(RecieveData,Data);
		  stoptest = 0;
//		  flag = 1;
	  }
	  else if(start == 1)
	  {
		  ENDEFF_GRIPPER_RUNMODE(&hi2c1);
		  HAL_I2C_Master_Receive(&hi2c1, 0x15<<1, &RecieveData, 1, HAL_MAX_DELAY);
//		  decimalToBinary(RecieveData,Data);
		  start = 0;
//		  flag = 1;
	  }
	  else if(pick == 1)
	  {
		  ENDEFF_GRIPPER_PICK(&hi2c1);
		  HAL_I2C_Master_Receive(&hi2c1, 0x15<<1, &RecieveData, 1, HAL_MAX_DELAY);
//		  decimalToBinary(RecieveData,Data);
		  pick = 0;
//		  flag = 1;
	  }
	  else if(place == 1)
	  {
		  ENDEFF_GRIPPER_PLACE(&hi2c1);
//		  HAL_I2C_Master_Receive(&hi2c1, 0x15<<1, &RecieveData, 1, HAL_MAX_DELAY);
//		  decimalToBinary(RecieveData,Data);
		  place = 0;
//		  flag = 1;
	  }
	  else if(stop == 1)
	  {
		  ENDEFF_GRIPPER_IDLE(&hi2c1);
		  HAL_I2C_Master_Receive(&hi2c1, 0x15<<1, &RecieveData, 1, HAL_MAX_DELAY);
//		  decimalToBinary(RecieveData,Data);
		  stop = 0;
//		  flag = 1;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1){
		Joystick_Received(&receivedByte);
	}
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
