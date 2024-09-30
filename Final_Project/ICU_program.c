
#include "ATmega32A_Config.h"
#include "STD_TYPES.h"
#include "BIT_MATH.h"


#include "ICU_interface.h"
#include "ICU_config.h"


void ICU_InitTimer1(const ICU_ConfigurationType* Config_Ptr);
void ICU_GetDutyCycle_Timer1(u16 Prescale_Value, ICU_DutyCycleType* Duty_Ptr);


void ICU_Init(const ICU_ConfigurationType* Config_Ptr) {
	switch (Config_Ptr->Tmr_Channel) {
		case ICU_TIMER_1: ICU_InitTimer1(Config_Ptr); break;
		default: break;
	}
}

void ICU_SetTriggerEdge(ICU_TimerChannelType Tmr_Channel, ICU_TriggerEdgeType Edge) {
	CLR_BIT(*TIMER1_CTRL_REG_B, ICU_TIMER1_EDGE_SELECT_BIT);
	switch (Tmr_Channel) {
		case ICU_TIMER_1: *TIMER1_CTRL_REG_B |= (Edge<<ICU_TIMER1_EDGE_SELECT_BIT); break;
		default: break;
	}
}

void ICU_ClearTimerValue(ICU_TimerChannelType Tmr_Channel) {
	switch (Tmr_Channel) {
		case ICU_TIMER_1: *(u16*)TIMER1_CNTR_REG_L = 0x0000U; break;
		default: break;
	}
}

u16 ICU_GetInputCaptureValue(ICU_TimerChannelType Tmr_Channel) {
	u16 u16Local_Value = 0U;
	switch (Tmr_Channel) {
		case ICU_TIMER_1: u16Local_Value = (*(u16*)TIMER1_INP_CAPT_REG_L);
		default: break;
	}
	return u16Local_Value;
}

ICU_StatusType ICU_GetStatus(ICU_TimerChannelType Tmr_Channel) {
	switch (Tmr_Channel) {
		case ICU_TIMER_1: 
			if (GET_BIT(*TIMERS_INT_FLAG_REG, ICU_TIMER1_INT_BIT) == 1) {
				SET_BIT(*TIMERS_INT_FLAG_REG, ICU_TIMER1_INT_BIT);  
				return ICU_EVENT_CATURED;
			} else {
				return ICU_RUNNING;
			}
		default: break;
	}
	return ICU_RUNNING;
}

void ICU_GetDutyCycle(ICU_TimerChannelType Tmr_Channel, u16 Prescale_Value, ICU_DutyCycleType* Duty_Ptr) {
	switch (Tmr_Channel) {
		case ICU_TIMER_1:
			ICU_GetDutyCycle_Timer1(Prescale_Value, Duty_Ptr);
			break;
		default: break;
	}
}


void ICU_InitTimer1(const ICU_ConfigurationType* Config_Ptr) {
	
	CLR_BIT(*PORTD_DIR_REG, 6U);
	
	*TIMER1_CTRL_REG_A = 0x00U;
	*TIMER1_CTRL_REG_B = 0x00U;
	
	
	*TIMER1_CTRL_REG_B |= (Config_Ptr->Clk_Prescale);
	
	*TIMER1_CTRL_REG_B |= ((Config_Ptr->Trigger_Edge)<<ICU_TIMER1_EDGE_SELECT_BIT);
	
	*TIMERS_INT_MASK_REG |= ((Config_Ptr->Int_State)<<ICU_TIMER1_INT_BIT);
	
	*(u16*)TIMER1_CNTR_REG_L = 0x0000U;
	*(u16*)TIMER1_INP_CAPT_REG_L = 0x0000U;
}

void ICU_GetDutyCycle_Timer1(u16 Prescale_Value, ICU_DutyCycleType* Duty_Ptr) {
	
	SET_BIT(*TIMERS_INT_FLAG_REG, ICU_TIMER1_INT_BIT);
	ICU_SetTriggerEdge(ICU_TIMER_1, ICU_TRIGGER_RISING_EDGE);
	
	while (ICU_GetStatus(ICU_TIMER_1) == ICU_RUNNING) {
		; 
	}

	u32 u32Local_ON_Counts = 0U;
	ICU_SetTriggerEdge(ICU_TIMER_1, ICU_TRIGGER_FALLING_EDGE);
	ICU_ClearTimerValue(ICU_TIMER_1);
	
	SET_BIT(*TIMERS_INT_FLAG_REG, ICU_TIMER1_INT_BIT);
	
	while (ICU_GetStatus(ICU_TIMER_1) == ICU_RUNNING) {
		if (GET_BIT(*TIMERS_INT_FLAG_REG, ICU_TIMER1_OVF_FLAG_BIT) == 1) {
			SET_BIT(*TIMERS_INT_FLAG_REG, ICU_TIMER1_OVF_FLAG_BIT);
			u32Local_ON_Counts += ICU_TIMER1_RESOLUTION;
		}
	}
	u32Local_ON_Counts += ICU_GetInputCaptureValue(ICU_TIMER_1);

	u32 u32Local_OFF_Counts = 0U;
	ICU_SetTriggerEdge(ICU_TIMER_1, ICU_TRIGGER_RISING_EDGE);
	ICU_ClearTimerValue(ICU_TIMER_1);
	
	SET_BIT(*TIMERS_INT_FLAG_REG, ICU_TIMER1_INT_BIT);
	
	while (ICU_GetStatus(ICU_TIMER_1) == ICU_RUNNING) {
		if (GET_BIT(*TIMERS_INT_FLAG_REG, ICU_TIMER1_OVF_FLAG_BIT) == 1) {
			SET_BIT(*TIMERS_INT_FLAG_REG, ICU_TIMER1_OVF_FLAG_BIT);
			u32Local_OFF_Counts += ICU_TIMER1_RESOLUTION;
		}
	}
	u32Local_OFF_Counts = ICU_GetInputCaptureValue(ICU_TIMER_1);


	Duty_Ptr->High_Time = (f32)(u32Local_ON_Counts * Prescale_Value * 1000.0 / F_CPU);
	Duty_Ptr->Period_Time = (f32)((u32Local_ON_Counts+u32Local_OFF_Counts) * Prescale_Value * 1000.0 / F_CPU);
}
