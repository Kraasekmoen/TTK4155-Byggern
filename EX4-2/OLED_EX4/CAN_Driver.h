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

typedef struct {				// Does not handle extended identifiers
	uint8_t ID_high;
	uint8_t ID_low;
	
	uint8_t data_length;
	uint8_t data[8];
		
	} CANMSG;

void CAN_ctrl_init(mcp_mode md);
int CAN_check_buffer_pending_tx(uint8_t buf_num);
int CAN_transmit_message(CANMSG* msg);
CANMSG CAN_read_rx_buffer(uint8_t rx_buf);
void CAN_print_message(CANMSG* msg);


#endif /* CAN_DRIVER_H_ */