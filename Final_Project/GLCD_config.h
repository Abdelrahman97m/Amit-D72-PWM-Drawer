

#ifndef GLCD_CONFIG_H_
#define GLCD_CONFIG_H_



#define GLCD_DATA_PORT    DIO_PORTA

#define GLCD_CTRL_PORT    DIO_PORTB
#define GLCD_CTRL_EN      DIO_PIN0
#define GLCD_CTRL_RW      DIO_PIN1
#define GLCD_CTRL_RS      DIO_PIN2
#define GLCD_CTRL_CS1     DIO_PIN4
#define GLCD_CTRL_CS2     DIO_PIN5
#define GLCD_CTRL_RST     DIO_PIN6

#define GLCD_MAX_LINES							(8U)
#define GLCD_MAX_LINE_PIXEL_WIDTH				(128U)
#define GLCD_MAX_PAGE_PIXEL_WIDTH				(64U)

#define GLCD_USE_TIMERS_FOR_DELAY				(0U)	



#endif