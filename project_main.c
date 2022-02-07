/*Base register adddress header file*/
#include "stm32l1xx.h"
/*Library related header files*/
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_tim.h"
#include "stm32l1xx_ll_usart.h"
#include <string.h>
#include "ESP8266_lowlevel_conf.h"

void SystemClock_Config(void);
void delay();
	
#define MAX_RESP_BUFFER_SIZE			200

uint8_t resp[MAX_RESP_BUFFER_SIZE] = {0};
uint8_t idx;
char mnum[6];
char hnum[5];
/*----------------WIFI config------------------------------------------------------*/
uint8_t ESP8266_SendCmd(uint8_t* cmd)
{
	ESP_USART_LOWLEVEL_Transmit(cmd);
	while(1)
	{
		
		(ESP_USART_LOWLEVEL_Recv(resp, idx) != 1)?(idx = (idx + 1) % MAX_RESP_BUFFER_SIZE):(idx);		
		if(strstr((const char*)resp, "OK"))
		{
			return 0;
		}
	}
}
void ESP_ServerStart()
{
	while(1)
	{
		
		(ESP_USART_LOWLEVEL_Recv(resp, idx) != 1)?(idx = (idx + 1) % MAX_RESP_BUFFER_SIZE):(idx);		
		if(strstr((const char*)resp, "CONNECT"))
		{
			return;
		}
	}
}
void ESP8266_RespBufferReset(void)
{
	memset(resp, NULL, MAX_RESP_BUFFER_SIZE);
	idx = 0;
}
/*----------------WIFI config------------------------------------------------------*/
void TIMBase_Config(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_EnableCounter(TIM2);
	
	/*create object name*/
	LL_TIM_InitTypeDef timbase;
	
	/*enable clock on bus path APB1*/
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9);
	
	timbase.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase.CounterMode = LL_TIM_COUNTERMODE_UP;
	//10s
	timbase.Autoreload = 1000 - 1;
	timbase.Prescaler = 32000- 1;
	
	LL_TIM_Init(TIM9, &timbase);
	
	/*Enable timer interrupt*/
	LL_TIM_EnableIT_UPDATE(TIM9);
	
	/*Nested vectored interrupt controller*/
	NVIC_SetPriority(TIM9_IRQn, 0);/*set priority 0, defualt priority 35*/
	NVIC_EnableIRQ(TIM9_IRQn);/*enable NVIC IRQ channal*/

	LL_TIM_EnableCounter(TIM9);
}

void GPIO_Config(void)
{
		LL_GPIO_InitTypeDef hcsr04_gpio;
		
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
		
		hcsr04_gpio.Mode = LL_GPIO_MODE_OUTPUT;
		hcsr04_gpio.Pull = LL_GPIO_PULL_NO;
		hcsr04_gpio.Pin = LL_GPIO_PIN_2;
		hcsr04_gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
		hcsr04_gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
		LL_GPIO_Init(GPIOA, &hcsr04_gpio);
	
		hcsr04_gpio.Mode = LL_GPIO_MODE_INPUT;
		hcsr04_gpio.Pin = LL_GPIO_PIN_1;
		hcsr04_gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
		hcsr04_gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
		LL_GPIO_Init(GPIOA, &hcsr04_gpio);
	
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_2);
}


void GPIO7sec_Config(void)
{
		LL_GPIO_InitTypeDef ltc4727_initstruct;
	  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	  ltc4727_initstruct.Mode = LL_GPIO_MODE_OUTPUT;
		ltc4727_initstruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
		ltc4727_initstruct.Pull = LL_GPIO_PULL_NO;
		ltc4727_initstruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
		ltc4727_initstruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 |  LL_GPIO_PIN_15;
		LL_GPIO_Init(GPIOB, &ltc4727_initstruct);
	
		ltc4727_initstruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
		LL_GPIO_Init(GPIOC, &ltc4727_initstruct);
	
}

void Time_stamp();
void Show_segment();
void Reset_show_segment();

uint16_t rise_timestamp = 0;
uint16_t fall_timestamp = 0;
uint16_t up_cycle = 0;

uint8_t state = 0;
float period = 0;
float distance = 0;
int s = 0;
uint32_t TIM2CLK;
uint32_t PSC;
uint32_t cnt = 0;
uint8_t second = 0;
uint8_t minute = 0;
uint8_t hour = 0;
uint8_t s1 = 0;
uint8_t s2 = 0;
uint8_t m1 =0 ;
uint8_t m2 = 0 ;

