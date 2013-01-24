/*
 * File:   soundpad.h
 * Author: Francois-Xavier MAURILLE
 * v1.0
 * Created on August 20, 2012, 11:37 AM
 */

#ifndef SOUNDPAD_H
#define	SOUNDPAD_H

#include <plib.h>
#include "HardwareProfile.h"
#include "Peripherals/p_lcd.h"

#define DISPLAY_DELAY SYS_FREQ/16
#define MAX_INPUT 4
typedef struct {
    UINT8 enable;
    UINT8 note;
    UINT8 assigned;
    UINT8 currentVelocity;
    UINT8 velocity;
    UINT8 slope;
    UINT8 state;
}pad;

enum{MODE_START, MODE_SOUNDPAD, MODE_MIDI_MASTER, MODE_MIDI_SLAVE, MODE_PAD, MODE_SETTINGS1, MODE_SETTINGS2};

#define NB_MODE 7
#define MODE_DEFAULT MODE_SOUNDPAD
#define SEUIL_SWITCH 2

#define MIDI_NOTE_L 21 // Minimum note value
#define MIDI_NOTE_H 107 // Maximum note value

#define MIDI_BANK_L 0 // Minimum bank value (corresponding to the bank 1 for MIDI)
#define MIDI_BANK_H 14 // Maximum bank value (corresponding to 15th bank for MIDI)

#define VELO_THRESHOLD_L 0 // Minimum threshold value
#define VELO_THRESHOLD_H 10 // Maximum threshold value

#define PAD_SENSITIVITY_L 0 // Minimum sensitivity value
#define PAD_SENSITIVITY_H 3 // Maximum sensitivity value

void mode_display(int mode);
void set_velocity(void);
void set_note(void);

#endif	/* SOUNDPAD_H */