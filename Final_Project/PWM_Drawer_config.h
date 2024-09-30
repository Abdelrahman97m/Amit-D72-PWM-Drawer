
#ifndef PWM_DRAWER_PCONFIG_H_
#define PWM_DRAWER_PCONFIG_H_

#define GENERATE_PWM_FROM_SAME_MCU			(1U)	

#if GENERATE_PWM_FROM_SAME_MCU == 0
	
	#define GLCD_MCU						(0U)	
#endif


#endif 