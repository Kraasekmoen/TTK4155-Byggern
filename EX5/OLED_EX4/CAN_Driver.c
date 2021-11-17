/*
 * CAN_Driver.c
 *
 * Created: 04.11.2021 14:13:04
 *  Author: sindrbov
 */ 
#include <CAN_Driver.h>

// 
int CAN_check_tx_buffer_pending(uint8_t buf_num){
	if (buf_num > 2) { 
		printf("CAN_Driver Error: Attempted to check invalid txbfr! \n"); 
		return 1; 
	}
	
	uint8_t tx_ctrl_reg = MCP_read_byte(MCP_TXB0CTRL + buf_num * 0x10);		// Extract control register of transmit buffer
	uint8_t pending_tx = tx_ctrl_reg &= MCP_TXREQ;							// Isolate the TXREQ-bit
	if (pending_tx == MCP_TXREQ){
		return 1;		
	}
	else {
		return 0;
	}
}

int CAN_transmit_message(CANMSG* msg){
	
	// Check which, if any, of transmit buffers are vacant. If none are available, the function returns False
	static uint8_t buffer_num = 0;
	for (uint8_t i = 0; i<3; i++){
		if (!CAN_check_tx_buffer_pending(i)){
			buffer_num = i;
			break;
		}
		if (CAN_check_tx_buffer_pending(i) && i > 1){
			return 0;
		}
	}
	//printf("Buffnum %d\n", buffer_num);
	
	// Compile message
	MCP_write_byte(MCP_TXB0SIDH + 0x10*buffer_num, msg->ID_high);										// Write message ID to buffer
	MCP_write_byte(MCP_TXB0SIDL + 0x10*buffer_num, msg->ID_low);
	MCP_write_byte(MCP_TXB0DLC + 0x10*buffer_num, msg->data_length);									// Write message data length to buffer
	MCP_write(MCP_TXB0D0 + 0x10*buffer_num,(uint8_t *) msg->data,(uint8_t) msg->data_length);			// Write message data to buffer
		
	// Test if message was written correctly
	CANMSG tst_msg;
	tst_msg.ID_high = MCP_read_byte(MCP_TXB0SIDH + 0x10*buffer_num);
	tst_msg.ID_low = MCP_read_byte(MCP_TXB0SIDL + 0x10*buffer_num);
	tst_msg.data_length = MCP_read_byte(MCP_TXB0DLC + 0x10*buffer_num);
	for (uint8_t i = 0; i<tst_msg.data_length; i++){
		tst_msg.data[i] = MCP_read_byte(MCP_TXB0D0 + 0x10*buffer_num + sizeof(uint8_t)*i);
	}
	
	//CAN_print_message(&tst_msg);
	
	if (tst_msg.ID_high != msg->ID_high || tst_msg.ID_low != msg->ID_low || tst_msg.data_length != msg->data_length){
		printf("CAN_snd_err\n");
	}
	/*
	// Test feedback
	MCP_bit_modify(MCP_TXB0CTRL, 0xB, 0xB);
	printf("TXBnCTRL: %d\n", MCP_read_byte(MCP_TXB0CTRL));
	printf("CANINTE: %d\n", MCP_read_byte(MCP_CANINTE));
	printf("CANINTF: %d\n", MCP_read_byte(MCP_CANINTF));
	MCP_request_to_send(0b10000111);
	//
	*/
	// Proclaim Send request	(There actually isn't a linear mapping between buff_num and RTS_TXn ...)	
	switch (buffer_num){
		case 0:
			//MCP_request_to_send(MCP_RTS_TX0);
			MCP_bit_modify(MCP_TXB0CTRL + buffer_num*0x10, 0x08, 0x08);
			break;
			
		case 1:
			//MCP_request_to_send(MCP_RTS_TX1);
			MCP_bit_modify(MCP_TXB0CTRL + buffer_num*0x10, 0x08, 0x08);
			break;
		
		case 2:
			//MCP_request_to_send(MCP_RTS_TX2);
			MCP_bit_modify(MCP_TXB0CTRL + buffer_num*0x10, 0x08, 0x08);
			break;
		
		default:
			//MCP_request_to_send(MCP_RTS_ALL);
			MCP_bit_modify(MCP_TXB0CTRL			, 0x08, 0x08);
			MCP_bit_modify(MCP_TXB0CTRL + 0x10	, 0x08, 0x08);
			MCP_bit_modify(MCP_TXB0CTRL + 0x20	, 0x08, 0x08);
	}
	return 1;
}

CANMSG CAN_read_rx_buffer(uint8_t rx_buf){
	CANMSG rec;
	rec.data_length = 9;
	if (rx_buf != 0 && rx_buf != 1) { printf("CAN Error: Requested read from buffer that doesn't exist; %d \n", rx_buf); return rec; }

	rec.ID_high = MCP_read_byte(MCP_RXB0SIDH + 0x10*rx_buf);
	rec.ID_low = MCP_read_byte(MCP_RXB0SIDL + 0x10*rx_buf);
	
	rec.data_length = MCP_read_byte(MCP_RXB0DLC + 0x10*rx_buf);
	for (uint8_t m = 0; m < rec.data_length; m++){
		rec.data[m] = MCP_read_byte(MCP_RXB0D0 + m + 0x10*rx_buf);
	}
	return rec;
}

void CAN_print_message(CANMSG* msg){
	uint8_t id = msg->ID_high*16 + msg->ID_low;
	printf("Message ID: %d\n", id);
	uint8_t len = msg->data_length;
	printf("Message data, %d bytes: ", len);
	for (uint8_t i = 0; i < len; i++){
		printf("%d ", msg->data[i]);
	}
	
	printf("\n");
}