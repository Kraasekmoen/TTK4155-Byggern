/*
 * CAN_Driver.h
 *
 * Created: 04.11.2021 14:13:23
 *  Author: sindrbov
 */ 


#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_

#include <mcp2515.h>

/*
What do we need...
- A CAN Message struct										
- An ability to generate message IDs					
- A way to handle interrupts from the CAN Controller	
+++
*/

typedef volatile struct {				// Does not handle extended identifiers
	uint8_t ID_high;
	uint8_t ID_low;
	
	uint8_t data_length;
	uint8_t data[8];
		
	} CANMSG;

void CAN_ctrl_init(mcp_mode md);
int CAN_check_tx_buffer_pending(uint8_t buf_num);
int CAN_transmit_message(CANMSG* msg);
CANMSG CAN_read_rx_buffer(uint8_t rx_buf);
CANMSG CAN_get_mail();
void CAN_print_message(CANMSG* msg);

void Exercise_6_Demo();

volatile uint8_t EXT_INT_FLAG;
ISR(INT1_vect);


#endif /* CAN_DRIVER_H_ */