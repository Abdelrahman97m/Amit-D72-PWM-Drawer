
#ifndef PWM_PRIVATE_H_
#define PWM_PRIVATE_H_



#if NUM_OPERATING_PWM_CHANNELS > 4
	#pragma GCC error "ATmega32A has only 3 timers with 4 PWM channels!! Fix NUM_OPERATING_PWM_CHANNELS."
#endif



#endif 