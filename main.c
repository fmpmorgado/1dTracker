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

#define FCY 29491200L // 30MPI / 4
#define FOSC (FCY*4)
#define PI 3.141592653

// Configuration settings
#pragma config FCKSMEN=CSW_FSCM_OFF
#pragma config FOS=PRI               //fonte é o cristal
#pragma config FPR=XT_PLL16          //oscilador a 16x cristal
#pragma config WDT=WDT_OFF           //watchdog timer off
#pragma config MCLRE=MCLR_EN         //turn MCLR pin ON and
#pragma config FPWRT=PWRT_OFF

unsigned int data[4] = {0,0,0,0};
int a=0;
float tension=3, current=5.2, temperature=0, angle=0;


void init_uart(void) {
    
   unsigned int UMODEvalue, U2STAvalue;  //auxiliary UART config variables (parâmetros de configuração da porta série)
   
   /*Serial port config*/ 
    
    UMODEvalue = UART_EN & UART_IDLE_CON & UART_NO_PAR_8BIT;
    U2STAvalue = UART_INT_TX & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_RX_TX;
    OpenUART2 (UMODEvalue, U2STAvalue, 15); 
    U2STAbits.URXISEL = 1;
    _U2RXIE = 1;
    U2MODEbits.LPBACK = 0;
    __C30_UART = 2;
 
    return;
 }


void init_ADC(void) {
    
    //ADCON1
    ADCON1bits.ADON = 0;
	ADCON1bits.ADSIDL = 0;	//Continue in iddle mode
	ADCON1bits.FORM = 0;	//Output format - integer
	ADCON1bits.SSRC = 7;	//Auto-convert
    ADCON1bits.SIMSAM = 0;   // Samples individually
	ADCON1bits.ASAM = 0;	//auto-start sample

	//ADCON2
	ADCON2bits.VCFG = 0;	//Reference - AVdd; AVss
	ADCON2bits.CSCNA = 0;	//Scan inputs
    ADCON2bits.CHPS = 0;    //Converts CH0
	ADCON2bits.SMPI = 0;	//interrupts at each 2 sample/convert sequence
	ADCON2bits.BUFM = 0;	//buffer 16-word buffer
	ADCON2bits.ALTS = 0;	//use mux A

	//ADCON3
	ADCON3bits.SAMC = 0b11111;	//Auto sample time = fastest possible with sequential sampling
	ADCON3bits.ADRC = 0;	//Conversion clock system
	ADCON3bits.ADCS = 0b111111;	//Conversion Clock select
	
	//INPUT
    ADCHSbits.CH0NA = 0;	// MUX A negative input in vref-
    ADCHSbits.CH0SA = 0;   // Channel 0 positive input is AN0      

	//Input Scan Select Register
	ADCSSL = 0; //não necessário
    
//    //Interrupt Configuration
//	IEC0bits.ADIE = 1;		//Enable ADC conversion complete interrupt
//	IFS0bits.ADIF = 0;		//Clear ADC conversion complete flag
//	IPC2bits.ADIP = 4;		//Priority for ADC interrupt

    TRISB = 0b000000000;
    ADPCFG = 0b111111111;
    
	ADCON1bits.ADON = 1;	//Enable ADC
	__delay32(FCY);	//ADC off-on stabilization delay
	
 
	return;
}


void init_Timer2(int pre_scale, int count)
{
    
    T2CONbits.TON = 0;      //Timer_2 is OFF
    TMR2 = 0;               //resets Timer_2
    PR2 = count;             //sets the maximum count for Timer_2
    T2CONbits.TCS = 0;      //choose FCY as clock source for Timer_2
    T2CONbits.TCKPS = pre_scale; //sets the Timer_2 pre-scaler to 64?
    IFS0bits.T2IF = 0;      //clears Timer_2 interrupt flag   
    
}


void init_Timer3(int pre_scale, int count)
{
    T3CON = 0;            // Clear Timer 3 configuration
    TMR3 = 0x0000; // Sets timer value to zero
    T3CONbits.TCKPS = pre_scale;  // Set timer 3 prescaler (0=1:1, 1=1:8, 2=1:64, 3=1:256)
    PR3 = count;          // Set Timer 3 period (max value is 65535)
    _T3IP = 1;            // Set Timer 3 interrupt priority
    _T3IF = 0;            // Clear Timer 3 interrupt flag
    _T3IE = 1;            // Enable Timer 3 interrupt
    
    
    T3CONbits.TON = 1;    // Turn on Timer 3        
    return;
}



void ADC_Analogic(int analogic) {

    TRISB |= (1 << analogic);            // ADC_CHANNEL defined as input
    ADPCFG &= ~(1 << analogic);          // ADC_CHANNEL defined as analog
}



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

void PWM_Config(int duty_cycle)

{
    
    OC2RS = duty_cycle;           //sets the initial duty_cycle;
    OC2CONbits.OCM =6;      //set OC2 mode to PWM
    OC2R = 0;               //Initial Delay (only for the first cycle)
    OC2CONbits.OCTSEL = 0;  //selects Timer_2 as the OC2 clock source
    T2CONbits.TON = 1;      //turns Timer_2 ON (starts PWM generation at OC2) 
    
    
}



//void __attribute__((interrupt, auto_psv, shadow)) _ADCInterrupt(void)
//{
////    char valores[];
////    char str[];
//    
//   
//    
//    data[0]= ADCBUF0;   
//    data[1]= ADCBUF1;
//    data[2]= ADCBUF2;
//    data[3]= ADCBUF3;
////    
////    char str=(data[0]);
////    valores = strcat(valores,str);
////    printf(" %s ",valores);
////        
//        
//        
//    
// //    printf ("\n\r 0->%d ,3-> %d, 4-> %d , 5-> %d", data[0],data[1],data[2], data[3]);
//    
//    __delay32(FCY);
//    IFS0bits.ADIF = 0; 
//
//}


void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {

    a=1;
    IFS0bits.T3IF = 0; //Clears interrupt flag 
    return;
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



void PDI ()
{
    
    
    
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
        Convert();
        
        
        sprintf(dados," %.2f, %.2f, %.2f, %.2f", tension, current, temperature, angle);
        // printf("\n\r%d -> angulo= %f",data[3],angle);
        printf("\n\r%s",dados);
      
        
       a=0;
    
         }       
    }
}
