

#define  F_CPU 16000000UL
#include <util/delay.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "ATmega32A_Config.h"

#include "DIO_interface.h"

#include "GLCD_config.h"
#include "GLCD_interface.h"
#include "GLCD_private.h"



#if GLCD_USE_TIMERS_FOR_DELAY == 1
#include "TIMERS_interface.h"
#else
#include <util/delay.h>
#endif

u8 gu8_Curr_X_Address = 0;
u8 gu8_Curr_Y_Address = 0;
u8 gu8_Curr_Page = 0;

void GLCD_Init(void) {
	#if GLCD_USE_TIMERS_FOR_DELAY == 1
	GPT_Init(gaStrGPT_Config);
	#endif
	
	DIO_u8SetPinDirection(GLCD_CTRL_PORT, GLCD_CTRL_EN, DIO_PIN_OUTPUT);
	DIO_u8SetPinDirection(GLCD_CTRL_PORT, GLCD_CTRL_RS, DIO_PIN_OUTPUT);
	DIO_u8SetPinDirection(GLCD_CTRL_PORT, GLCD_CTRL_RW, DIO_PIN_OUTPUT);
	DIO_u8SetPinDirection(GLCD_CTRL_PORT, GLCD_CTRL_RST, DIO_PIN_OUTPUT);
	DIO_u8SetPinDirection(GLCD_CTRL_PORT, GLCD_CTRL_CS1, DIO_PIN_OUTPUT);
	DIO_u8SetPinDirection(GLCD_CTRL_PORT, GLCD_CTRL_CS2, DIO_PIN_OUTPUT);

	DIO_u8SetPortDirection(GLCD_DATA_PORT, DIO_PORT_OUTPUT);
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_RST, DIO_PIN_HIGH);
	
	GLCD_SelectPage(GLCD_BOTH_PAGES);
	GLCD_SendInstruction(GLCD_DISPLAY_OFF);
	GLCD_SendInstruction(GLCD_SET_X_ADDRESS_PREFIX + 0); 
	gu8_Curr_X_Address = 0;
	GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + 0);  
	gu8_Curr_Y_Address = 0;
	GLCD_SendInstruction(GLCD_SET_Z_ADDRESS_PREFIX + 0);  
	GLCD_SendInstruction(GLCD_DISPLAY_ON);
	GLCD_SelectPage(GLCD_PAGE_0);
	gu8_Curr_Page = 0;
}

void GLCD_SelectPage(GLCD_PageSelectType Page) {
	switch (Page) {
		case GLCD_NO_PAGES:
		DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_CS1, DIO_PIN_LOW);
		DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_CS2, DIO_PIN_LOW);
		break;
		case GLCD_PAGE_0:
		DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_CS1, DIO_PIN_HIGH);
		DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_CS2, DIO_PIN_LOW);
		break;
		case GLCD_PAGE_1:
		DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_CS1, DIO_PIN_LOW);
		DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_CS2, DIO_PIN_HIGH);
		break;
		case GLCD_BOTH_PAGES:
		DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_CS1, DIO_PIN_HIGH);
		DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_CS2, DIO_PIN_HIGH);
		break;
		default: break;
	}
}

void GLCD_SendInstruction(GLCD_CommandType Instruction) {
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_RS, GLCD_INSTRUCTION_REG);
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_RW, GLCD_WRITE_MODE);
	
	DIO_u8SetPortValue(GLCD_DATA_PORT, Instruction);
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_EN, DIO_PIN_HIGH);
	#if GLCD_USE_TIMERS_FOR_DELAY == 1
	GPT_Delay_us(&gaStrGPT_Config[0], 5);
	#else
	_delay_us(5);
	#endif
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_EN, DIO_PIN_LOW);
}

void GLCD_SendData(u8 Data) {
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_RS, GLCD_DATA_REG);
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_RW, GLCD_WRITE_MODE);
	
	DIO_u8SetPortValue(GLCD_DATA_PORT, Data);
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_EN, DIO_PIN_HIGH);
	#if GLCD_USE_TIMERS_FOR_DELAY == 1
	GPT_Delay_us(&gaStrGPT_Config[0], 5);
	#else
	_delay_us(5);
	#endif
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_EN, DIO_PIN_LOW);
}

