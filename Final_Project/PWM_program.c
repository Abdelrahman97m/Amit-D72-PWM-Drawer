
#include "BIT_MATH.h"
#include "STD_TYPES.h"
#include "ATmega32A_Config.h"
#include "PWM_interface.h"
#include "PWM_config.h"

#include "PWM_private.h"


u16 gu16_PWM_TimerTopValue = 0U;


void PWM_InitTimer0(const PWM_ConfigurationType* Config_Ptr);
void PWM_InitTimer1(const PWM_ConfigurationType* Config_Ptr);
void PWM_InitTimer2(const PWM_ConfigurationType* Config_Ptr);


void PWM_Init(const PWM_ConfigurationType* Config_Ptr) {
	switch (Config_Ptr->Tmr_Channel) {
		case PWM_TIMER_0_CHANNEL: PWM_InitTimer0(Config_Ptr); break;
		case PWM_TIMER_1_CHANNEL_A:
		case PWM_TIMER_1_CHANNEL_B: PWM_InitTimer1(Config_Ptr); break;
		case PWM_TIMER_2_CHANNEL: PWM_InitTimer2(Config_Ptr); break;
		default: break;
	}
}

void PWM_SetDuty(PWM_ChannelType Tmr_Channel, PWM_OutputStateType State, u16 Duty_Percent) {
	switch (Tmr_Channel) {
		case PWM_TIMER_0_CHANNEL:
			switch (State) {
				case PWM_NON_INVERTED: *TIMER0_CMP_REG = (u8)(Duty_Percent * PWM_TIMER_0_RESOLUTION / 100.0); break;
				case PWM_INVERTED: *TIMER0_CMP_REG = (u8)((100.0 - Duty_Percent) * PWM_TIMER_0_RESOLUTION / 100.0); break;
				default: break;
			}
			break;
		case PWM_TIMER_1_CHANNEL_A:
			switch (State) {
				case PWM_NON_INVERTED: *(u16*)TIMER1_CMP_A_REG_L = (u16)(Duty_Percent * gu16_PWM_TimerTopValue / 100.0); break;
				case PWM_INVERTED: *(u16*)TIMER1_CMP_A_REG_L = (u16)((100.0 - Duty_Percent) * gu16_PWM_TimerTopValue / 100.0); break;
				default: break;
			}
			break;
		case PWM_TIMER_1_CHANNEL_B:
			switch (State) {
				case PWM_NON_INVERTED: *(u16*)TIMER1_CMP_B_REG_L = (u16)(Duty_Percent * gu16_PWM_TimerTopValue / 100.0); break;
				case PWM_INVERTED: *(u16*)TIMER1_CMP_B_REG_L = (u16)((100.0 - Duty_Percent) * gu16_PWM_TimerTopValue / 100.0); break;
				default: break;
			}
			break;
		case PWM_TIMER_2_CHANNEL:
			switch (State) {
				case PWM_NON_INVERTED: *TIMER2_CMP_REG = (u8)(Duty_Percent * PWM_TIMER_2_RESOLUTION / 100.0); break;
				case PWM_INVERTED: *TIMER2_CMP_REG = (u8)((100.0 - Duty_Percent) * PWM_TIMER_2_RESOLUTION / 100.0); break;
				default: break;
			}
			break;
		default: break;
	}
}


void PWM_InitTimer0(const PWM_ConfigurationType* Config_Ptr) {
	
	SET_BIT(*PORTB_DIR_REG, 3U);
	
	*TIMER0_CTRL_REG = 0x00U;
	
	*TIMER0_CTRL_REG |= (1U<<6U) | ((Config_Ptr->Mode)<<PWM_TIMER_0_FAST_BIT);
	
	*TIMER0_CTRL_REG |= (1U<<5U) | ((Config_Ptr->State)<<PWM_TIMER_0_INVERTING_BIT);
	
	switch (Config_Ptr->Clk_Prescale) {
		case PWM_NO_CLK_SRC:
		case PWM_PRESCALE_1:
		case PWM_PRESCALE_8:
			*TIMER0_CTRL_REG |= (Config_Ptr->Clk_Prescale); break;
		case PWM_PRESCALE_64:
			*TIMER0_CTRL_REG |= ((Config_Ptr->Clk_Prescale)-1U); break;
		case PWM_PRESCALE_256:
		case PWM_PRESCALE_1024:
		case PWM_EXT_CLK_FALLING_EDGE:
		case PWM_EXT_CLK_RISING_EDGE:
			*TIMER0_CTRL_REG |= ((Config_Ptr->Clk_Prescale)-2U); break;
		default: break;
	}
	
	*TIMER0_CMP_REG = 0x00U;
	switch (Config_Ptr->State) {
		case PWM_NON_INVERTED: *TIMER0_CMP_REG = (u8)((Config_Ptr->Duty_Percent) * PWM_TIMER_0_RESOLUTION / 100.0); break;
		case PWM_INVERTED: *TIMER0_CMP_REG = (u8)((100.0 - Config_Ptr->Duty_Percent) * PWM_TIMER_0_RESOLUTION / 100.0); break;
		default: break;
	}
	
	*TIMER0_CNTR_REG = 0x00U;
}