int main()
{    
		SystemClock_Config();
	  uint8_t  o;
	  uint8_t check_digit = 4;
	  uint32_t number[10] = { LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14,	//0
										LL_GPIO_PIN_10 | LL_GPIO_PIN_11,	//1
										LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_15,	//2
									  LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_15,	//3
										LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,	//4
										LL_GPIO_PIN_2 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,	//5
										LL_GPIO_PIN_2 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,	//6
										LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11,	//7
										LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15,	//8
										LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15 //9 
								   };
		uint32_t digit[4] = {LL_GPIO_PIN_0 , LL_GPIO_PIN_1, LL_GPIO_PIN_2,LL_GPIO_PIN_3};
		GPIO_Config();
		GPIO7sec_Config();
		TIMBase_Config();
	ESP_USART_LOWLEVEL_Conf();
	ESP_USART_Start();
	
		
	ESP8266_SendCmd((uint8_t*)"AT+RST\r\n");
	ESP8266_RespBufferReset();	
	ESP8266_SendCmd((uint8_t*)"AT+RESTORE\r\n");
	ESP8266_RespBufferReset();
	LL_mDelay(1000); //Prevent ESP8266 flooding message
	ESP8266_SendCmd((uint8_t*)"AT+CWMODE=1\r\n");
	ESP8266_RespBufferReset();
	ESP8266_SendCmd((uint8_t*)"AT+RST\r\n");
	ESP8266_RespBufferReset();	
	LL_mDelay(1000);
	ESP8266_SendCmd((uint8_t*)"AT+CWJAP=\"padthai_2G\",\"1911310231\"\r\n");
	ESP8266_RespBufferReset();	
	ESP8266_SendCmd((uint8_t*)"AT+CWJAP?\r\n");
	ESP8266_RespBufferReset();	
	LL_mDelay(1000);
    
		while(1)
		{
			if (s ==0)
			{
			switch(state)
			{
				case 0:
					//Trigger measurement
					LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_2);
					LL_mDelay(1);
					LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_2);
					state = 1;
				break;
				
				case 1:
					if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1))
					{
						rise_timestamp = LL_TIM_GetCounter(TIM2);
						state = 2;
					}
				break;
					
				case 2:
					if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1) == RESET)
					{
						fall_timestamp = LL_TIM_GetCounter(TIM2);
						//Calculate uptime
						if(fall_timestamp > rise_timestamp)
						{
							up_cycle = fall_timestamp - rise_timestamp;
						}
						else if(fall_timestamp < rise_timestamp)
						{
							up_cycle = (LL_TIM_GetAutoReload(TIM2) - rise_timestamp) + fall_timestamp + 1; 
						}
						else
						{
							//cannot measure at this freq
							up_cycle = 0;
						}
						
						if(up_cycle != 0)
						{
							PSC = LL_TIM_GetPrescaler(TIM2) + 1;
							TIM2CLK = SystemCoreClock / PSC;
							
							period = (up_cycle*(PSC) * 1.0) / (TIM2CLK * 1.0); //calculate uptime period
							distance = (period * 340) / 2; //meter unit
							if(distance<= 0.05)
							{
							cnt = LL_TIM_GetCounter(TIM9);
									if (cnt >= 1000-3)
									{
										LL_TIM_SetCounter(TIM9, 0);
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
										  break;
									}
							}
							else
							{
								if (second > 10)
									s = 1;
								else 
									s = 0;
							}
						}
						state = 0;
					}
				break;
			}
		}
			else
				break;

   }
	 /*----------------CLIENT------------------------------------------------------*/
	m1 = minute/10;
	m2 = minute%10;
	s1 = second/10;
	s2 = second%10;
	sprintf(hnum, "%d%d", m1,m2);
	sprintf(mnum, ":%d%d", s1,s2);
	strcat(hnum,mnum);
	ESP8266_SendCmd((uint8_t*)"AT+CIPMUX=0\r\n");
  ESP8266_RespBufferReset();
  ESP8266_SendCmd((uint8_t*)"AT+CIPSTART=\"TCP\",\"192.168.1.42\",1100\r\n");
  ESP8266_RespBufferReset();
  	
  ESP8266_SendCmd((uint8_t*)"AT+CIPSEND=5\r\n");
  ESP8266_RespBufferReset();
	ESP_USART_LOWLEVEL_Transmit((uint8_t *)hnum);
	
		while(1){
		
		for (o = 0 ; o < 5 ; o++){
			LL_GPIO_ResetOutputPin(GPIOC,LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3);
			LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 |
															LL_GPIO_PIN_13 | LL_GPIO_PIN_14 |  LL_GPIO_PIN_15);
		switch (o){
			case 0:
				LL_GPIO_SetOutputPin(GPIOB,number[m1]);
				LL_GPIO_SetOutputPin(GPIOC,digit[o]);
				break;
			case 1:
				LL_GPIO_SetOutputPin(GPIOB,number[m2]);
				LL_GPIO_SetOutputPin(GPIOC,digit[o]);
			case 2:
				LL_GPIO_SetOutputPin(GPIOB,number[s1]);
				LL_GPIO_SetOutputPin(GPIOC,digit[o]);
				break;
			case 3:
				LL_GPIO_SetOutputPin(GPIOB,number[s2]);
				LL_GPIO_SetOutputPin(GPIOC,digit[o]);
				break;
			case 4:
				LL_GPIO_SetOutputPin(GPIOB,LL_GPIO_PIN_2|LL_GPIO_PIN_10);
			}
		//LL_mDelay(1000);
}
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
void TIM9_IRQHandler(void){
	if(LL_TIM_IsActiveFlag_UPDATE(TIM9) == SET)/*unnecessary because */
	{
		LL_TIM_ClearFlag_UPDATE(TIM9);
		//Once update event occure, program jump to here
	}
}
void delay()
{
	unsigned int i = 0;
	for (i=0; i<100; ++i);
}
