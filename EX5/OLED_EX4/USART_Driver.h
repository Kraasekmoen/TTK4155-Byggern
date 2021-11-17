/*
 * USART_Driver.h
 *
 * Created: 08.10.2021 16:47:56
 *  Author: sindrbov
 */ 
#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);
void USART_init(unsigned int ubrr);


