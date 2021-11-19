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
void AG_sequencer_16();

int OC1A_COMP_INT;
//ISR(TIMER1_COMPA_vect);

#endif /* AUDIOGENERATOR_H_ */