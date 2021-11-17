/*
 * ADC_Driver.h
 *
 * Created: 08.10.2021 16:53:48
 *  Author: sindrbov
 */ 



#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_
	#include <avr/io.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <stdbool.h>
	#include <stdlib.h>
	#include <avr/interrupt.h>

	uint8_t * adc_read();

	int8_t adc_formatter(uint8_t reading, uint8_t offset);		// Takes reading from ADC [00-FF] and returns number between -128 and 127

	uint8_t array_average(uint8_t *array, uint8_t size);

	uint8_t * adc_joystick_autocalibrate(uint8_t samples);		// Takes a number of samples from both joysticks and returns pointer to array of averages

	void ADC_test_1(uint8_t *joy_origins);


#endif /* ADC_DRIVER_H_ */