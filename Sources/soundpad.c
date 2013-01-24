/*
 * File:   soundpad.c
 * Author: Francois-Xavier MAURILLE
 * v1.0
 * Created on August 20, 2012, 18:47 PM
 */

#include "soundpad.h"
/*
 *
 */
volatile pad padTab[MAX_INPUT];
volatile UINT8 nInput = 4;
volatile pad * padEnabled[MAX_INPUT];

volatile UINT8 bank = 0; // MIDI BANK 1
volatile UINT8 channel = 0x0;    // 4 LSB Message header, Channel 0 by default
volatile UINT8 note = 21; //between 21 - 107

volatile UINT8 velocityThreshold = 1;
volatile UINT8 padSensitivity = 3; // 3 is the most sensitive

extern UINT8 state;

// AUDIO PLAY
volatile UINT16 buffer_file_size=0;
volatile UINT16 buffer_audio_size=0;
volatile UINT16 buffer_file_index=0;
volatile UINT16 buffer_audio_index=0;
//volatile UINT8 buffer_file[0x2000];
volatile UINT16 buffer_audio[0x30FF];

volatile BOOL buffer_file_fflag = 0;
volatile BOOL file_eof = TRUE;
volatile BOOL new_tour = FALSE;

FILE* wavfile;


/*
 * Function mode_display(int mode)
 *
 * This function manage different displays according to the
 * current state
 */
void mode_display(int mode)
{
    int count;
    switch(mode)
    {
         case MODE_START :          lcd_goto(LCD_L1,2);
                                    lcd_puts("------------------");
                                    lcd_goto(LCD_L2,3);
                                    lcd_puts("PIC32 Sound Pad");
                                    lcd_goto(LCD_L3,2);
                                    lcd_puts("------------------");
                                    lcd_goto(LCD_L4,6);
                                    lcd_puts("Starting");
                                    for(count = 0;count<DISPLAY_DELAY;count++);
                                    lcd_puts(" .");
                                    for(count = 0;count<DISPLAY_DELAY;count++);
                                    lcd_puts(" .");
                                    for(count = 0;count<DISPLAY_DELAY;count++);
                                    lcd_puts(" .");
                                    for(count = 0;count<DISPLAY_DELAY;count++);
                                    break;

          case MODE_SOUNDPAD :      lcd_goto(LCD_L1,1);
                                    lcd_puts("<<   SOUND PAD    >>");
                                    lcd_goto(LCD_L2,1);
                                    lcd_puts("             BANK ");
                                    lcd_putn(bank+1,2);
                                    lcd_goto(LCD_L3,1);
                                    lcd_puts("                    ");
                                    lcd_goto(LCD_L4,1);
                                    lcd_puts((padTab[0].enable?" PAD1 ":"      "));
                                    lcd_puts((padTab[1].enable?"PAD2 ":"     "));
                                    lcd_puts((padTab[2].enable?"PAD3 ":"     "));
                                    lcd_puts((padTab[3].enable?"PAD4":"    "));
                                    break;

           case MODE_MIDI_SLAVE :   lcd_goto(LCD_L1,1);
                                    lcd_puts("<<   MIDI SLAVE   >>");
                                    lcd_goto(LCD_L2,1);
                                    lcd_puts("NOTE ");
                                    lcd_putn(note,3);
                                    lcd_puts("     BANK ");
                                    lcd_putn(bank+1,2);
                                    lcd_goto(LCD_L3,1);
                                    lcd_puts("                    ");
                                    lcd_goto(LCD_L4,1);
                                    lcd_puts("                    ");
                                    break;

           case MODE_MIDI_MASTER :  lcd_goto(LCD_L1,1);
                                    lcd_puts("<<   MIDI MASTER  >>");
                                    lcd_goto(LCD_L2,1);
                                    lcd_puts("NOTE ");
                                    lcd_putn(note,3);
                                    lcd_puts("     BANK ");
                                    lcd_putn(bank+1,2);
                                    lcd_goto(LCD_L3,1);
                                    lcd_puts("                    ");
                                    lcd_goto(LCD_L4,1);
                                    lcd_puts((padTab[0].enable?" PAD1 ":"      "));
                                    lcd_puts((padTab[1].enable?"PAD2 ":"     "));
                                    lcd_puts((padTab[2].enable?"PAD3 ":"     "));
                                    lcd_puts((padTab[3].enable?"PAD4":"    "));
                                    break;

            case MODE_PAD :         lcd_goto(LCD_L1,1);
                                    lcd_puts("<<      PADS      >>");
                                    lcd_goto(LCD_L2,1);
                                    lcd_puts("                    ");
                                    lcd_goto(LCD_L3,1);
                                    lcd_puts(" PAD1 PAD2 PAD3 PAD4");
                                    lcd_goto(LCD_L4,1);
                                    lcd_puts((padTab[0].enable?"  ON ":"  OFF"));
                                    lcd_puts((padTab[1].enable?"  ON ":"  OFF"));
                                    lcd_puts((padTab[2].enable?"  ON ":"  OFF"));
                                    lcd_puts((padTab[3].enable?"  ON ":"  OFF"));
                                    break;

            case MODE_SETTINGS1 :   lcd_goto(LCD_L1,1);
                                    lcd_puts("<<   SETTINGS 1   >>");
                                    lcd_goto(LCD_L2,1);
                                    lcd_puts("NOTE ");
                                    lcd_putn(note,3);
                                    lcd_puts(" |   BANK ");
                                    lcd_putn(bank+1,2);
                                    lcd_goto(LCD_L3,1);
                                    lcd_puts("         |          ");
                                    lcd_goto(LCD_L4,1);
                                    lcd_puts(" -    +  |   -    + ");
                                    break;

            case MODE_SETTINGS2 :   lcd_goto(LCD_L1,1);
                                    lcd_puts("<<   SETTINGS 2   >>");
                                    lcd_goto(LCD_L2,1);
                                    lcd_puts("SENS. ");
                                    lcd_putn(padSensitivity,1);
                                    lcd_puts("  |   THR. ");
                                    lcd_putn(velocityThreshold,2);
                                    lcd_goto(LCD_L3,1);
                                    lcd_puts("         |          ");
                                    lcd_goto(LCD_L4,1);
                                    lcd_puts(" -    +  |   -    + ");
                                    break;
    }

}


