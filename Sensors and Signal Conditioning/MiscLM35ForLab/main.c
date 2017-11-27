#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    P1SEL0 |= BIT6 | BIT7;                    // set UART pins as second function

    // Configure UART
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;

      // Baud Rate calculation
      // 8000000/(16*9600) = 52.083
      // Fractional portion = 0.083
      // User's Guide Table 17-4: UCBRSx = 0x49
      // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW = 0x4900 | UCOS16 | UCBRF_1;

    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    P1SEL0 |= BIT1;                             //Configure ADC 1 pin
    P1SEL1 |= BIT1;

    ADCCTL0 |= ADCON | ADCMSC;                                // ADCON
    ADCCTL1 |= ADCSHS_2 | ADCCONSEQ_2;                        // repeat single channel; TB1.1 trig sample start
    ADCCTL2 |= ADCRES;                                        // 10-bit conversion results
    ADCMCTL0 |= ADCINCH_1 | ADCSREF_3;                        // A1 ADC input select; Vref=Vref+V
    ADCIE |= ADCIE0;                                           //Enable ADC Interupt

    PMMCTL0_H = PMMPW_H;                                      // Unlock the PMM registers
    PMMCTL2 |= INTREFEN;                                      // Enable internal reference
    __delay_cycles(400);                                      // Delay for reference settling

    ADCCTL0 |= ADCENC;                                          //ADC Enable


	return 0;
}
// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
        case ADCIV_NONE:			//ignore flag
            break;
        case ADCIV_ADCOVIFG:		//ignore flag
            break;
        case ADCIV_ADCTOVIFG:		//ignore flag
            break;
        case ADCIV_ADCHIIFG:		//ignore flag
            break;
        case ADCIV_ADCLOIFG:		//ignore flag
            break;
        case ADCIV_ADCINIFG:		//ignore flag
            break;
        case ADCIV_ADCIFG:
            if (ADCMEM0 < 0x155)                             // ADCMEM = A0 < 0.5V?
                P1OUT &= ~BIT0;                              // Clear P1.0 LED off
            else
                P1OUT |= BIT0;                               // Set P1.0 LED on
            ADCIFG = 0;
            break;                                           // Clear CPUOFF bit from 0(SR)
        default:
            break;
    }
}
