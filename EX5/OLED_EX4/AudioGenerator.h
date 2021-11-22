/*
 * AudioGenerator.h
 *
 * Created: 17.11.2021 14:48:19
 *  Author: sindrbov
 */ 


#ifndef AUDIOGENERATOR_H_
#define AUDIOGENERATOR_H_

#include <avr/io.h>

const int TONE_A;
const int TONE_As;
const int TONE_B;
const int TONE_C;
const int TONE_D;
const int TONE_E;
const int TONE_F;
const int TONE_G;

void AG_init();

void AG_set_freq(int freq);
void AG_enable_osc();
void AG_disable_osc();
void AG_majorizer(int freq, int durtn);
void AG_octavizer(int freq, int durtn);
void AG_set_chord(int *freqs, int num_tones, int durtn);
void AG_sequencer_16(int *notes, int legato, int step_length, int majordize);
void AG_adv_sequencer_16(int *notes, int *legato, int *majordize, int step_length);


int OC1A_COMP_INT;
//ISR(TIMER1_COMPA_vect);

#endif /* AUDIOGENERATOR_H_ */