void GLCD_Reset(void) {
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_RST, DIO_PIN_LOW);
	#if GLCD_USE_TIMERS_FOR_DELAY == 1
	GPT_Delay_us(&gaStrGPT_Config[0], 10);
	#else
	_delay_us(10);
	#endif
	
	DIO_u8SetPinValue(GLCD_CTRL_PORT, GLCD_CTRL_RST, DIO_PIN_HIGH);
}

void GLCD_DisplaySpecialPattern(u64 Pattern) {
	for(u8 i = 0; i < 5; i++) {
		GLCD_SendInstruction(GLCD_SET_X_ADDRESS_PREFIX + gu8_Curr_X_Address);
		GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + gu8_Curr_Y_Address);
		GLCD_SendData((Pattern>>(i*8U)) & 0x00000000FFUL);
		gu8_Curr_Y_Address++;
		if (gu8_Curr_Y_Address >= GLCD_MAX_PAGE_PIXEL_WIDTH && gu8_Curr_Page == 0) {
			GLCD_SelectPage(GLCD_PAGE_1);
			gu8_Curr_Y_Address = 0;
		}
	}
}

void GLCD_DisplayCharacter(u8 Data) {
	if ((gu8_Curr_Page == 0) | (gu8_Curr_Y_Address < GLCD_MAX_PAGE_PIXEL_WIDTH - 7U)) {
		
		switch (Data) {
			case 'A': case 'a': GLCD_DisplaySpecialPattern(GLCD_CHAR_A); break;
			case 'F': case 'f': GLCD_DisplaySpecialPattern(GLCD_CHAR_F); break;
			case 'R': case 'r': GLCD_DisplaySpecialPattern(GLCD_CHAR_R); break;
			case 'E': case 'e': GLCD_DisplaySpecialPattern(GLCD_CHAR_E); break;
			case 'Q': case 'q': GLCD_DisplaySpecialPattern(GLCD_CHAR_Q); break;
			case 'U': case 'u': GLCD_DisplaySpecialPattern(GLCD_CHAR_U); break;
			case 'N': case 'n': GLCD_DisplaySpecialPattern(GLCD_CHAR_N); break;
			case 'C': case 'c': GLCD_DisplaySpecialPattern(GLCD_CHAR_C); break;
			case 'Y': case 'y': GLCD_DisplaySpecialPattern(GLCD_CHAR_Y); break;
			case 'D': case 'd': GLCD_DisplaySpecialPattern(GLCD_CHAR_D); break;
			case 'T': case 't': GLCD_DisplaySpecialPattern(GLCD_CHAR_T); break;
			case 'K': case 'k': GLCD_DisplaySpecialPattern(GLCD_CHAR_K); break;
			case 'H': case 'h': GLCD_DisplaySpecialPattern(GLCD_CHAR_H); break;
			case 'Z': case 'z': GLCD_DisplaySpecialPattern(GLCD_CHAR_Z); break;
			case 'M': case 'm': GLCD_DisplaySpecialPattern(GLCD_CHAR_M); break;
			case 'S': case 's': GLCD_DisplaySpecialPattern(GLCD_CHAR_S); break;
			case 'I': case 'i': GLCD_DisplaySpecialPattern(GLCD_CHAR_I); break;
			case '%': GLCD_DisplaySpecialPattern(GLCD_CHAR_PERCENT); break;
			case '=': GLCD_DisplaySpecialPattern(GLCD_CHAR_EQUAL); break;
			case '.': GLCD_DisplaySpecialPattern(GLCD_CHAR_POINT); break;
			case ' ': GLCD_DisplaySpecialPattern(GLCD_CHAR_SPACE); break;
			case '-': GLCD_DisplaySpecialPattern(GLCD_CHAR_NEGATIVE); break;
			case '0': GLCD_DisplaySpecialPattern(GLCD_CHAR_0); break;
			case '1': GLCD_DisplaySpecialPattern(GLCD_CHAR_1); break;
			case '2': GLCD_DisplaySpecialPattern(GLCD_CHAR_2); break;
			case '3': GLCD_DisplaySpecialPattern(GLCD_CHAR_3); break;
			case '4': GLCD_DisplaySpecialPattern(GLCD_CHAR_4); break;
			case '5': GLCD_DisplaySpecialPattern(GLCD_CHAR_5); break;
			case '6': GLCD_DisplaySpecialPattern(GLCD_CHAR_6); break;
			case '7': GLCD_DisplaySpecialPattern(GLCD_CHAR_7); break;
			case '8': GLCD_DisplaySpecialPattern(GLCD_CHAR_8); break;
			case '9': GLCD_DisplaySpecialPattern(GLCD_CHAR_9); break;
			default: break;
		}
		GLCD_SendInstruction(GLCD_SET_X_ADDRESS_PREFIX + gu8_Curr_X_Address);
		GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + gu8_Curr_Y_Address);
		GLCD_SendData(0);
		gu8_Curr_Y_Address++;
		if (gu8_Curr_Y_Address >= GLCD_MAX_PAGE_PIXEL_WIDTH && gu8_Curr_Page == 0) {
			GLCD_SelectPage(GLCD_PAGE_1);
			gu8_Curr_Y_Address = 0;
		}
	}
}

