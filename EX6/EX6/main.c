/*
 * EX6.c
 *
 * Created: 15.11.2021 16:04:45
 * Author : sindrbov
 */ 


#include "sam.h"
#include "printf-stdarg.h"
#include "uart.h"
#include "can_controller.h"
#include "can_interrupt.h"

// User-defined thingies
//#define LED1 PIO_PA19
//#define LED2 PIO_PA20

#define CAN_BAUD_RATE 250000



void LED_init(){
	// 'Turn on the power' on Port A
	PMC->PMC_PCER0 = ID_PIOA;
	
	// Set Output Enable on pins
	PIOA->PIO_PER = PIO_PER_P19;
	PIOA->PIO_OER = PIO_OER_P19;
	PIOA->PIO_PER = PIO_PER_P20;
	PIOA->PIO_OER = PIO_OER_P20;
}



void feed_watchdog(){
	WDT->WDT_CR = WDT_CR_KEY_PASSWD;		// Password
	WDT->WDT_CR = WDT_CR_WDRSTT | WDT_CR_KEY_PASSWD;
}

/*
void CAN_init(){
	// ATSAM has 2 CAN controllers(?) - This is for enabling CAN0
	//	CANRX0 [PA1  | A]	CANTX0 [PA0  | A]
	//	CANRX1 [PB15 | A]	CANTX1 [PB14 | A]
	// CANs Peripheral Identifiers are PID43 and PID44 respectively
	
	// Enable CAN controller in PMC			40.6.2 'Turn on power in CAN'
	PMC->PMC_PCER1 = ID_CAN0;
	
	// Enable CAN controller interrupt line (AIC)
	//		There are 2 types of interrupts in this setting: Message object Interrupts and System Interrupts
	//		All sources are masked in CAN_IDR on startup and must be unmasked with CAN_IER. Status is found in CAN_IMR
	CAN0->CAN_IER = 0x1;
	CAN0->CAN_IER = 0x2:	// Enable Mailbox 0 + 1 
	
	
	// Disable CAN register write protection
	CAN0->CAN_WPMR = CAN_WPMR_WPKEY(0x43414E);	// Whoever made this can go fuck themselves
	
	// Configure a mailbox (there are 8 mailboxes available)
	//	A mailbox can be in Reception modes or Transmission modes!
	//		There are 2 modes for reception: Receive Mode; first message is stored in mailbox (in data registers), and Receive with overwrite Mode; last message is kept
	//			First, set Message ID [CAN_MIDx] and Acceptance Masks[CAN_MAMx]. Then set Receive mode by setting MOT field in CAN_MMRx
	//			When message is accepted by mailbox, MRDY flag is set in CAN_MSR. This triggers an interrupt.
	//		Message data is stored in the mailbox data registers (High and Low), until one asks for a new transfer command.
	//
	//		Transmit Mode set by configuring MOT field in CAN_MMRx. MRDY flag in CAN_MSR is automatically set until first message is sent.
	//			While MRDY is set, message can be prepared for sending by writing to CAN_MDx. Transfer Command is set by MTCR bit and message data length in CAN_MCRx.
	
	//CAN0->CAN_MAM
	
	
	
	// Set CAN_BR for timing settings
	CAN0->CAN_BR = 0x293112;						// SMP:0 BRP:41 SJW:3 PROPAG:1 PHS1:1 PHS2:2
	
	// Enable CAN controller
	CAN0->CAN_MR = CAN_MR_CANEN;
	
	// Enable CAN register write protection
	CAN0->CAN_WPMR = CAN_WPMR_WPKEY(0x43414E) | CAN_WPMR_WPEN;
}
*/

void LED_toggle_demo(){
	// ATSAM has several Parallel IO controllers, and dedicated set, clear and status registers for controlling pins (LAB Manual, Fig. 11)
	// SODR: Set Output Data Register		CODR: Clear Output Data Register		ODSR: Output Data Status Register
	
	
	PIOA->PIO_SODR = PIO_SODR_P19;
	PIOA->PIO_SODR = PIO_SODR_P20;
	
	for (int i = 0; i<1000; i++){
		for (int k = 0; k<10000; k++){}
	}
	
	PIOA->PIO_CODR = PIO_CODR_P19;
	PIOA->PIO_CODR = PIO_CODR_P20;
	
	for (int i = 0; i<1000; i++){
		for (int k = 0; k<10000; k++){}
	}	
}

void CAN_demo(){
	CAN_MESSAGE msg;
	
	for (int n=0; n<3; n++){
		printf("Checking mailbox %d\n\r",n);
		if (!can_receive(&msg, n)){
			can_print_message(&msg);
		}
		else{
			printf("No mail today...\n\r");
		}
	}
	printf("\n\r");
}

int main(void)
{
    SystemInit();
	WDT->WDT_MR = WDT_MR_WDDIS;						//Disable watchdog

	LED_init();
	configure_uart();								// UART printf init
	can_init_def_tx_rx_mb(0x293112);				// SMP:0 BRP:41 SJW:3 PROPAG:1 PHS1:1 PHS2:2

    while (1) 
    {
		for (int i = 0; i<1000; i++) { for (int j=0; j<30000; j++){} }
		
		LED_toggle_demo();
		CAN_demo();
    }
}

