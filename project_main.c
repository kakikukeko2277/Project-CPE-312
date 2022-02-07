
void Time_stamp(distance)
{
	while(1)
	{
 		cnt = LL_TIM_GetCounter(TIM4);
		if (cnt >= LL_TIM_GetAutoReload(TIM4))
		{
			LL_TIM_SetCounter(TIM4, 0);
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
		if(cnt >= LL_TIM_GetAutoReload(TIM4))
		{
			cnt = 0;
			break;
		}
	}
}

void Reset_show_segment()
{
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15);
}

void Show_segment(time){
	for(uint8_t i=0	; i<4; i++)
	{
		Reset_show_segment();
		LL_GPIO_SetOutputPin(GPIOB, seg[arrtime[i]]);
		LL_GPIO_SetOutputPin(GPIOC, digit[i]);
	}
}
