
#ifndef PWM_DRAWER_INTERFACE_H_
#define PWM_DRAWER_INTERFACE_H_



#define NUM_SPECIAL_CHARACTERS				        (7U)

#define PWM_DRAWER_GLCD_HIGH_LINE					(0x0202020202UL)
#define PWM_DRAWER_GLCD_LOW_LINE					(0x4040404040UL)
#define PWM_DRAWER_GLCD_RISING_EDGE					(0x02027E4040UL)
#define PWM_DRAWER_GLCD_FALLING_EDGE				(0x40407E0202UL)
#define PWM_DRAWER_GLCD_RIGHT_ARROW_HEAD			(0x003E081C2AUL)
#define PWM_DRAWER_GLCD_LEFT_ARROW_HEAD				(0x2A1C083E00UL)
#define PWM_DRAWER_GLCD_ARROW_MIDDLE_BODY			(0x0808080808UL)


typedef struct {
	u8 Duty_Percent;
	f32 High_Time_ms;
	f32 Low_Time_ms;
	f32 Period_Time_ms;
	f32 Freq_kHz;
}PWMDrawer_SignalAttributesType;


void PWMDrawer_Init(void);
void PWMDrawer_MeasureSignal(PWMDrawer_SignalAttributesType* Signal_Data_Ptr);
void PWMDrawer_DrawSignal(PWMDrawer_SignalAttributesType* Signal_Data_Ptr, f32 Scale_ms);


#endif 