/*
 * Function set_velocity(void)
 *
 * This function detect when a stroke is done on a pad, and
 * manage pad.state values
 */
void set_velocity(void){
    UINT8 index;
    UINT8 buffer[nInput];

    ADC_SAMPLING = STOP;            // stop sampling
    ADC_FLAG = 0;                   // clear flag

    switch(nInput)
    {
        case 4 : buffer[3] = ADC1BUF3 >> (PAD_SENSITIVITY_H - padSensitivity);

        case 3 : buffer[2] = ADC1BUF2 >> (PAD_SENSITIVITY_H - padSensitivity);

        case 2 : buffer[1] = ADC1BUF1 >> (PAD_SENSITIVITY_H - padSensitivity);

        case 1 : buffer[0] = ADC1BUF0 >> (PAD_SENSITIVITY_H - padSensitivity);

    }

    for(index=0;index<nInput;index++)
    {
        if(buffer[index] > (0 + velocityThreshold)){
            if ((buffer [index] + velocityThreshold) < padEnabled[index]->currentVelocity){
                if(padEnabled[index]->slope==1)
                {
                    padEnabled[index]->state = 1;
                    padEnabled[index]->velocity = padEnabled[index]->currentVelocity;
                }
                else padEnabled[index]->currentVelocity = buffer[index];

                padEnabled[index]->slope = 0;
            }else if((buffer[index] - velocityThreshold) > padEnabled[index]->currentVelocity){
                padEnabled[index]->currentVelocity = buffer[index];
                padEnabled[index]->slope = 1;
            }
        }else padEnabled[index]->currentVelocity = 0;
    }
    ADC_SAMPLING = START;           // restart automatic sampling
}



