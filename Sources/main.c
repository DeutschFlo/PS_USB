/* 
 * File:   main.c
 * Author: Francois-Xavier MAURILLE
 * v1.0
 * Created on July 11, 2012, 9:16 AM
 */

#include "main.h"

extern pad padTab[MAX_INPUT];
extern volatile pad * padEnabled[MAX_INPUT];
extern volatile UINT8 nInput;

volatile UINT8 state = MODE_START;

extern volatile BOOL new_tour;


/*
 * Main
 *
 * State machin
 */
int main(int argc, char** argv) {

    padTab[0].enable = 1;
    padTab[1].enable = 1;
    padTab[2].enable = 1;
    padTab[3].enable = 1;
	///loooool

    /* Wait before init.
     * Due to the display
     */
    UINT32 count;
    for(count=0;count<SYS_FREQ/32;count++) asm("nop");

    hardware_config();
    INTEnableSystemMultiVectoredInt();

    while(1){

        switch(state)
        {
            case MODE_START :       mode_display(MODE_START);
                                    state = MODE_DEFAULT;
                                    // Enable EXTINT
                                    BP_R_INT = 1;
                                    BP_L_INT = 1;
                                    break;

            case MODE_SOUNDPAD :    mode_display(MODE_SOUNDPAD);
                                    pad_refresh(0);
                                    USBTasks();
                                    if(nInput == 0) // See if there are some pads enabled
                                    {
                                        lcd_goto(LCD_L4,1);
                                        lcd_puts("<!>No PAD enabled...");
                                        while(state == MODE_SOUNDPAD);
                                    }
                                    else if(!USBHostMSDSCSIMediaDetect()) // See if a USB stick is connected
                                    {
                                        lcd_goto(LCD_L3,1);
                                        lcd_puts("<!>No USB device... ");
                                        while((state == MODE_SOUNDPAD) && !USBHostMSDSCSIMediaDetect()) USBTasks();
                                    }
                                    else
                                    {
                                        deviceAttached = TRUE;
                                        if(!FSInit()) // See if the USB stick is readable
                                        {
                                            lcd_goto(LCD_L3,1);
                                            lcd_puts("<!>USB no readable..");
                                            while((state == MODE_SOUNDPAD) && (deviceAttached == TRUE) && !FSInit()) USBTasks();
                                        }
                                        else if (FSchdir("\\SOUNDPAD")!=0) // Try to go into SOUNDPAD directory; Return 0 if success
                                        {
                                            lcd_goto(LCD_L3,1);
                                            lcd_puts("<!>SOUNDPAD error...");
                                            while((state == MODE_SOUNDPAD) && (deviceAttached == TRUE)) USBTasks(); // We could try in this loop but that could be too much for the USB stick
                                        }

                                        // Verify if BANKs / Samples aaare readable . . .

                                        else // Everything is OK
                                        {
                                            adc_start();
                                            //I2S_TX_INT = 1; // ~ ENABLE I2S INT TO SEND DATA TO DAC
                                            while((state == MODE_SOUNDPAD)&&(deviceAttached == TRUE))
                                            {
                                                USBTasks();

                                                // FOR Debug ///////////////////////////
                                                //lcd_goto(LCD_L3,1);
                                                //lcd_putn(padEnabled[3]->note,3);
                                                //lcd_puts("  ");
                                                //lcd_putn(padEnabled[3]->velocity & 0x7F,3);
                                                //lcd_puts("       ");
                                                /////////////////////////////////////////
                                                for(count=0;count<nInput;count++)
                                                {
                                                    // FOR Debug ///////////////////////
                                                    if(padEnabled[3]->state == 1)
                                                    {
                                                     init_read();
                                                     padEnabled[3]->state = 0;
                                                    }
                                                    ////////////////////////////////////
                                                }
                                                read_audio();
                                            }
                                        }
                                    }
                                    I2S_TX_INT = 0;
                                    adc_stop();
                                    break;

          case MODE_MIDI_MASTER : mode_display(MODE_MIDI_MASTER);
                                    pad_refresh(0);
                                    midi_set_note();
                                    if(nInput == 0)
                                    {
                                        lcd_goto(LCD_L4,1);
                                        lcd_puts("<!>No PAD enabled...");
                                        while(state == MODE_MIDI_MASTER);
                                    }
                                    else
                                    {
                                        int index;
                                        adc_start();
                                        while(state == MODE_MIDI_MASTER)
                                        {
                                            for(index=0;index<nInput;index++)
                                            {
                                                if(padEnabled[index]->state == 1)midi_send_note(index);
                                            }
                                        }
                                    }
                                    adc_stop();
                                    break;

            case MODE_MIDI_SLAVE :  mode_display(MODE_MIDI_SLAVE);
                                    pad_refresh(0);
                                    while(state == MODE_MIDI_SLAVE)
                                    {
                                        //TODO
                                    }
                                    break;

            case MODE_PAD :         mode_display(MODE_PAD);
                                    pad_refresh(1);
                                    adc_start();
                                    while(state == MODE_PAD)
                                    {
                                        mode_display(MODE_PAD);
                                        pad_toggle();
                                    }
                                    adc_stop();
                                    break;

            case MODE_SETTINGS1 :   mode_display(MODE_SETTINGS1);
                                    pad_refresh(1);
                                    adc_start();
                                    while(state == MODE_SETTINGS1)
                                    {
                                        mode_display(MODE_SETTINGS1);
                                        set_note();
                                        set_bank();
                                    }
                                    adc_stop();
                                    break;

            case MODE_SETTINGS2 :   mode_display(MODE_SETTINGS2);
                                    pad_refresh(1);
                                    adc_start();
                                    while(state == MODE_SETTINGS2)
                                    {
                                        mode_display(MODE_SETTINGS2);
                                        set_threshold();
                                        set_sensitivity();
                                    }
                                    adc_stop();
                                    break;
        }

    }
    return (EXIT_SUCCESS);
}

