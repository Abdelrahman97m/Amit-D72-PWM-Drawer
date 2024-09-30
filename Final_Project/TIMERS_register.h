
#ifndef TIMERS_REGISTER_H_
#define TIMERS_REGISTER_H_
 
#define TCCR0        (*(( volatile u8 *)0X53))
#define TCNT0        (*(( volatile u8 *)0X52))
#define OCR0         (*(( volatile u8 *)0X5C))
#define TIMSK        (*(( volatile u8 *)0X59))
#define TIFR         (*(( volatile u8 *)0X58))


#endif