/*
 * Function set_note(void)
 *
 * This function change the note offset
 */
void set_note(void){
    if((padTab[0].state == 1)&&(padTab[0].velocity > SEUIL_SWITCH))
    {
        padTab[0].state = 0;
        if(note > MIDI_NOTE_L)note --;
        else note = MIDI_NOTE_L;
    }
    if((padTab[1].state == 1)&&(padTab[1].velocity > SEUIL_SWITCH))
    {
        padTab[1].state = 0;
        if(note < MIDI_NOTE_H)note ++;
        else note = MIDI_NOTE_H;
    }
}


/*
 * Function set_bank(void)
 *
 * The function change the bank
 */
void set_bank(void){
    if((padTab[2].state == 1)&&(padTab[2].velocity > SEUIL_SWITCH))
    {
        padTab[2].state = 0;
        if(bank > MIDI_BANK_L)bank --;
        else bank = MIDI_BANK_L;
    }
    if((padTab[3].state == 1)&&(padTab[3].velocity > SEUIL_SWITCH))
    {
        padTab[3].state = 0;
        if(bank < MIDI_BANK_H)bank ++;
        else bank = MIDI_BANK_H;
    }
}


/*
 * Function set_sensitivity(void)
 *
 * This function adjust the sensitivity
 */
void set_sensitivity(void){
    if((padTab[0].state == 1)&&(padTab[0].velocity > SEUIL_SWITCH))
    {
        padTab[0].state = 0;
        if(padSensitivity > PAD_SENSITIVITY_L)padSensitivity --;
        else padSensitivity = PAD_SENSITIVITY_L;
    }
    if((padTab[1].state == 1)&&(padTab[1].velocity > SEUIL_SWITCH))
    {
        padTab[1].state = 0;
        if(padSensitivity < PAD_SENSITIVITY_H)padSensitivity ++;
        else padSensitivity = PAD_SENSITIVITY_H;
    }
}


/*
 * Function set_threshold(void)
 *
 * this function adjust the treshold
 */
void set_threshold(void){
    if((padTab[2].state == 1)&&(padTab[2].velocity > SEUIL_SWITCH))
    {
        padTab[2].state = 0;
        if(velocityThreshold > VELO_THRESHOLD_L)velocityThreshold --;
        else velocityThreshold = VELO_THRESHOLD_L;
    }
    if((padTab[3].state == 1)&&(padTab[3].velocity > SEUIL_SWITCH))
    {
        padTab[3].state = 0;
        if(velocityThreshold < VELO_THRESHOLD_H)velocityThreshold ++;
        else velocityThreshold = VELO_THRESHOLD_H;
    }
}


/*
 * Function init_read(void)
 *
 * This function initialize a file reading, called when a note is detected
 */
