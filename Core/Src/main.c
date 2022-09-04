/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define MAX_CMD_NUM 10
#define BUF_LEN     128
#define ON			1
#define OFF			0
typedef int cmd_func(int argc, char* argv[]);
struct Command_List
{
	char* cmd;
	cmd_func* func;
	char* help_str;
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define BUFSIZE 30
uint8_t rx3_data;
uint8_t rx3buf[BUFSIZE];

uint8_t bufindex = 0;
uint8_t rx_start = 0;
uint8_t rx_complete = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int cmd_torque(int argc, char* argv[])
{
	if (argv[1] == NULL) {
		printf("wrong command pattern. \n");
	}
	else {
		if (!strcmp(argv[1], "on")) {
			printf("torque on. \n");
		}
		else if (!strcmp(argv[1], "off")) {
			printf("torque off. \n");
		}
	}

	return 0;
}

int cdm_pump(int argc, char* argv[])
{
	if (argv[1] == NULL) {
		printf("wrong command pattern. \n");
	}
	else {
		if (!strcmp(argv[1], "on")) {
			printf("pump on. \n");
		}
		else if (!strcmp(argv[1], "off")) {
			printf("pump off. \n");
		}
	}

	return 0;
}

int cmd_conveyorBelt(int argc, char* argv[])
{
	if (argv[1] == NULL) {
		printf("wrong command pattern. \n");
	}
	else {
		if (!strcmp(argv[1], "on")) {
			if (argv[2] == NULL) {
				printf("wrong command pattern. \n");
			}
			else {
				if (!strcmp(argv[2], "right")) {
					printf("conveyor belt turn endlessly right. \n");
				}
				else if (!strcmp(argv[2], "left")) {
					printf("conveyor belt turn endlessly left. \n");
				}else {
					printf("wrong command pattern. \n");
				}
			}
		}
		else if (!strcmp(argv[1], "off")) {
			printf("conveyor belt stop. \n");
		}
	}

	return 0;
}

int cmd_pick(int argc, char* argv[])
{
	printf("pick up item on the conveyor belt. \n");
	return 0;
}

int cmd_throw(int argc, char* argv[])
{
	printf("throw item away \n");
	return 0;
}

int cmd_exit(int argc, char* argv[])
{
	printf("exit command received \n");
	return 0;
}

void parse_input_string(char* input_string, int* argc, char* argv[])
{
	int found_arg = 1;
	int argn = 0;

	while (*input_string)
	{
		if (*input_string == '\n') {
			*input_string = '\0';
			break;
		}

		if (*input_string == ' ') {
			found_arg = 1;
			*input_string = '\0';
		}
		else if (found_arg) {
			argv[argn++] = input_string;
			found_arg = 0;
		}
		input_string++;
	}

	*argc = argn;
}

struct Command_List  CmdList[] =
{
	{"torque",	cmd_torque,	"on/off Torque"},
	{"pump",	cdm_pump,	"on/off Pump"},
	{"cvbelt",	cmd_conveyorBelt,	"on/off Conveyor Belt. right/left"},
	{"pick",	cmd_pick,	"no option. End effector Down + Pump On + default position"},
	{"throw",	cmd_throw,	"no option. Throw + default position"},
	{"exit",	cmd_exit,	"exit command"},
	{0,0,0}
};

void cmd_handler(char* cmd)
{
	struct Command_List* pCmdList = CmdList;

	uint32_t command_found = 0;

	int	  argc;
	char* argv[MAX_CMD_NUM] = { 0, };

	parse_input_string(cmd, &argc, argv);

	if (argc)
	{
		while (pCmdList->cmd)
		{
			if (!strcmp((const char*)pCmdList->cmd, (const char*)cmd))
			{
				command_found = 1;
				pCmdList->func(argc, argv);
				break;
			}
			++pCmdList;
		}
	}
	if (command_found == 0) printf("command not found!\n");
}


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
  MX_USART3_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, &rx3_data, 1);
  printf("=====================\r\n");
  printf("Command Line Interface\r\n");
  printf("=====================\r\n");
  char temp[4]={0,};
  sprintf(temp, ">> ");
  HAL_UART_Transmit_IT(&huart3, (uint8_t*)temp, sizeof(temp));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_Delay(1000);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance == USART3){
		if(rx3_data != '\n' && bufindex < BUFSIZE)
			rx3buf[bufindex++] = rx3_data;
		else {
			cmd_handler((char*)rx3buf);
			char temp[4]={0,};
			sprintf(temp, ">> ");
			HAL_UART_Transmit_IT(&huart3, (uint8_t*)temp, sizeof(temp));
			memset(rx3buf,0,sizeof(rx3buf));
			bufindex=0;

		}


		HAL_UART_Receive_IT(&huart3, &rx3_data, 1);
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
