/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

#define GREEN GPIO_PIN_12
#define ORANGE GPIO_PIN_13
#define RED GPIO_PIN_14
#define BLUE GPIO_PIN_15

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

int __io_putchar(int ch);

void vBlueLEDControllerTask(void *pvParameters);
void vRedLEDControllerTask(void *pvParameters);
void vGreenLEDControllerTask(void *pvParameters);

typedef uint32_t TaskProfiler;
TaskProfiler BlueTaskProfiler, GreenTaskProfiler, RedTaskProfiler;

TaskHandle_t blue_handle, red_handle, green_handle;

int delay = 50000;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  xTaskCreate(vBlueLEDControllerTask,
		  	  "blue led controller",
			  100,
			  NULL,
			  1,
			  &blue_handle);

  xTaskCreate(vRedLEDControllerTask,
			  "red led controller",
			  100,
			  NULL,
			  2,
			  &red_handle);

  xTaskCreate(vGreenLEDControllerTask,
			  "green led controller",
			  100,
			  NULL,
			  3,
			  &green_handle);

  vTaskStartScheduler();
}

void vBlueLEDControllerTask(void *pvParameters)
{
	int i;

	for (;;)
	{
		BlueTaskProfiler++;
		// HAL_GPIO_TogglePin(GPIOD, BLUE);

		// Non-delay delay...
		for (i=0; i<delay; i++) {}
	}
}

void vRedLEDControllerTask(void *pvParameters)
{
	int i, priority;

	// This is how you get a task's priority
	priority = uxTaskPriorityGet(NULL);
	for (;;)
	{
		RedTaskProfiler++;
		// HAL_GPIO_TogglePin(GPIOD, ORANGE);

		// Non-delay delay...
		for (i=0; i<delay; i++) {}

		if (priority > 1) {
			priority--;

			// Change own priority
			vTaskPrioritySet(NULL, priority);
		}
	}
}

void vGreenLEDControllerTask(void *pvParameters)
{
	int i, priority;
	priority = uxTaskPriorityGet(NULL);
	for (;;)
	{
		GreenTaskProfiler++;
		// HAL_GPIO_TogglePin(GPIOD, ORANGE);

		// Non-delay delay...
		for (i=0; i<delay; i++) {}

		if (priority > 1) {
			priority--;

			// Change own priority
			vTaskPrioritySet(NULL, priority);
		}
	}
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    // Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    // Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    // Error_Handler();
  }
}

static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    // Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Enable clock access to GPIO port */
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Reset pins */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);

  /* Configure pins */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  /* Initialize pins*/
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
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
