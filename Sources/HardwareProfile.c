/*
 * File:   HardwareProfile.h
 * Author: Francois-Xavier MAURILLE
 * Version : v1.0
 * Created on July 11, 2012, 12:47 PM
 */

#include "HardwareProfile.h"

/*
 * Function hardware_config()
 *
 * This function configure oscillator and call
 * config function for each module
 */
void hardware_config(void){

    // Config Osc. Bits
    #pragma config POSCMOD = HS        // Primary Oscillator : Ext
    #pragma config FNOSC = PRIPLL       // Ext Osc. 8MHz
    #pragma config FPLLIDIV = DIV_2     // Divide before PLL, 4MHz
    #pragma config FPLLMUL = MUL_20     // PLL Multiply, 80MHz
    #pragma config FPLLODIV = DIV_2     // Divide after PLL, 40MHz
    #pragma config FPBDIV = DIV_1       // Divisor for PBCLK -> 40MHz
    
    // USB Clk config
    #pragma config UPLLEN   = ON            // USB PLL Enabled
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider -> USB CLOCK 48MHz

    // Config Watchdog/prog/debug/...
    #pragma config FWDTEN = OFF         // Watchdog Timer Disabled
    #pragma config ICESEL = ICS_PGx1    // For Microstick II, USE PGEC/D 1
    #pragma config JTAGEN = OFF         // Disable JTAG
    #pragma config FSOSCEN = OFF        // Disable Secondary Oscillator

    #pragma config CP       = OFF       // Code Protect
    //#pragma config BWP      = OFF     // Boot Flash Write Protect
    //#pragma config PWP      = OFF     // Program Flash Write Protect
    #pragma config IOL1WAY  = OFF

    /* IO config */
    gpio_config();
    adc_config();
    uart_config();
    i2s_config();
    lcd_config();
    usb_config();
}


