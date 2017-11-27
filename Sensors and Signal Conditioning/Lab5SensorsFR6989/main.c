/*
 * Author: Sean McGuire
 * Last Edited: 4:43
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430FR69x Demo - LCD_C, 4-Mux example, display "430" string on the LCD and
//                     blinks the string every 1Hz.
//
//  Description: This examples configures the LCD in 4-Mux mode and displays
//  "430" on a TI LCD and blinks it at 1Hz interval. This example was tested
//  on MSP-EXP430FR6989 Launchpad. This example would need to be modified based
//  on the final LCD and GPIO routing where lcd_num[] would need to be modified.
//  The internal voltage is sourced to v2 through v4 and v5 is connected to
//  ground. Charge pump is enabled.
//
//  It uses LCD pin L0~L21 and L26~L43 as segment pins.
//  f(LCD) = 32768Hz/((1+1)*2^4) = 1024Hz, ACLK = ~32768Hz,
//  MCLK = SMCLK = default DCODIV ~1MHz.
//
//      MSP430FR6989 / MSP-EXP430FR6989 Launchpad
//              -----------------
//          /|\|                 |
//           | |              XIN|--
//  GND      --|RST              |  32768Hz
//   |         |             XOUT|--
//   |         |                 |
//   |         |             COM3|----------------|
//   |         |             COM2|---------------||
//   |--4.7uF -|LCDCAP       COM1|--------------|||
//             |             COM0|-------------||||
//             |                 |    -------------
//             |           Sx~Sxx|---| 1 2 3 4 5 6 |
//             |                 |    -------------
//             |                 |       TI LCD
//                                 (See MSP-EXP430FR6989 Schematic)
//
//   William Goh
//   Texas Instruments Inc.
//   May 2015
//   Built with IAR Embedded Workbench V6.30 & Code Composer Studio V6.1
//*****************************************************************************
#include "msp430.h"
#include "math.h"
float value = 0;


int main(void)
{
    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;


    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC;               // Clock synchronization enabled

    LCDCMEMCTL = LCDCLRM;                   // Clear LCD memory



    // Blink all segments, BLK_FREQ = 1Hz
    //LCDCBLKCTL |= LCDBLKPRE1 | LCDBLKPRE2 | LCDBLKMOD_2;

    showChar('A',1);
    //Turn LCD on
    LCDCCTL0 |= LCDON;

    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    PJSEL0 = BIT4 | BIT5;                   // For LFXT
    P2SEL0 |= BIT0 | BIT1;                    // USCI_A0 UART operation
    P2SEL1 &= ~(BIT0 | BIT1);
    P1OUT &= ~BIT0;                           // Clear LED to start
    P1DIR |= BIT0;                            // P1.0 output
    P9SEL1 |= BIT4;                           // Configure P1.1 for ADC
    P9SEL0 |= BIT4;

    //Configure UART
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 6;                              // 1MHz 9600 (see User's Guide)
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTLW = UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,                                                  // over sampling
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt


    // Configure ADC12
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;      // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                   // Use sampling timer
    ADC12CTL2 |= ADC12RES_2;                // 12-bit conversion results
    ADC12MCTL0 |= ADC12INCH_12;              // A1 ADC input select; Vref=AVCC
    ADC12IER0 |= ADC12IE0;                  // Enable ADC conv complete interrupt

    while (1)
    {
            __delay_cycles(5000);
            ADC12CTL0 |= ADC12ENC | ADC12SC;    // Start sampling/conversion

            __bis_SR_register(LPM0_bits | GIE); // LPM0, ADC12_ISR will force exit
            __no_operation();                   // For debugger
    }

    __bis_SR_register(LPM3_bits | GIE);
    __no_operation();

    return 0;
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
    {
        case ADC12IV_NONE:        break;    // Vector  0:  No interrupt
        case ADC12IV_ADC12OVIFG:  break;    // Vector  2:  ADC12MEMx Overflow
        case ADC12IV_ADC12TOVIFG: break;    // Vector  4:  Conversion time overflow
        case ADC12IV_ADC12HIIFG:  break;    // Vector  6:  ADC12BHI
        case ADC12IV_ADC12LOIFG:  break;    // Vector  8:  ADC12BLO
        case ADC12IV_ADC12INIFG:  break;    // Vector 10:  ADC12BIN
        case ADC12IV_ADC12IFG0:             // Vector 12:  ADC12MEM0 Interrupt

                value = ADC12MEM0;
                //Convert to voltage
                char c[4];
                value = value*0.0006347;
                unsigned int j;
                unsigned int i;
                unsigned int tmp;
                float t;
                for(j = 0; j < 4; j ++){	//seperating each digit of value
                for(i = 0; i <= 9; i ++){	//Sends proper value to Transmiswsion buffer to be displayed
                    if(j == 0){
                        t = value;
                        tmp = t;
                    }else if(j == 1){
                        t = value*(10);
                        tmp = t;
                    }else if(j == 2){
                        t = value*(100);
                        tmp = t;
                    }else if(j == 3){
                        t = value*(1000);
                        tmp = t;
                    }
                    t = value*pow(10,j);
                    tmp = t;
                    if(tmp == 1){
                        showChar('1',j+1);
                        UCA0TXBUF = 1;
                        break;
                    }else if(tmp == 2){
                        showChar('2',j+1);
                        UCA0TXBUF = 2;
                        break;
                    }else if(tmp == 3){
                        showChar('3',j+1);
                        UCA0TXBUF = 3;
                        break;
                    }else if(tmp == 4){
                        showChar('4',j+1);
                        UCA0TXBUF = 4;
                        break;
                    }else if(tmp == 5){
                        showChar('5',j+1);
                        UCA0TXBUF = 5;
                        break;
                    }else if(tmp == 6){
                        showChar('6',j+1);
                        UCA0TXBUF = 6;
                        break;
                    }else if(tmp == 7){
                        showChar('7',j+1);
                        UCA0TXBUF = 7;
                        break;
                    }else if(tmp == 8){
                        showChar('8',j+1);
                        UCA0TXBUF = 8;
                        break;
                    }else if(tmp == 9){
                        showChar('9',j+1);
                        UCA0TXBUF = 9;
                        break;
                    }else{
                        showChar('0',j+1);
                        UCA0TXBUF = 0;
                        break;
                    }
                }
                }
                // Exit from LPM0 and continue executing main
                __bic_SR_register_on_exit(LPM0_bits);
            break;
        case ADC12IV_ADC12IFG1:   break;    // Vector 14:  ADC12MEM1
        case ADC12IV_ADC12IFG2:   break;    // Vector 16:  ADC12MEM2
        case ADC12IV_ADC12IFG3:   break;    // Vector 18:  ADC12MEM3
        case ADC12IV_ADC12IFG4:   break;    // Vector 20:  ADC12MEM4
        case ADC12IV_ADC12IFG5:   break;    // Vector 22:  ADC12MEM5
        case ADC12IV_ADC12IFG6:   break;    // Vector 24:  ADC12MEM6
        case ADC12IV_ADC12IFG7:   break;    // Vector 26:  ADC12MEM7
        case ADC12IV_ADC12IFG8:   break;    // Vector 28:  ADC12MEM8
        case ADC12IV_ADC12IFG9:   break;    // Vector 30:  ADC12MEM9
        case ADC12IV_ADC12IFG10:  break;    // Vector 32:  ADC12MEM10
        case ADC12IV_ADC12IFG11:  break;    // Vector 34:  ADC12MEM11
        case ADC12IV_ADC12IFG12:  break;    // Vector 36:  ADC12MEM12
        case ADC12IV_ADC12IFG13:  break;    // Vector 38:  ADC12MEM13
        case ADC12IV_ADC12IFG14:  break;    // Vector 40:  ADC12MEM14
        case ADC12IV_ADC12IFG15:  break;    // Vector 42:  ADC12MEM15
        case ADC12IV_ADC12IFG16:  break;    // Vector 44:  ADC12MEM16
        case ADC12IV_ADC12IFG17:  break;    // Vector 46:  ADC12MEM17
        case ADC12IV_ADC12IFG18:  break;    // Vector 48:  ADC12MEM18
        case ADC12IV_ADC12IFG19:  break;    // Vector 50:  ADC12MEM19
        case ADC12IV_ADC12IFG20:  break;    // Vector 52:  ADC12MEM20
        case ADC12IV_ADC12IFG21:  break;    // Vector 54:  ADC12MEM21
        case ADC12IV_ADC12IFG22:  break;    // Vector 56:  ADC12MEM22
        case ADC12IV_ADC12IFG23:  break;    // Vector 58:  ADC12MEM23
        case ADC12IV_ADC12IFG24:  break;    // Vector 60:  ADC12MEM24
        case ADC12IV_ADC12IFG25:  break;    // Vector 62:  ADC12MEM25
        case ADC12IV_ADC12IFG26:  break;    // Vector 64:  ADC12MEM26
        case ADC12IV_ADC12IFG27:  break;    // Vector 66:  ADC12MEM27
        case ADC12IV_ADC12IFG28:  break;    // Vector 68:  ADC12MEM28
        case ADC12IV_ADC12IFG29:  break;    // Vector 70:  ADC12MEM29
        case ADC12IV_ADC12IFG30:  break;    // Vector 72:  ADC12MEM30
        case ADC12IV_ADC12IFG31:  break;    // Vector 74:  ADC12MEM31
        case ADC12IV_ADC12RDYIFG: break;    // Vector 76:  ADC12RDY
        default: break;
    }
}
