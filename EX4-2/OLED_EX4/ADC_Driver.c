/*
 * ADC_Driver.c
 *
 * Created: 08.10.2021 16:55:52
 *  Author: sindrbov
 */ 

#include <ADC_Driver.h>

uint8_t * adc_read()
{
	volatile char *adc = (char *) 0x1400;
	uint8_t command = 3;
	adc[0] = command;

	//_delay_us(200);
	for(int j=0; j<10; j++)
	{
		for(int k=0; k<6000; k++);
	}
	
	static uint8_t value[4];
	value[0] = adc[0];
	value[1] = adc[0];
	value[2] = adc[0];
	value[3] = adc[0];
	return value;
}

int8_t adc_formatter(uint8_t reading, uint8_t offset){		// Takes reading from ADC [00-FF] and returns number between -128 and 127
	int8_t reading_conv;
	if (reading <= offset) {
		reading_conv = -(128 - ((reading * 128) / offset));
		} else {
		reading_conv = reading - offset;
		reading_conv = (reading_conv * 127) / (255-offset);
	}
	
	return reading_conv;
}

uint8_t array_average(uint8_t *array, uint8_t size){		// Broken; doesnt work for some reason
	int sum, loop;
	uint8_t avg;
	sum = avg = 0;
	
	for(loop = 0; loop < size; loop++) {
		sum = sum + array[loop];
	}
	
	avg = sum / loop;
	return avg;
}

uint8_t * adc_joystick_calibrate(uint8_t samples){		// Takes a number of samples from both joysticks and returns pointer to array of averages [Doesnt work]
	uint8_t *sampled_value;
	uint8_t joy1_samples[samples];
	uint8_t joy2_samples[samples];
	uint8_t joy_origins[2];
	
	for(int i=0; i<samples; i++){
		sampled_value = adc_read();
		printf("Sample from Joy1 = %d \n", sampled_value[0]);
		joy1_samples[i] = sampled_value[0];
		joy2_samples[i] = sampled_value[1];
	}
	
	joy_origins[0] = array_average(joy1_samples, samples);
	joy_origins[1] = array_average(joy2_samples, samples);
	
	return joy_origins;
}


void pos_read (void)
// uso variabili globali
{
	// calculate the position of the joystick
}