void init_read(void)
{
                                            // READ .PAX
                                            wavfile = FSfopen("01_32k.wav","r");
                                            buffer_audio_size = FSfread(buffer_audio, 2, 22, wavfile); // WAVE header
                                            buffer_audio_size = FSfread(buffer_audio, 2, 0x30FF, wavfile);

                                            // READ .WAV
                                            // OPEN FILE
                                            /*
                                            wavfile = FSfopen("01.wav","r");
                                            buffer_file_size = FSfread(buffer_file, 1, 44, wavfile);
                                            buffer_file_size = FSfread(buffer_file, 1, 4092, wavfile);
                                            buffer_audio[0] = buffer_audio[0] | (buffer_audio[1] << 8);
                                            for(buffer_file_index=2;buffer_file_index<buffer_file_size;buffer_file_index+=2) buffer_audio[buffer_file_index/2] = buffer_file[buffer_file_index] | (buffer_file[buffer_file_index+1] << 8);
                                            buffer_audio_size = buffer_file_size/2;

                                            buffer_file_size = FSfread(buffer_file, 1, 512, wavfile);
                                            buffer_audio[buffer_audio_size] = buffer_file[0] | (buffer_file[1] << 8);
                                            for(buffer_file_index=2;buffer_file_index<buffer_file_size;buffer_file_index+=2) buffer_audio[buffer_audio_size -1 + buffer_file_index/2] = buffer_file[buffer_file_index] | (buffer_file[buffer_file_index+1] << 8);
                                            buffer_audio_size += buffer_file_size/2;

                                            buffer_file_size = FSfread(buffer_file, 1, 512, wavfile);
                                            buffer_audio[buffer_audio_size] = buffer_file[0] | (buffer_file[1] << 8);
                                            for(buffer_file_index=2;buffer_file_index<buffer_file_size;buffer_file_index+=2) buffer_audio[buffer_audio_size -1 + buffer_file_index/2] = buffer_file[buffer_file_index] | (buffer_file[buffer_file_index+1] << 8);
                                            buffer_audio_size += buffer_file_size/2;
                                            */


                                            buffer_file_index=0;
                                            buffer_audio_index=0;
                                            buffer_file_fflag = TRUE;
                                            file_eof = FALSE;

                                            I2S_TX_INT = 1;
}


/*
 * Function read_audio(void)
 *
 * This function continue to read the sound file on the USB stick
 */
void read_audio(void)
{
        if((!file_eof)&&((buffer_file_index < buffer_audio_index)||((buffer_file_index > buffer_audio_index)&&new_tour)))
        {
            if(FSfeof(wavfile) == 0) // Return non-zero if EOF
            {
                // TEST 03 : file reading is slower than i2S transmitting thus that should work
                FSfread((buffer_audio), 2, 0x3000, wavfile);

                // TEST 02
                //FSfread((buffer_audio + buffer_file_index), 2, 512, wavfile);
                //buffer_file_index +=512;

                // TEST 01
                //int i = 0;
                //FSfread(buffer_file, 1, 1024, wavfile);
                //buffer_audio[buffer_file_index++] = buffer_file[0] | (buffer_file[1] << 8);
                //for(i=2;i<1024;i+=2) buffer_audio[buffer_file_index++] = buffer_file[i] | (buffer_file[i+1] << 8);

                /*if(buffer_file_index >= buffer_audio_size)
                {
                    buffer_file_index = 0;
                    new_tour = FALSE;
                }*/
            }else{
                file_eof = TRUE;
            }
        }
        else if((file_eof==TRUE) && (new_tour==TRUE)) buffer_audio_size = buffer_file_index-1;
}


/*
 * Function i2s_send_data()
 *
 * This function send data throw I2S, called by interrupt SFR
 */
void i2s_send_data(){
    I2S_TX_FLAG = 0;
    SPI2BUF = buffer_audio[buffer_audio_index++];
    if (buffer_audio_index >= buffer_audio_size)
    {
        if(file_eof) I2S_TX_INT = 0;
        else
        {
            buffer_audio_index = 0;
            new_tour = TRUE;
        }
    }
}


/* ISR for buttons
 *
 * Change state value
 */
void __ISR(BP_R_VECTOR, ipl6)bp_r_int(void)
{
    BP_R_FLAG = 0;
    if(state < NB_MODE-1) state++;
    else state = 1;
}

void __ISR(BP_L_VECTOR, ipl6)bp_l_int(void)
{
    BP_L_FLAG = 0;
    if(state > 1) state--;
    else state = NB_MODE-1;
}



/* ISR for ADC capture
 *
 * According to  adc_config(), ADC_INT  is SET every nInput sample/convert
 * sequence sample finished
 */
void __ISR(_ADC_VECTOR, ipl5)adc_int(void)
{
    set_velocity();
}


/* ISR for I2S buffer
 *
 * Put data from a tab to I2S buffer
 */
void __ISR(_SPI_2_VECTOR, ipl6)i2s_int(void)
{
    i2s_send_data();
}