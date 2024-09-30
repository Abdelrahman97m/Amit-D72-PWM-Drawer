
#ifndef _BIT_MATH_H_ 
#define _BIT_MATH_H_
 
#define SET_BIT(REG,BIT_NUM)    REG|=(1<<BIT_NUM)
#define CLR_BIT(REG,BIT_NUM)    REG&=(~(1<<BIT_NUM))
#define TOG_BIT(REG,BIT_NUM)    REG^=(1<<BIT_NUM)
#define GET_BIT(REG,BIT_NUM)    ((REG>>BIT_NUM)&1)
 
#define ASSGIN_BIT(VAR,BIT_NUM,VALUE) (VAR = ((VAR & (~(1<<BIT_NUM))) | (VALUE << BIT_NUM)))
 
 
#endif