void PWM_InitTimer1(const PWM_ConfigurationType* Config_Ptr) {
	
	switch (Config_Ptr->Tmr_Channel) {
		case PWM_TIMER_1_CHANNEL_A:
			SET_BIT(*PORTD_DIR_REG, 5U);
			break;
		case PWM_TIMER_1_CHANNEL_B:
			SET_BIT(*PORTD_DIR_REG, 4U);
			break;
		default: break;
	}
	
	*TIMER1_CTRL_REG_A = 0x00U;
	*TIMER1_CTRL_REG_B = 0x00U;
	
	*TIMER1_CTRL_REG_A |= (0x02U);
	*TIMER1_CTRL_REG_B |= (1U<<4U) | ((Config_Ptr->Mode)<<PWM_TIMER_1_FAST_BIT);
	
	switch (Config_Ptr->Tmr_Channel) {
		case PWM_TIMER_1_CHANNEL_A:
			*TIMER1_CTRL_REG_A |= (1U<<7U) | ((Config_Ptr->State)<<PWM_TIMER_1_A_INVERTING_BIT);
		case PWM_TIMER_1_CHANNEL_B: 
			*TIMER1_CTRL_REG_A |= (1U<<5U) | ((Config_Ptr->State)<<PWM_TIMER_1_B_INVERTING_BIT);
			break;
		default: break;
	}
	
	switch (Config_Ptr->Clk_Prescale) {
		case PWM_NO_CLK_SRC:
		case PWM_PRESCALE_1:
		case PWM_PRESCALE_8:
			*TIMER1_CTRL_REG_B |= (Config_Ptr->Clk_Prescale); break;
		case PWM_PRESCALE_64:
			*TIMER1_CTRL_REG_B |= ((Config_Ptr->Clk_Prescale)-1U); break;
		case PWM_PRESCALE_256:
		case PWM_PRESCALE_1024:
		case PWM_EXT_CLK_FALLING_EDGE:
		case PWM_EXT_CLK_RISING_EDGE:
			*TIMER1_CTRL_REG_B |= ((Config_Ptr->Clk_Prescale)-2U); break;
		default: break;
	}
	
	gu16_PWM_TimerTopValue = (u16)((F_CPU * (Config_Ptr->Period_ms) / ((Config_Ptr->Prescale_Value) * 1000.0)) - 1U);
	*(u16*)TIMER1_INP_CAPT_REG_L = gu16_PWM_TimerTopValue;
	
	switch (Config_Ptr->Tmr_Channel) {
		case PWM_TIMER_1_CHANNEL_A:
			switch (Config_Ptr->State) {
				case PWM_NON_INVERTED: *(u16*)TIMER1_CMP_A_REG_L = (u16)((Config_Ptr->Duty_Percent) * gu16_PWM_TimerTopValue / 100.0); break;
				case PWM_INVERTED: *(u16*)TIMER1_CMP_A_REG_L = (u16)((100.0 - Config_Ptr->Duty_Percent) * gu16_PWM_TimerTopValue / 100.0); break;
				default: break;
			}
			break;
		case PWM_TIMER_1_CHANNEL_B:
			switch (Config_Ptr->State) {
				case PWM_NON_INVERTED: *(u16*)TIMER1_CMP_B_REG_L = (u16)((Config_Ptr->Duty_Percent) * gu16_PWM_TimerTopValue / 100.0); break;
				case PWM_INVERTED: *(u16*)TIMER1_CMP_B_REG_L = (u16)((100.0 - Config_Ptr->Duty_Percent) * gu16_PWM_TimerTopValue / 100.0); break;
				default: break;
			}
			break;
		default: break;
	}
	
	*(u16*)TIMER1_CNTR_REG_L = 0x0000U;
}

void PWM_InitTimer2(const PWM_ConfigurationType* Config_Ptr) {

	SET_BIT(*PORTD_DIR_REG, 7U);
	*TIMER2_CTRL_REG = 0x00U;
	*TIMER2_CTRL_REG |= (1U<<6U) | ((Config_Ptr->Mode)<<PWM_TIMER_2_FAST_BIT);
	*TIMER0_CTRL_REG |= (1U<<5U) | ((Config_Ptr->State)<<PWM_TIMER_2_INVERTING_BIT);
	
	if ((Config_Ptr->Clk_Prescale) != PWM_EXT_CLK_FALLING_EDGE && (Config_Ptr->Clk_Prescale) != PWM_EXT_CLK_RISING_EDGE) {
		*TIMER2_CTRL_REG |= (Config_Ptr->Clk_Prescale);
	}
	
	*TIMER2_CMP_REG = 0x00U;
	switch (Config_Ptr->State) {
		case PWM_NON_INVERTED: *TIMER2_CMP_REG = (u8)((Config_Ptr->Duty_Percent) * PWM_TIMER_2_RESOLUTION / 100.0); break;
		case PWM_INVERTED: *TIMER2_CMP_REG = (u8)((100.0 - Config_Ptr->Duty_Percent) * PWM_TIMER_2_RESOLUTION / 100.0); break;
		default: break;
	}
	
	*TIMER2_CNTR_REG = 0x00U;
}

