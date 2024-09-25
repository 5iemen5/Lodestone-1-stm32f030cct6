/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
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
//#include "stm32f0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "UIdriver.h"
//#include "UIdriver.c"
//#include "display.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
unsigned int adcConverted=64;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc;
extern TIM_HandleTypeDef htim7;
/* USER CODE BEGIN EV */
extern int nextOrPrev;
extern int menuTriggered;
extern int allowNextPage;
extern struct uiPage * currentPage;
extern struct uiPage page1, page2, page3;
extern RTC_HandleTypeDef hrtc;

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
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
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
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
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

	HAL_NVIC_DisableIRQ(EXTI4_15_IRQn); //FOR BUTTON DEBOUNCE

	HAL_TIM_Base_Start_IT(&htim7);		//STARTING 5 SECS CONTDOWN

	htim7.Instance->CNT=0;				//NULLIFY THE COUNTER

	HAL_ResumeTick();					//WAKE THE CPU UP
	for(int i=0; i<100000; i++){};

	HAL_ADC_Start_IT(&hadc);			//STARTING ADC CONVERSIONS WITH IT

	nextOrPrev=CURRENT;					//STARTING PAGE IS CURRENT
	nextPage();

	if((currentPage->numberOfPage)==0)	//DISPLAY TIME AT THE BOTTOM OF THE SCREEN
	 {checkTime();
	 ssd1306_SetCursor(0, 55);
	 displayTime();}

	ssd1306_SetDisplayOn(1);			//DISPLAY SET ON
	pressButton();
	selectPage();						//SELECT THE CURRENT PAGE


	for(int i=0; i<350000; i++){};	//BUTTON DEBOUNCE DELAY

	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);	//DEBOUNCE DISABLED
  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
  * @brief This function handles ADC interrupt.
  */
void ADC1_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_IRQn 0 */

  /* USER CODE END ADC1_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc);
  /* USER CODE BEGIN ADC1_IRQn 1 */
  	  adcConverted=63;
	  HAL_ADC_PollForConversion(&hadc, 50);
	  adcConverted=HAL_ADC_GetValue(&hadc);
	  HAL_ADC_Stop_IT(&hadc);

if(adcConverted>23){

}

	  else if (adcConverted<3){			//BUTTON 3	(BACK)
		htim7.Instance->CNT=0;
		htim7.Instance->ARR=25000;
		unselectPage();
	  }
	  else if (adcConverted<12){		//BUTTON 2	(RIGHT/DOWN)
		nextOrPrev=NEXT;
		nextPage();
		moveCursor();

		htim7.Instance->CNT=0;
		htim7.Instance->ARR=25000;
	  }
	  else {							//BUTTON 1	(LEFT/UP)[if (adcConverted<24||adcConverted>11)]
		nextOrPrev=PREV;
		moveCursor();
		nextPage();

		htim7.Instance->CNT=0;
		htim7.Instance->ARR=25000;
	  }
for(int i=0; i<350000; i++){};


	  HAL_ADC_Start_IT(&hadc);

	 //checkTime();
	 //ssd1306_SetCursor(2, 52);
	 //displayTime();
	 //ssd1306_UpdateScreen();
  /* USER CODE END ADC1_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */

  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */
		HAL_TIM_Base_Stop_IT(&htim7);
		HAL_ADC_Stop_IT(&hadc);
		ssd1306_SetDisplayOn(0);

		htim7.Instance->ARR=5000;
		htim7.Instance->CNT=0;
		currentPage= &page1;
		//nextOrPrev=CURRENT;
	//	menuTriggered = 0;
		//allowNextPage=0;
		currentPage->buttonMenu[0].selected=0;
		currentPage->buttonMenu[1].selected=0;
		currentPage->buttonMenu[2].selected=0;
		currentPage->currentButton=0;
		//page1.selected=1;
		//page2.selected=0;
		//page3.selected=0;
		ssd1306_FillRectangle(0, 10, 5, 54, 0x00);

		HAL_SuspendTick();
   // HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  /* USER CODE END TIM7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
