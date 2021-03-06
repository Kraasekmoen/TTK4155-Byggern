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
	int interim;
	if (reading <= offset) {
		interim = -(128 - ((reading * 128) / offset));
		} 
	else {
		interim = ((reading - offset) * 127) / (255-offset);
	}
	
	return reading_conv = interim;
}

uint8_t array_average(uint8_t *array, uint8_t size){		// 
	int sum, loop;
	uint8_t avg;
	sum = avg = 0;
	
	printf("Avg, array-size %d\n", size);
	
	for(loop = 0; loop < size; loop++) {
		sum = sum + array[loop];
	}
	
	avg = sum / size;
	printf("Sum %d. Average %d. \n\n", sum, avg);
	return avg;
}

uint8_t * adc_joystick_autocalibrate(uint8_t samples){		// Takes a number of samples from both joysticks and returns pointer to array of averages
	uint8_t *sampled_value;
	uint8_t joy1_samples[samples];
	uint8_t joy2_samples[samples];
	static uint8_t joy_origins[2];
	
	for(int i=0; i<samples; i++){
		sampled_value = adc_read();
		printf("Joy1 sample %d \n", sampled_value[0]);
		printf("Joy2 sample %d \n", sampled_value[1]);
		joy1_samples[i] = sampled_value[0];
		joy2_samples[i] = sampled_value[1];
		
		// Delay
		for(int j=0; j<1000; j++)
		{
			for(int k=0; k<1000; k++);
		}
	}
	
	joy_origins[0] = array_average(joy1_samples, samples);
	joy_origins[1] = array_average(joy2_samples, samples);
	
	return joy_origins;
}

//		--		TEST CODE BELOW		--		//

void ADC_test_1(uint8_t *joy_origins){
	uint8_t *sampled_value;
	char str[] = "Sampled value ch";
	
	sampled_value = adc_read();
	printf("%s (Joy1) = %d \n", str, adc_formatter(sampled_value[0], joy_origins[0]));
	printf("%s 1 (Joy2) = %d \n",str, adc_formatter(sampled_value[1], joy_origins[1]));
	printf("%s 2 (SliL) = %d \n",str, (sampled_value[2]));
	printf("%s ch 3 (SliR) = %d \n",str, (sampled_value[3]));
	
	// Delay
	for(int j=0; j<100; j++)
	{
		for(int k=0; k<10000; k++);
	}
}

void ADC_test_2(uint8_t *joy_origins){
	uint8_t *sampled_value;
	
	sampled_value = adc_read();
	printf("\rJoy1: %d ", adc_formatter(sampled_value[0], joy_origins[0]));
	printf("Joy2: %d | ", adc_formatter(sampled_value[1], joy_origins[1]));
	printf("SliL: %d ", (sampled_value[2]));
	printf("SliR: %d\n", (sampled_value[3]));
	
	// Delay
	for(int j=0; j<10; j++)
	{
		for(int k=0; k<3000; k++);
	}
}