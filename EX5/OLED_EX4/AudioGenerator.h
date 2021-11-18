/*
 * AudioGenerator.h
 *
 * Created: 17.11.2021 14:48:19
 *  Author: sindrbov
 */ 


#ifndef AUDIOGENERATOR_H_
#define AUDIOGENERATOR_H_

#include <avr/io.h>

typedef enum {
	A = 440;
	B = 494;
	C = 523;
	D = 587;
	E = 659;
	F = 698;
	G = 784;
} tone ;

void AG_init(int bpm);
void AG_osc_init();
void AG_timer_init();

void AG_set_freq(int freq);


#endif /* AUDIOGENERATOR_H_ */