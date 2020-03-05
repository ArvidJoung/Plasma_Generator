/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "target.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
#include "cmsis_os.h"
#include "usart.h"
#include "adc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc;

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  osSystickHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line 0 to 1 interrupts.
*/
void EXTI0_1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0, &xHigherPriorityTaskWoken);

    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}


/**
* @brief This function handles EXTI line 4 to 15 interrupts.
*/
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4, &xHigherPriorityTaskWoken);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5, &xHigherPriorityTaskWoken);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6, &xHigherPriorityTaskWoken);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7, &xHigherPriorityTaskWoken);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9, &xHigherPriorityTaskWoken);

    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}

void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  /* USER CODE END USART1_IRQn 0 */
  
  //HAL_UART_IRQHandler(&huart1);
  HAL_UART_IRQHandler(&huart1, &xHigherPriorityTaskWoken);
  
  /* USER CODE BEGIN USART1_IRQn 1 */
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  /* USER CODE END USART1_IRQn 1 */
}

#if defined(ADC_USE_IT_METHOD)  
/**
* @brief This function handles ADC interrupt.
*/
void ADC1_IRQHandler(void)
{
    /* USER CODE BEGIN */
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    /* USER CODE END */
    
    HAL_ADC_IRQHandler(&hadc, &xHigherPriorityTaskWoken);

    /* USER CODE BEGIN */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    /* USER CODE END */   
}
#endif

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
