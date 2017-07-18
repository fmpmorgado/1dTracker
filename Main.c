/* 
 * File:   Main.c
 * Author: 1dTracker
 *
 * Created on 13 July 2017, 15:34
 */




#include <libpic30.h>
#include <p30F4011.h>
#include <math.h>
#include <limits.h>
#include <uart.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Main.h"
#include "Configs.h"


// Configuration settings
#pragma config FCKSMEN=CSW_FSCM_OFF
#pragma config FOS=PRI               //fonte é o cristal
#pragma config FPR=XT_PLL16          //oscilador a 16x cristal
#pragma config WDT=WDT_OFF           //watchdog timer off
#pragma config MCLRE=MCLR_EN         //turn MCLR pin ON and
#pragma config FPWRT=PWRT_OFF

int a=0;
double tension, current, temperature, angle;
double data[4];


void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void);


int Read_ADC (int analogic)

{
    int aux;
    ADCHSbits.CH0SA = analogic;
    ADCON1bits.SAMP = 1;
    while (ADCON1bits.SAMP);
    while (!ADCON1bits.DONE);
    aux = ADCBUF0;
    return aux;
}

void Convert()
{
    float R = 0;
	float Vo;
	float Vin = 5.00;
	float Ro = 10000;
	float To = 298.15;
	float B = 3435;
	float T;
    
   
    //angle
    angle = ((((305*PI)/180)/1024) * data[3]); // 305 graus
  
	//temperature
	Vo = Vin*(((float)data[2])/1024);
	R =  Ro/((Vin - Vo)/(Vo));
	T = (B*To)/(To*log(R/Ro)+B);
    
    temperature = (T - 273.15); 
}


// tensão, corrente, temperatura, angulo
int main (void)
{  
    char dados[30];
    
    init_uart();
    init_Timer2(2,2000); //prescale, count
    init_Timer3(3,58594);
    init_ADC();   
    
    PWM_Config(1000);
   
    ADC_Analogic(0);
    ADC_Analogic(1);
    ADC_Analogic(2);
    ADC_Analogic(3);
    
    
    while(1)        
    {
 
    
        if (a==1)
      {
          
       
        data[0]=Read_ADC(0);         
        data[1]=Read_ADC(1);
        data[2]=Read_ADC(2);         
        data[3]=Read_ADC(3);
        Convert(data[0],data[1],data[2],data[3]);
        
        
        sprintf(dados," %f, %f, %f, %f", tension, current, temperature, angle);
        printf("\n\r%d -> angulo= %f",data[3],angle);
        printf("\n\r%s",dados);
      
        
       a=0;
    
         }       
    }
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {

    a=1;
    IFS0bits.T3IF = 0; //Clears interrupt flag 
    return;
    }