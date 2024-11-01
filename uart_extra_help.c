/*
*
*   uart_extra_help.c
* Description: This is file is meant for those that would like a little
*              extra help with formatting their code, and followig the Datasheet.
*/

#include "uart_extra_help.h"
#include "timer.h"
#define REPLACE_ME 0x00
volatile int receive_event;
volatile char received_char;

void uart_init(int baud)
{
    SYSCTL_RCGCGPIO_R  |= 0x00000002;      // enable clock GPIOB (page 340)
    SYSCTL_RCGCUART_R  |= 0x00000002;      // enable clock UART1 (page 344)
    GPIO_PORTB_AFSEL_R |= 0x00000003;      // sets PB0 and PB1 as peripherals (page 671)
    GPIO_PORTB_PCTL_R  &= 0xFFFFFF00;      // pmc0 and pmc1       (page 688)  also refer to page 650
    GPIO_PORTB_PCTL_R  |= 0x00000011;
    GPIO_PORTB_DEN_R   |= 0x00000003;      // enables pb0 and pb1
    GPIO_PORTB_DIR_R   &= 0xFFFFFFFE;      // sets pb0 as input(template wanted output), pb1 as output(template says input)
    GPIO_PORTB_DIR_R   |= 0x00000002;

    //compute baud values [UART clock= 16 MHz] 
    double brd;
    double fbrd;
    int    ibrd;
    int ClkDiv;
    int DivFrac;

    if(UART1_CTL_R & 0x00000020)
    {
        ClkDiv = 8;
    }
    else
    {
        ClkDiv = 16;
    }

    brd = 16000000.0/(ClkDiv * baud);
    fbrd = brd - (int)brd ; // page 903
    ibrd = (int)brd;
    DivFrac = (int) ((fbrd * 64) + 0.5);

    UART1_CTL_R &= 0xFFFFFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R = ibrd;      // write integer portion of BRD to IBRD
    UART1_FBRD_R = DivFrac;      // write fractional portion of BRD to FBRD
    UART1_LCRH_R |= UART_LCRH_WLEN_8;
    UART1_CC_R   &= 0xFFFFFFF0;      // use system clock as clock source (page 939)
    UART1_CTL_R |= 0x00000001;      // enable UART1

}

void uart_sendChar(char data)
{
    while(UART1_FR_R & 0x20) {}
    UART1_DR_R = data;
}

char uart_receive(void)
{
 int i;
 int value;
 while (UART1_FR_R & 0x10) {}
 i = UART1_DR_R;
 if (i & 0xF00) {
     GPIO_PORTE_DATA_R = 0xF;
 }
 else {
     value = (int) UART1_DR_R & 0xFF;
 }
 return value;
}

void uart_sendStr(const char *data)
{
    while (*data != '\0') {
        uart_sendChar(*data);
        data++;
        if(*data == '\r'){
            uart_sendChar('\n');
        }
    }
}

// _PART3


void uart_interrupt_init()
{
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0x00000010; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0x0000040; //enable uart1 interrupts - page 104

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number

}

void uart_interrupt_handler()
{

// STEP1: Check the Masked Interrup Status

//STEP2:  Copy the data 
    receive_event = 1;
    received_char = uart_receive();
//STEP3:  Clear the interrup   
    UART1_ICR_R |= 0x00000040;
}
