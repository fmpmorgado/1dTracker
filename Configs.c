#include "Configs.h"
#include "Main.h"
#include <uart.h>
#include <libpic30.h>
#include <p30F4011.h>

/*
 
 ***** UART CONFIGURATION *****
 
 */

void init_UART2(void) {
//    
//   unsigned int UMODEvalue, U2STAvalue;  //auxiliary UART config variables (parâmetros de configuração da porta série)
//   
//   /*Serial port config*/ 
//    
//    UMODEvalue = UART_EN & UART_IDLE_CON & UART_NO_PAR_8BIT;
//    U2STAvalue = UART_INT_TX & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_RX_TX;
//    OpenUART2 (UMODEvalue, U2STAvalue, 191); 
//    U2STAbits.URXISEL = 1;
//    _U2RXIE = 1;
//    U2MODEbits.LPBACK = 0;
//    __C30_UART = 2;
// 
//  
 }
void init_UART1(void) {
    
   unsigned int UMODEvalue, U1STAvalue;  //auxiliary UART config variables (parâmetros de configuração da porta série)
   
   /*Serial port config*/ 
    
    UMODEvalue = UART_EN & UART_IDLE_CON & UART_NO_PAR_8BIT;
    U1STAvalue = UART_INT_TX & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_RX_TX;
    OpenUART1 (UMODEvalue, U1STAvalue, 191); 
    U1STAbits.URXISEL = 1;
    _U1RXIE = 1;
    U1MODEbits.LPBACK = 0;
    __C30_UART = 1;
 
  
 }
//
//
//void init_UART1(void){
//
//    /* configures LED to denote USD/UART activity*/
//    int BRGVAL = ((FCY/16)*(<1/.9600.))-1;
//    
//    
//     TRISFbits.TRISF2 = 1;
//     TRISFbits.TRISF3 = 0;
//    LATFbits.LATF2 = 0;
//
//    U1BRG = BRGVAL;
//    U1MODEbits.PDSEL = 0;        /* 8-bit data, no parity */
//    U1MODEbits.STSEL = 0;        /* 1 Stop-bit */
//    U1MODEbits.USIDL = 0;        /* Continue operation in idle mode */
//    U1MODEbits.ALTIO = 0;        /* Use U1TX and U1RX only*/
//    IEC0bits.U1TXIE = 0;        /* No interrupt when transmitting */
//    U1STAbits.UTXISEL = 0;        /* Interrupt when a character is transferred to the Transmit Shift register */
//    IEC0bits.U1RXIE = 1;        /* Enable UART Receive interrupt */
//    IPC2bits.U1RXIP = 5;        /* UART1 Receiver Interrupt Priority is 5 */
//    U1STAbits.URXISEL = 0;        /* Interrupt flag bit is set when a character is received */
//    IFS0bits.U1RXIF = 0;        /* clear the Rx Interrupt Flag */
//    U1MODEbits.UARTEN = 1;        /* Enanble UART */
//    U1STAbits.UTXEN = 1;        /* UART transmitter enabled, UxTX pin controlled by UART (if UARTEN = 1) */
//
//
//    return;
//}




/*
 
***** ADC CONFIGURATION *****
 
 */


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
    
	ADCON1bits.ADON = 1;	//Enable ADC
	__delay32(FCY);	//ADC off-on stabilization delay
	
 
	return;
}


void ADC_Analogic(int analogic) {

    TRISB |= (1 << analogic);            // ADC_CHANNEL defined as input
    ADPCFG &= ~(1 << analogic);          // ADC_CHANNEL defined as analog
}



/*
 
 ***** TIMERS CONFIGURATION *****
 
 */

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
