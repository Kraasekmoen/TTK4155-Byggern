/*
 * first trial.c
 *
 * Created: 31.08.2021 09:45:49
 * Author : thoam
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void InitPorts();

int main(void)
{
	unsigned int i=0, j=0;
	InitPorts();
	
    /* Replace with your application code */
    while (1) 
    {
		PORTB^=(1<<PB0);
		
		for(i=0;i<10;i++)
		{
			for(j=0;j<60000;j++);
		}
    }
}

void InitPorts()
{
	DDRB|=(1<<DDB0);
	PORTB|=(1<<PB0);	
	
}