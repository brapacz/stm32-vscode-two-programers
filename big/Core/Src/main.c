/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "printf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STR1(x) #x
#define STR(x) STR1(x)
#define huartM huart1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t UART_rxBuffer[2] = {0};
char CommandBuffer[64] = {0x00};
size_t CommandBufferLength = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  printf(STR(BUILD_ID) "\r\n");
  // HAL_UART_Transmit(&huartM, "Ready\r\n", 7, 100);
  HAL_UART_Receive_DMA(&huartM, UART_rxBuffer, sizeof(UART_rxBuffer));
  printf("Ready\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  size_t LastCommandBufferLength = -1;
  uint8_t SPI_rxBuffer[9] = {0x00};

  while (1)
  {
    uint8_t any = 0;
    if (LastCommandBufferLength != CommandBufferLength)
    {
      printf("buffer state: %d %s\r\n", CommandBufferLength, CommandBuffer);
      LastCommandBufferLength = CommandBufferLength;
      any = 1;
    }

    if (LastCommandBufferLength > 0)
    {
      if (HAL_OK == HAL_SPI_Transmit(&hspi1, &CommandBuffer, CommandBufferLength, 100))
      {
        printf("sent %d bytes via SPI: %s\r\n", CommandBufferLength, CommandBuffer);
        memset(&CommandBuffer, 0x00, 8);
        // memset(&CommandBuffer, 0x00, CommandBufferLength);
        CommandBufferLength = 0;
        any = 1;
      }
    }

    if (HAL_OK == HAL_SPI_Receive(&hspi1, &SPI_rxBuffer, sizeof(SPI_rxBuffer) - 1, 100) && strlen(SPI_rxBuffer) > 0)
    {
      printf("got %d bytes from SPI: %s\r\n", strlen(SPI_rxBuffer), SPI_rxBuffer);
      memset(&SPI_rxBuffer, 0x00, sizeof(SPI_rxBuffer));
      any = 1;
      HAL_Delay(2000);
    }

    if (!any)
      HAL_Delay(100);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  // printf("got %d bytes: %s\r\n", strlen(UART_rxBuffer), UART_rxBuffer);
  // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  if (CommandBufferLength > sizeof(CommandBuffer) - 2)
  {
    CommandBufferLength -= sizeof(CommandBuffer);
    memset(&CommandBuffer + CommandBufferLength, 0x00, sizeof(CommandBuffer) - CommandBufferLength);
  }
  uint8_t l = strlen(&UART_rxBuffer);
  for (size_t i = 0; i < l; i++)
    if (UART_rxBuffer[i] != '\n' && UART_rxBuffer[i] != '\r')
      CommandBuffer[CommandBufferLength++] = UART_rxBuffer[i];
  memset(&UART_rxBuffer, 0x00, sizeof(UART_rxBuffer));
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
  printf("Error_Handler\r\n");
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
