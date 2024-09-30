
#include "ATmega32A_Config.h"
#include "STD_TYPES.h"

#include "GLCD_config.h"
#include "GLCD_interface.h"

#include "PWM_interface.h"
#include "ICU_interface.h"

#include "PWM_Drawer_interface.h"
#include "PWM_Drawer_config.h"



char gac_Line_Patterns[NUM_SPECIAL_CHARACTERS][8] = {
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00},
	{0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1C, 0x00},
	{0x1C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x07, 0x00},
	{0x01, 0x11, 0x09, 0x1D, 0x09, 0x11, 0x00, 0x00},
	{0x10, 0x11, 0x12, 0x17, 0x12, 0x11, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00}
};

void PWMDrawer_Init(void) {
	#if GENERATE_PWM_FROM_SAME_MCU == 1
		GLCD_Init();
		GLCD_ClearDisplay();
		PWM_Init(&gaStrPWM_Config[0]);		
		ICU_Init(&gStrICU_Config);			
	#elif GLCD_MCU == 1
		GLCD_Init();
		GLCD_ClearDisplay();
		ICU_Init(&gStrICU_Config);			
	#else
		PWM_Init(&gaStrPWM_Config[0]);		
	#endif
}

void PWMDrawer_MeasureSignal(PWMDrawer_SignalAttributesType* Signal_Data_Ptr) {
	ICU_DutyCycleType strLocal_Duty = {0, 0};
	ICU_GetDutyCycle(ICU_TIMER_1, 8U, &strLocal_Duty);
	Signal_Data_Ptr->Period_Time_ms = strLocal_Duty.Period_Time;
	Signal_Data_Ptr->High_Time_ms = strLocal_Duty.High_Time;
	Signal_Data_Ptr->Low_Time_ms = strLocal_Duty.Period_Time - strLocal_Duty.High_Time;
	
	Signal_Data_Ptr->Freq_kHz = (1.0 / (Signal_Data_Ptr->Period_Time_ms));
	Signal_Data_Ptr->Duty_Percent = (u8)((Signal_Data_Ptr->High_Time_ms) * 100.0 / (Signal_Data_Ptr->Period_Time_ms));
}

void PWMDrawer_DrawSignal(PWMDrawer_SignalAttributesType* Signal_Data_Ptr, f32 Scale_ms) {
	GLCD_ClearDisplay();
	
	GLCD_GoToLine(0);
	char txt1[15] = "Frequency=\0";
	GLCD_DisplayString(txt1);
	GLCD_DisplayFloatingPoint(Signal_Data_Ptr->Freq_kHz);
	char txt2[5] = " kHz\0";
	GLCD_DisplayString(txt2);

	GLCD_GoToLine(1);
	char txt3[15] = "Duty=\0";
	GLCD_DisplayString(txt3);
	GLCD_DisplayInteger(Signal_Data_Ptr->Duty_Percent);
	char txt4[5] = " %\0";
	GLCD_DisplayString(txt4);

	GLCD_GoToLine(6);
	u8 pixel_width_count = 0;
	u8 i;
	while (pixel_width_count < GLCD_MAX_LINE_PIXEL_WIDTH - 8U) {
		GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_RISING_EDGE); pixel_width_count += 5;
		for (i = 0; i < ((u8)(Signal_Data_Ptr->High_Time_ms / Scale_ms)); i++) {
			GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_HIGH_LINE); pixel_width_count += 5;
		}
		GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_FALLING_EDGE); pixel_width_count += 5;
		for (i = 0; i < ((u8)(Signal_Data_Ptr->Low_Time_ms / Scale_ms)); i++) {
			GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_LOW_LINE); pixel_width_count += 5;
		}
	}

	GLCD_GoToLine(5);
	GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_LEFT_ARROW_HEAD);
	for (i = 0; i < ((u8)(Signal_Data_Ptr->High_Time_ms / Scale_ms)); i++) {
		GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_ARROW_MIDDLE_BODY);
	}
	GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_ARROW_MIDDLE_BODY);
	for (i = 0; i < ((u8)(Signal_Data_Ptr->Low_Time_ms / Scale_ms)); i++) {
		GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_ARROW_MIDDLE_BODY);
	}
	GLCD_DisplaySpecialPattern(PWM_DRAWER_GLCD_RIGHT_ARROW_HEAD);

	GLCD_GoToLine(4);
	char txt5[15] = " Time=\0";
	GLCD_DisplayString(txt5);
	GLCD_DisplayFloatingPoint(Signal_Data_Ptr->Period_Time_ms);
	char txt6[5] = " ms\0";
	GLCD_DisplayString(txt6);
}

