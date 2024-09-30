
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "ATmega32A_Config.h"

#include "TIMERS_config.h"
#include "TIMERS_private.h"
#include "TIMERS_interface.h"
#include "TIMERS_register.h"


const GPT_ConfigurationType gaStrGPT_Config[NUM_TIMERS_OPERATING] =
{
	{
		GPT_TIMER_2,
		GPT_CTC_MODE,
		GPT_PRESCALE_8,
		GPT_INTERRUPT_DISABLED
	}
};


static void GPT_InitTimer0(const GPT_ConfigurationType* Config_Ptr);
static void GPT_InitTimer1(const GPT_ConfigurationType* Config_Ptr);
static void GPT_InitTimer2(const GPT_ConfigurationType* Config_Ptr);

void GPT_Init(const GPT_ConfigurationType* Config_Arr_Ptr) {
	for (u8 i = 0; i < NUM_TIMERS_OPERATING; i++, Config_Arr_Ptr++) {
		switch (Config_Arr_Ptr->Tmr_Channel) {
			case GPT_TIMER_0: GPT_InitTimer0(Config_Arr_Ptr); break;
			case GPT_TIMER_1: GPT_InitTimer1(Config_Arr_Ptr); break;
			case GPT_TIMER_2: GPT_InitTimer2(Config_Arr_Ptr); break;
			default: break;
		}
	}
}

static void GPT_InitTimer0(const GPT_ConfigurationType* Config_Ptr) {
	
	*TIMER0_CTRL_REG = 0x00U;
	*TIMER0_CTRL_REG |= (Config_Ptr->Op_Mode);
	switch (Config_Ptr->Clk_Prescale) {
		case GPT_TIMER_STOP:
		case GPT_PRESCALE_1:
		case GPT_PRESCALE_8:
		*TIMER0_CTRL_REG |= (Config_Ptr->Clk_Prescale); break;
		case GPT_PRESCALE_64:
		*TIMER0_CTRL_REG |= ((Config_Ptr->Clk_Prescale)-1U); break;
		case GPT_PRESCALE_256:
		case GPT_PRESCALE_1024:
		case GPT_EXT_CLK_FALLING_EDGE:
		case GPT_EXT_CLK_RISING_EDGE:
		*TIMER0_CTRL_REG |= ((Config_Ptr->Clk_Prescale)-2U); break;
		default: break;
	}
	/* Set Interrupt State */
	*TIMERS_INT_MASK_REG &= (0xFCU);
	switch (Config_Ptr->Op_Mode) {
		case GPT_NORMAL_MODE: *TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER0_OVF_INT_BIT);
		case GPT_CTC_MODE: *TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER0_CMP_INT_BIT);
		default: break;
	}
}

static void GPT_InitTimer1(const GPT_ConfigurationType* Config_Ptr) {
	
	*TIMER1_CTRL_REG_A = 0x00U;
	*TIMER1_CTRL_REG_B = 0x00U;
	*TIMER1_CTRL_REG_B |= (Config_Ptr->Op_Mode);
	switch (Config_Ptr->Clk_Prescale) {
		case GPT_TIMER_STOP:
		case GPT_PRESCALE_1:
		case GPT_PRESCALE_8:
		*TIMER1_CTRL_REG_B |= (Config_Ptr->Clk_Prescale); break;
		case GPT_PRESCALE_64:
		*TIMER1_CTRL_REG_B |= ((Config_Ptr->Clk_Prescale)-1U); break;
		case GPT_PRESCALE_256:
		case GPT_PRESCALE_1024:
		case GPT_EXT_CLK_FALLING_EDGE:
		case GPT_EXT_CLK_RISING_EDGE:
		*TIMER1_CTRL_REG_B |= ((Config_Ptr->Clk_Prescale)-2U); break;
		default: break;
	}

	*TIMERS_INT_MASK_REG &= (0xE3U);
	switch (Config_Ptr->Op_Mode) {
		case GPT_NORMAL_MODE: *TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER1_OVF_INT_BIT);
		case GPT_CTC_MODE: *TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER1_CMP_A_INT_BIT);
		default: break;
	}
}

static void GPT_InitTimer2(const GPT_ConfigurationType* Config_Ptr) {
	
	*TIMER2_CTRL_REG = 0x00U;
	*TIMER2_CTRL_REG |= (Config_Ptr->Op_Mode);
	if ((Config_Ptr->Clk_Prescale) != GPT_EXT_CLK_FALLING_EDGE && (Config_Ptr->Clk_Prescale) != GPT_EXT_CLK_RISING_EDGE) {
		*TIMER2_CTRL_REG |= (Config_Ptr->Clk_Prescale);
	}
	
	*TIMERS_INT_MASK_REG &= (0x3FU);
	switch (Config_Ptr->Op_Mode) {
		case GPT_NORMAL_MODE: *TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER2_OVF_INT_BIT);
		case GPT_CTC_MODE: *TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER2_CMP_INT_BIT);
		default: break;
	}
}

