/*
 * EX6.c
 *
 * Created: 15.11.2021 16:04:45
 * Author : sindrbov
 */ 


#include "sam.h"

// User-defined thingies
#define LED1 PIO_PA5
#define LED2 PIO_PA0


void LED_toggle_demo(){
	// ATSAM has several Parallel IO controllers, and dedicated set, clear and status registers for controlling pins (LAB Manual, Fig. 11)
	// SODR: Set Output Data Register		CODR: Clear Output Data Register		ODSR: Output Data Status Register
	
	
	PIOA->PIO_SODR = PIO_SODR_P5;	
	PIOA->PIO_SODR = PIO_SODR_P0;
	/*
	for (int i = 0; i<1000; i++){
		for (int k = 0; k<3000; k++){}
	}
	
	PIOA->PIO_CODR = PIO_CODR_P5;
	
	for (int i = 0; i<1000; i++){
		for (int k = 0; k<3000; k++){}
	}
	*/	
}

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
	// Set Output Enable on pin PA5
	PIOA->PIO_PER = PIO_PER_P5;
	PIOA->PIO_OER = PIO_OER_P5;
	//PIOA->PIO_ABSR = PIO_ABSR_P5;

    /* Replace with your application code */
    while (1) 
    {
		LED_toggle_demo();
    }
}
