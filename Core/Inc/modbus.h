#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_
#include "inttypes.h"

#define MB_TCP_PORT (502)

#define MB_PDU_MAXSIZE (253)
#define MB_MBAP_SIZE (7)
#define MB_ADU_MAXSIZE (MB_PDU_MAXSIZE + MB_MBAP_SIZE)

#define MB_MBAP_TRANSACTION_ID_H	(0)
#define MB_MBAP_TRANSACTION_ID_L	(1)
#define MB_MBAP_PROTOCOL_ID_H 		(2)
#define MB_MBAP_PROTOCOL_ID_L 		(3)
#define MB_MBAP_LEN_H 						(4)
#define MB_MBAP_LEN_L 						(5)
#define MB_MBAP_CLIENT_ID 				(6)

#define MB_PDU_FN									(7)
//req byte positions
#define MB_PDU_ST_ADDR_H					(8)
#define MB_PDU_ST_ADDR_L					(9)
#define MB_PDU_QUANTITY_H					(10)
#define MB_PDU_QUANTITY_L					(11)
// repl byte positions
#define MB_PDU_VAL_START					(MB_PDU_FN+1)
#define MB_PDU_EXCEPTION_CODE			(MB_PDU_VAL_START)


#define MB_COILS_ST			(0)
#define MB_COILS_Q			(4)
#define MB_DISCRETE_ST	(MB_COILS_ST + MB_COILS_Q)
#define MB_DISCRETE_Q		(0)
#define MB_HOLDING_ST 	(MB_DISCRETE_ST + MB_DISCRETE_Q)
#define MB_HOLDING_Q		(0 * 2)
#define MB_INPUT_ST			(MB_HOLDING_ST + MB_HOLDING_Q)
#define MB_INPUT_Q			(4 * 2)
#define MB_MEMORY_SIZE	(MB_COILS_Q + MB_DISCRETE_Q + MB_HOLDING_Q + MB_INPUT_Q)

#define MB_FN_READ_COILS		(0x01)
#define MB_FN_READ_DISCRETE	(0x02)
#define MB_FN_READ_HOLDING	(0x03)
#define MB_FN_READ_INPUT		(0x04)

#define MB_EXCEPTION_OK					(0x00)
#define MB_EXCEPTION_FN_UNSUPPORTED (0x01)
#define MB_EXCEPTION_DATA_ADDR			(0x02)
#define MB_EXCEPTION_DATA_VAL				(0x03)

uint16_t mb_process(char *mb_repl_buf, char *mb_req_buf, uint16_t req_buf_len);
#endif /* INC_MODBUS_H_ */
