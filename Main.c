/* 
 * File:   Main.c
 * Author: 1dTracker
 *
 * Created on 13 July 2017, 15:34
 */


/* SDI = RX    SDO TX*/


#define UART_BUFFER_SIZE 256
#define SETUP_BUF_LENGTH 20
#define ANAL_BUF_LENGTH  15
#define M_SEC FCY*0.01 //0.001

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

volatile unsigned int UART_write = 0;
volatile unsigned int UART_read = 0;
volatile unsigned char UARTbuffer[UART_BUFFER_SIZE];


void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void);

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void);


//void UART1_send(char *UARTdata, int n){
//
//    int i=0;
//
//    while(i<n){
//
//        while(U1STAbits.UTXBF == 1); /* hold while buffer is full */
//        U1TXREG = UARTdata[i];
//        printf("%c",U1TXREG);
//        i++;
//
//    }
//}




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
    float R;
	float Vo;
	float T;
    float Ts;
    
   
    //Angle
    angle = ((((305*PI)/180)/1024) * data[2]); // 305 graus
  
	//Temperature
	Vo = 5.00*((data[1])/1024);
	R =  10000.00/((5.00 - Vo)/(Vo));
	T = (3435.00*298.15)/(298.15*log(R/10000.00)+3435.00);
 
    temperature = (T - 273.15);  
    
    // Voltage
	
	// Caso Tensão máxima seja 38V
    //Ts = 5.00*((data[0])/1024);
    //tension = (38/5.00)*Ts;
	
	//Caso Corrente máxima seja os 0.86A, passando numa resistência de 10 Ohm
	Ts = 5.00*((data[0])/1024);
	tension = (8.6/5.00)*Ts;


 
    //Current
    current=tension/10.0;
}


// tensão, corrente, temperatura, angulo
int main (void)
{     
    init_UART1();
    init_Timer3(3,58594);
    init_ADC();   
       
    ADC_Analogic(0);
    ADC_Analogic(1);
    ADC_Analogic(2);
 
    
//    
//    
//    
    while(1)        
    {  
        if (a==1)
      {   
            
      // UART1_send("TESTE",strlen("TESTE));
        
        data[0]=Read_ADC(0);         
        data[1]=Read_ADC(1);
        data[2]=Read_ADC(2);         


        Convert();
            
       printf(",%f, %f, %f, %f, \n", tension, current, temperature, angle);

       a=0;
    
         }       
    }
       
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {

  
    a=1;
    IFS0bits.T3IF = 0; //Clears interrupt flag 
    return;
    }

/**********************************************************************

 * Assign UART1 interruption

 **********************************************************************/

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void){

    

    UARTbuffer[UART_write] = U1RXREG;
    UART_write = (UART_write+1)%UART_BUFFER_SIZE; 

    IFS0bits.U1RXIF = 0;        /* clear the Rx Interrupt Flag */

    return;

}