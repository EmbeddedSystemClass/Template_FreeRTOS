/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_ThreadCreation/Src/main.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    14-August-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Uncomment this line to use the board as master, if not it is used as slave */
//#define MASTER_BOARD
#define I2C_ADDRESS        0x3E
#define MASTER_REQ_READ    0x12
#define MASTER_REQ_WRITE   0x34

/* Private variables ---------------------------------------------------------*/
/* FreeRTOS */
xTaskHandle LEDThread1Handle, LEDThread2Handle, I2CThreadHandle;

/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/* I2C handler declaration */
I2C_HandleTypeDef I2CxHandle;

/* Buffer used for UART and I2c transmission */
uint8_t aTxBuffer[] = " **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling **** \n";

/* Buffer used for I2C reception */
uint8_t aRxBuffer[RXBUFFERSIZE];
uint16_t hTxNumData = 0, hRxNumData = 0;
uint8_t bTransferRequest = 0;

/* Private function prototypes -----------------------------------------------*/
void LED_Thread1(void *argument);
void LED_Thread2(void *argument);
void I2C_Slave_Mode(void *argument);

static void SystemClock_Config(void);
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength);
static void Error_Handler(void);

//static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  
  /* Configure LED3 and LED4 */
//  BSP_LED_Init(LED3);
//  BSP_LED_Init(LED4);
  
  /* Configure the system clock to 180 MHz */
  SystemClock_Config();
  
	/*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART1 configured as follow:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance          = USARTx;
  
  UartHandle.Init.BaudRate     = 9600;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }
	
	/*##-1- Configure the I2C peripheral #######################################*/
  I2CxHandle.Instance             = I2Cx;
  I2CxHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2CxHandle.Init.ClockSpeed      = 400000;
  I2CxHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2CxHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
  I2CxHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2CxHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  I2CxHandle.Init.OwnAddress1     = I2C_ADDRESS;
  I2CxHandle.Init.OwnAddress2     = 0;
	
	if(HAL_I2C_Init(&I2CxHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
	
	//HAL_UART_Transmit(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 5000);
	
  /* Start thread 1 */
  //xTaskCreate(LED_Thread1,"Thread 1", configMINIMAL_STACK_SIZE,
	//NULL, 0, &LEDThread1Handle);
	
  //	HAL_UART_Transmit(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 5000);
	
  /* Start thread 2 */
  //xTaskCreate(LED_Thread2,"Thread 2", configMINIMAL_STACK_SIZE,
	//NULL, 0, &LEDThread2Handle);
	
	//HAL_UART_Transmit(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 5000);
	
	/* Start thread 3 */
  xTaskCreate(I2C_Slave_Mode,"I2C Slave", configMINIMAL_STACK_SIZE,
	NULL, 0, &I2CThreadHandle);
	
		//HAL_UART_Transmit(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 5000);
	
  /* Start scheduler */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */
  for(;;);
}

/**
  * @brief  Toggle LED3 and LED4 thread
  * @param  thread not used
  * @retval None
  */
void LED_Thread1(void *argument)
{
  uint32_t count = 0UL;
  (void) argument;
  
  for(;;)
  {
    count = xTaskGetTickCount() + 5000;
    
    /* Toggle LED3 every 200 ms for 5 s */
    while (count >= xTaskGetTickCount())
    {
    //  BSP_LED_Toggle(LED3);
      
      vTaskDelay(200);
    }
    
    /* Turn off LED3 */
  //  BSP_LED_Off(LED3);
    
    /* Suspend Thread 1 */
    vTaskSuspend(NULL);
    
    count = xTaskGetTickCount() + 5000;
    
    /* Toggle LED3 every 400 ms for 5 s */
    while (count >= xTaskGetTickCount())
    {
    //  BSP_LED_Toggle(LED3);
      
      vTaskDelay(400);
    }
    
    /* Resume Thread 2 */
    vTaskResume(LEDThread2Handle);
  }
}

/**
  * @brief  Toggle LED4 thread
  * @param  argument not used
  * @retval None
  */
void LED_Thread2(void *argument)
{
  uint32_t count = 0UL;;
  (void) argument;
  
  for(;;)
  {
    count = xTaskGetTickCount() + 10000;
    
    /* Toggle LED4 every 500 ms for 10 s */
    while (count >= xTaskGetTickCount())
    {
 //     BSP_LED_Toggle(LED4);

      vTaskDelay(500);
    }
    
    /* Turn off LED4 */
//    BSP_LED_Off(LED4);
	/*
  if(HAL_UART_Transmit(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE, 5000)!= HAL_OK)
  {
    Error_Handler();   
  }
   */
    /* Resume Thread 1 */
    vTaskResume(LEDThread1Handle);
    
    /* Suspend Thread 2 */
    vTaskSuspend(NULL);  
  }
}






void I2C_Slave_Mode(void *argument)
{
    
  for(;;)
  {
		/* Initialize number of data variables */
    hTxNumData = 0;
    hRxNumData = 0;
			
    /*##-2- Slave receive request from master ################################*/
    while(HAL_I2C_Slave_Receive_IT(&I2CxHandle, (uint8_t*)&bTransferRequest, 1)!= HAL_OK)
    {
    }

		
    /*  Before starting a new communication transfer, you need to check the current
    state of the peripheral; if it? busy you need to wait for the end of current
    transfer before starting a new one.
    For simplicity reasons, this example is just waiting till the end of the
    transfer, but application may perform other tasks while transfer operation
    is ongoing. */
    while (HAL_I2C_GetState(&I2CxHandle) != HAL_I2C_STATE_READY)
    {
    }
		
    /* If master request write operation #####################################*/
    if (bTransferRequest == MASTER_REQ_WRITE)
    {
      /*##-3- Slave receive number of data to be read ########################*/
      while(HAL_I2C_Slave_Receive_IT(&I2CxHandle, (uint8_t*)&hRxNumData, 2)!= HAL_OK);

      /*  Before starting a new communication transfer, you need to check the current
      state of the peripheral; if it? busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
      while (HAL_I2C_GetState(&I2CxHandle) != HAL_I2C_STATE_READY)
      {
      }

      /*##-4- Slave receives aRxBuffer from master ###########################*/
      while(HAL_I2C_Slave_Receive_IT(&I2CxHandle, (uint8_t*)aRxBuffer, hRxNumData)!= HAL_OK);

      /*  Before starting a new communication transfer, you need to check the current
      state of the peripheral; if it? busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
      while (HAL_I2C_GetState(&I2CxHandle) != HAL_I2C_STATE_READY)
      {
      }

//      /* Check correctness of received buffer ################################*/
//      if(Buffercmp((uint8_t*)aTxBuffer,(uint8_t*)aRxBuffer,hRxNumData))
//      {
//        /* Processing Error */
//        Error_Handler();
//      }

      /* Flush Rx buffers */
      Flush_Buffer((uint8_t*)aRxBuffer,RXBUFFERSIZE);

      /* Toggle LED3 */
//      BSP_LED_Toggle(LED3);
    }
    /* If master request write operation #####################################*/
    else
    {
      /*##-3- Slave receive number of data to be written #####################*/
      while(HAL_I2C_Slave_Receive_IT(&I2CxHandle, (uint8_t*)&hTxNumData, 2)!= HAL_OK);

      /*  Before starting a new communication transfer, you need to check the current
      state of the peripheral; if it? busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
      while (HAL_I2C_GetState(&I2CxHandle) != HAL_I2C_STATE_READY)
      {
      }

      /*##-4- Slave transmit aTxBuffer to master #############################*/
      while(HAL_I2C_Slave_Transmit_IT(&I2CxHandle, (uint8_t*)aTxBuffer, RXBUFFERSIZE)!= HAL_OK);

      /*  Before starting a new communication transfer, you need to check the current
      state of the peripheral; if it? busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
      while (HAL_I2C_GetState(&I2CxHandle) != HAL_I2C_STATE_READY)
      {
      }
    }
	}
}



/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
 
  /* Activate the Over-Drive mode */
  HAL_PWREx_EnableOverDrive();
 
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Turn LED4 on: Transfer error in reception/transmission process */
//  BSP_LED_On(LED4); 
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
  /* Turn Off LED3 */
//  BSP_LED_Off(LED3);
  /* Turn On LED4 */
//  BSP_LED_On(LED4);
  while(1)
  {
  }
}


static void Error_Handler(void)
{
  /* Turn LED4 on */
 // BSP_LED_On(LED4);
  while(1)
  {
  }
}

/**
  * @brief  Flushes the buffer
  * @param  pBuffer: buffers to be flushed.
  * @param  BufferLength: buffer's length
  * @retval None
  */
static void Flush_Buffer(uint8_t* pBuffer, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    *pBuffer = 0;

    pBuffer++;
  }
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
