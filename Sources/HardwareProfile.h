/* 
 * File:   HardwareProfile.h
 * Author: Francois-Xavier MAURILLE
 * Version : v1.0
 * Created on July 11, 2012, 12:47 PM
 */

#ifndef HARDWARE_PROFILE_H
#define	HARDWARE_PROFILE_H

#include <plib.h>
#include "Peripherals/p_adc.h"
#include "Peripherals/p_gpio.h"
#include "Peripherals/p_uart.h"
#include "Peripherals/p_usb.h"
#include "Peripherals/p_i2s.h"
#include "Peripherals/p_lcd.h"

#define SYS_FREQ        (40000000L)

// USB Stack / MDD needed
#define USE_USB_INTERFACE               // USB host MSD library

#define GetSystemClock()            40000000UL
#define GetPeripheralClock()        40000000UL  // Will be divided down
#define GetInstructionClock()       GetSystemClock()


void hardware_config(void);

#endif	/* HARDWARE_PROFILE_H */