void GPT_Delay_us(const GPT_ConfigurationType* Config_Ptr, u16 Delay_Time_us) {

	u16 Tmr_Counts = 0U;
	switch (Config_Ptr->Clk_Prescale) {
		case GPT_PRESCALE_1:	Tmr_Counts = (u16)(Delay_Time_us * F_CPU / (1.0 * 1000000UL)); break;
		case GPT_PRESCALE_8:	Tmr_Counts = (u16)(Delay_Time_us * F_CPU / (8.0 * 1000000UL)); break;
		case GPT_PRESCALE_32:	Tmr_Counts = (u16)(Delay_Time_us * F_CPU / (32.0 * 1000000UL)); break;
		case GPT_PRESCALE_64:	Tmr_Counts = (u16)(Delay_Time_us * F_CPU / (64.0 * 1000000UL)); break;
		case GPT_PRESCALE_128:	Tmr_Counts = (u16)(Delay_Time_us * F_CPU / (128.0 * 1000000UL)); break;
		case GPT_PRESCALE_256:	Tmr_Counts = (u16)(Delay_Time_us * F_CPU / (256.0 * 1000000UL)); break;
		case GPT_PRESCALE_1024: Tmr_Counts = (u16)(Delay_Time_us * F_CPU / (1024.0 * 1000000UL)); break;
		default:break;
	}
	switch (Config_Ptr->Tmr_Channel) {
		case GPT_TIMER_0:
		*TIMERS_INT_MASK_REG &= (0xFCU);
		switch (Config_Ptr->Op_Mode) {
			case GPT_CTC_MODE:
			*TIMER0_CMP_REG = (u8)Tmr_Counts;
			*TIMER0_CNTR_REG = 0x00U;
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_CMP_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_CMP_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER0_CMP_INT_BIT);
			return;
			case GPT_NORMAL_MODE:
			*TIMER0_CNTR_REG = 256-(u8)(Tmr_Counts);
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_OVF_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_OVF_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER0_OVF_INT_BIT);
			return;
			default:break;
		}
		break;
		case GPT_TIMER_1:
		*TIMERS_INT_MASK_REG &= (0xC3U);
		switch (Config_Ptr->Op_Mode) {
			case GPT_CTC_MODE:
			*TIMER1_CMP_A_REG_L = (u8)(Tmr_Counts & 0x00FFU);
			*TIMER1_CMP_A_REG_H = (u8)((Tmr_Counts & 0xFF00U)>>8);
			*TIMER1_CNTR_REG_L = 0x00U;
			*TIMER1_CNTR_REG_H = 0x00U;
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_CMP_A_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_CMP_A_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER1_CMP_A_INT_BIT);
			return;
			case GPT_NORMAL_MODE:
			*TIMER1_CNTR_REG_L = (u8)((65535-Tmr_Counts) & 0x00FFU);
			*TIMER1_CNTR_REG_H = (u8)((65535-Tmr_Counts)>>8);
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_OVF_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_OVF_INT_BIT);
			/* Re-set interrupt state */
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER1_OVF_INT_BIT);
			return;
			default:break;
		}
		break;
		case GPT_TIMER_2:
		*TIMERS_INT_MASK_REG &= (0x3FU);
		switch (Config_Ptr->Op_Mode) {
			case GPT_CTC_MODE:
			*TIMER2_CMP_REG = (u8)Tmr_Counts;
			*TIMER2_CNTR_REG = 0x00U;
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_CMP_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_CMP_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER2_CMP_INT_BIT);
			return;
			case GPT_NORMAL_MODE:
			*TIMER2_CNTR_REG = 256-(u8)(Tmr_Counts);
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_OVF_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_OVF_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER2_OVF_INT_BIT);
			return;
			default:break;
		}
		break;
		default: break;
	}
}

