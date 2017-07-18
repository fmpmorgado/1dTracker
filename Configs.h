#ifndef CONFIGS_H
#define	CONFIGS_H


void init_uart(void);
void init_ADC(void);
void ADC_Analogic(int analogic);
void init_Timer2(int pre_scale, int count);
void init_Timer3(int pre_scale, int count);
void PWM_Config(int duty_cycle);

#endif	/* CONFIGS_H */