void GLCD_DisplayString(u8* Data_Ptr) {
	while (*Data_Ptr != '\0') {
		GLCD_DisplayCharacter(*Data_Ptr);
		Data_Ptr++;
	}
}

void GLCD_DisplayInteger(s32 Data) {
	if (Data < 0) {
		GLCD_DisplayCharacter('-');
		Data = -1 * Data;
	}
	u8 u8Local_Digit = 0U;
	char str_reversed[5] = "";
	char* str_ptr = str_reversed;
	do {
		u8Local_Digit = Data%10;
		*str_ptr = (char)(48 + u8Local_Digit); 
		Data = Data/10;
		str_ptr++;
	} while (Data > 0);
	do {
		str_ptr--;
		GLCD_DisplayCharacter(*str_ptr);
	} while (str_ptr != str_reversed);
}

void GLCD_DisplayFloatingPoint(f32 Data) {
	
	u16 IntegerPart = (u16)Data;
	GLCD_DisplayInteger(IntegerPart);
	GLCD_DisplayCharacter('.');
	f32 DecimalPart = (Data - IntegerPart);
	u8 Digit = 0;
	for (u8 i = 0; i < 3; i++) {
		Digit = (u8)(DecimalPart * 10.0);
		GLCD_DisplayCharacter((char)(48+Digit));
		DecimalPart = ((DecimalPart*10.0) - Digit);
	}
}

void GLCD_GoToLine(u8 Line) {
	if (Line < GLCD_MAX_LINES) {
		GLCD_SelectPage(GLCD_PAGE_0);
		gu8_Curr_X_Address = Line;
		GLCD_SendInstruction(GLCD_SET_X_ADDRESS_PREFIX + gu8_Curr_X_Address);
		gu8_Curr_Y_Address = 0;
		GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + gu8_Curr_Y_Address);
	}
}

void GLCD_ClearDisplay(void) {
	for (u8 Line = 0; Line < GLCD_MAX_LINES; Line++) {
		GLCD_GoToLine(Line);
		GLCD_SelectPage(GLCD_BOTH_PAGES);
		for (u8 i = 0; i < GLCD_MAX_PAGE_PIXEL_WIDTH; i++) {
			GLCD_SendInstruction(GLCD_SET_Y_ADDRESS_PREFIX + i);
			GLCD_SendData(0);
		}
	}
	GLCD_Reset();
	GLCD_SelectPage(GLCD_PAGE_0);
	gu8_Curr_Page = 0;
	GLCD_GoToLine(0);
}