void GPT_Delay_ms(const GPT_ConfigurationType* Config_Ptr, u16 Delay_Time_ms) {
	
	u16 Tmr_Counts = 0U;
	switch (Config_Ptr->Clk_Prescale) {
		case GPT_PRESCALE_1:	Tmr_Counts = (u16)(Delay_Time_ms * F_CPU / (1.0 * 1000UL)); break;
		case GPT_PRESCALE_8:	Tmr_Counts = (u16)(Delay_Time_ms * F_CPU / (8.0 * 1000UL)); break;
		case GPT_PRESCALE_32:	Tmr_Counts = (u16)(Delay_Time_ms * F_CPU / (32.0 * 1000UL)); break;
		case GPT_PRESCALE_64:	Tmr_Counts = (u16)(Delay_Time_ms * F_CPU / (64.0 * 1000UL)); break;
		case GPT_PRESCALE_128:	Tmr_Counts = (u16)(Delay_Time_ms * F_CPU / (128.0 * 1000UL)); break;
		case GPT_PRESCALE_256:	Tmr_Counts = (u16)(Delay_Time_ms * F_CPU / (256.0 * 1000UL)); break;
		case GPT_PRESCALE_1024: Tmr_Counts = (u16)(Delay_Time_ms * F_CPU / (1024.0 * 1000UL)); break;
		default:break;
	}
	switch (Config_Ptr->Tmr_Channel) {
		case GPT_TIMER_0:
		*TIMERS_INT_MASK_REG &= (0xFCU);
		switch (Config_Ptr->Op_Mode) {
			case GPT_CTC_MODE:
			*TIMER0_CMP_REG = (u8)Tmr_Counts;
			*TIMER0_CNTR_REG = 0x00U;
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_CMP_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_CMP_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER0_CMP_INT_BIT);
			return;
			case GPT_NORMAL_MODE:
			*TIMER0_CNTR_REG = 256U-(u8)(Tmr_Counts);
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_OVF_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_OVF_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER0_OVF_INT_BIT);
			return;
			default:break;
		}
		break;
		case GPT_TIMER_1:
		*TIMERS_INT_MASK_REG &= (0xC3U);
		switch (Config_Ptr->Op_Mode) {
			case GPT_CTC_MODE:
			*TIMER1_CMP_A_REG_H = (u8)(Tmr_Counts>>8U);
			*TIMER1_CMP_A_REG_L = (u8)(Tmr_Counts & 0x00FFU);
			*TIMER1_CNTR_REG_H = 0x00U;
			*TIMER1_CNTR_REG_L = 0x00U;
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_CMP_A_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_CMP_A_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER1_CMP_A_INT_BIT);
			return;
			case GPT_NORMAL_MODE:
			*TIMER1_CNTR_REG_L = (u8)((65535U-Tmr_Counts) & 0x00FFU);
			*TIMER1_CNTR_REG_H = (u8)((65535U-Tmr_Counts)>>8U);
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_OVF_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_OVF_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER1_OVF_INT_BIT);
			return;
			default:break;
		}
		break;
		case GPT_TIMER_2:
		*TIMERS_INT_MASK_REG &= (0x3FU);
		switch (Config_Ptr->Op_Mode) {
			case GPT_CTC_MODE:
			*TIMER2_CMP_REG = (u8)Tmr_Counts;
			*TIMER2_CNTR_REG = 0x00U;
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_CMP_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_CMP_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER2_CMP_INT_BIT);
			return;
			case GPT_NORMAL_MODE:
			*TIMER2_CNTR_REG = 256U-(u8)(Tmr_Counts);
			while (GET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_OVF_INT_BIT) == 0) ;
			SET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_OVF_INT_BIT);
			*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<TIMER2_OVF_INT_BIT);
			return;
			default:break;
		}
		break;
		default: break;
	}
}

GPT_StatusType GPT_GetTimerStatus(const GPT_ConfigurationType* Config_Ptr) {
	switch (Config_Ptr->Tmr_Channel) {
		case GPT_TIMER_0:
		switch (Config_Ptr->Op_Mode) {
			case GPT_NORMAL_MODE:
			return GET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_OVF_INT_BIT);
			case GPT_CTC_MODE:
			return GET_BIT(*TIMERS_INT_FLAG_REG, TIMER0_CMP_INT_BIT);
			default: break;
		}
		case GPT_TIMER_1:
		switch (Config_Ptr->Op_Mode) {
			case GPT_NORMAL_MODE:
			return GET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_OVF_INT_BIT);
			case GPT_CTC_MODE:
			return GET_BIT(*TIMERS_INT_FLAG_REG, TIMER1_CMP_A_INT_BIT);
			default: break;
		}
		case GPT_TIMER_2:
		switch (Config_Ptr->Op_Mode) {
			case GPT_NORMAL_MODE:
			return GET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_OVF_INT_BIT);
			case GPT_CTC_MODE:
			return GET_BIT(*TIMERS_INT_FLAG_REG, TIMER2_CMP_INT_BIT);
			default: break;
		}
		default: break;
	}
	return GPT_TIMER_FINISHED;
}