#include "stm32l1xx.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_utils.h"

/*Timer peripherral driver included*/
#include "stm32l1xx_ll_tim.h"

//LL driver for LCD included
#include "stm32l1xx_ll_lcd.h"
#include "stm32l152_glass_lcd.h"
#include "stdio.h" //for sprintf function access


void SystemClock_Config(void);
void GPIO_Configuretion (void);
void TIMBase_Config(void);
uint32_t cnt = 0;
uint8_t second = 0;
uint8_t minute = 0;
uint8_t hour = 0;
char display[7];

void Show_segment();
void Reset_show_segment();
uint32_t arrtime[4] = {0, 0, 0, 0};

uint8_t i;

uint32_t seg[11] = {LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14,	//0
										LL_GPIO_PIN_10 | LL_GPIO_PIN_11,	//1
										LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_15,	//2
									  LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_15,	//3
										LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,	//4
										LL_GPIO_PIN_2 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,	//5
										LL_GPIO_PIN_2 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,	//6
										LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11,	//7
										LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,	//8
										LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,	//9 
										LL_GPIO_PIN_4}; //L1,L2
	
uint32_t digit[4] = {LL_GPIO_PIN_0,LL_GPIO_PIN_1, LL_GPIO_PIN_2, LL_GPIO_PIN_3};

int main()
{	
	SystemClock_Config();
	GPIO_Configuretion();
	TIMBase_Config();

	while(1)
	{
 		cnt = LL_TIM_GetCounter(TIM2);
		if (cnt >= LL_TIM_GetAutoReload(TIM2))
		{
			LL_TIM_SetCounter(TIM2, 0);
			second++;
			if (second == 60)
			{
				second = second%60;
				minute++;
				if (minute == 60)
				{
					minute = minute%60;
					hour++;
				}
			}
			arrtime[3] = second%10;
			arrtime[2] = second/10;
			arrtime[1] = minute%10;
			arrtime[0] = minute/10;
		}
		Show_segment(arrtime);
	}
}

void Reset_show_segment()
{
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15);
}

void Show_segment(time){
	for(i=0	; i<4; i++)
	{
		Reset_show_segment();
		LL_GPIO_SetOutputPin(GPIOB, seg[arrtime[i]]);
		LL_GPIO_SetOutputPin(GPIOC, digit[i]);
	}
}

void TIMBase_Config(void)
{
	/*create object name*/
	LL_TIM_InitTypeDef timbase;
	
	/*enable clock on bus path APB1*/
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	
	timbase.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase.Autoreload = 8000 - 1;
	timbase.Prescaler = 4000 - 1;
	
	LL_TIM_Init(TIM2, &timbase);
	
	/*Enable timer interrupt*/
	LL_TIM_EnableIT_UPDATE(TIM2);
	
	/*Nested vectored interrupt controller*/
	NVIC_SetPriority(TIM2_IRQn, 0);/*set priority 0, defualt priority 35*/
	NVIC_EnableIRQ(TIM2_IRQn);/*enable NVIC IRQ channal*/

	LL_TIM_EnableCounter(TIM2);
}

void GPIO_Configuretion (void)
{
//	LL_GPIO_InitTypeDef gpio_conf;
//	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
//	gpio_conf.Pin = LL_GPIO_PIN_6;
//	gpio_conf.Mode = LL_GPIO_MODE_OUTPUT;
//	gpio_conf.Pull = LL_GPIO_PULL_NO;
//	gpio_conf.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//	gpio_conf.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//	LL_GPIO_Init(GPIOB, &gpio_conf);
//	
//	gpio_conf.Pin = LL_GPIO_PIN_7;
//	LL_GPIO_Init(GPIOB, &gpio_conf);
//	
	LL_GPIO_InitTypeDef ltc4727_initstruct;
	//configure ltc4727js
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	
	ltc4727_initstruct.Mode = LL_GPIO_MODE_OUTPUT;
	ltc4727_initstruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	ltc4727_initstruct.Pull = LL_GPIO_PULL_NO;
	ltc4727_initstruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	ltc4727_initstruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
	LL_GPIO_Init(GPIOB, &ltc4727_initstruct);
	
	ltc4727_initstruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
	LL_GPIO_Init(GPIOC, &ltc4727_initstruct);
	
}

void TIM2_IRQHandler(void){
	if(LL_TIM_IsActiveFlag_UPDATE(TIM2) == SET)/*unnecessary because */
	{
		LL_TIM_ClearFlag_UPDATE(TIM2);
		//Once update event occure, program jump to here
	}
}

void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */ 
  LL_FLASH_Enable64bitAccess();; 
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  };
  
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
  
	
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  /* This frequency can be calculated through LL RCC macro                          */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3); */
  LL_Init1msTick(32000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}
