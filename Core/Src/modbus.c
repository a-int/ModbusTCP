#include "modbus.h"
#include <string.h>


static void mb_process_read_fn(char *mb_repl_buf, char *mb_req_buf, uint16_t req_buf_len);
static void mb_process_err(char *mb_repl_buf, uint8_t fn, uint16_t exceptionCode);
static int mb_process_start_address(uint16_t fn, uint16_t start_address, uint16_t quantity);

static void mb_mbap_copy(char *mb_repl_buf, char *mb_req_buf);
static uint16_t mb_start_offset(uint8_t fn, uint16_t start_address);
static uint16_t mb_calculate_len(char *mb_repl_buf);
static uint16_t mb_pdu_calc_N(uint16_t fn, uint16_t quantity);

uint16_t mb_process(char *mb_repl_buf, char *mb_req_buf, uint16_t req_buf_len) {
	memset(mb_repl_buf, '\0', MB_ADU_MAXSIZE);  // clear the buffer

	mb_mbap_copy(mb_repl_buf, mb_req_buf);

	uint8_t fn = mb_req_buf[MB_PDU_FN];

	switch (fn) {
	case MB_FN_READ_COILS:
		mb_process_read_fn(mb_repl_buf, mb_req_buf, req_buf_len);
		break;
	case MB_FN_READ_DISCRETE: // read Discrete inputs
		mb_process_read_fn(mb_repl_buf, mb_req_buf, req_buf_len);
		break;
	case MB_FN_READ_HOLDING: // read Holding Registers
		mb_process_read_fn(mb_repl_buf, mb_req_buf, req_buf_len);
		break;
	case MB_FN_READ_INPUT: // read Input Registers
		mb_process_read_fn(mb_repl_buf, mb_req_buf, req_buf_len);
		break;
	case 0x05: // Write single Coil
	case 0x06: // Write single Register
	case 0x0f: // Write multiple Coils
	case 0x10: // Write single Coil
	default:
		mb_process_err(mb_repl_buf, fn, MB_EXCEPTION_FN_UNSUPPORTED);
		break;
	}

	uint16_t len = mb_calculate_len(mb_repl_buf);
	mb_repl_buf[MB_MBAP_LEN_H] = (len) >> 8;
	mb_repl_buf[MB_MBAP_LEN_L] = (len) & 0xff;

	return (len + 6); // MODBUS Messaging on TCP/IP 1.0b sec. 3.1.3 pg. 5
}

static void mb_process_read_fn(char *mb_repl_buf, char *mb_req_buf, uint16_t req_buf_len) {
	uint8_t fn = mb_req_buf[MB_PDU_FN];
	uint16_t start_address = mb_req_buf[MB_PDU_ST_ADDR_L] + (mb_req_buf[MB_PDU_ST_ADDR_H] << 8);
	uint16_t quantity = mb_req_buf[MB_PDU_QUANTITY_L] + (mb_req_buf[MB_PDU_QUANTITY_H] << 8);

	if (mb_process_start_address(fn, start_address, quantity) != MB_EXCEPTION_OK) {
		mb_process_err(mb_repl_buf, fn, MB_EXCEPTION_DATA_ADDR);
	} else { // form PDU content
		mb_repl_buf[MB_PDU_FN] = fn;
		mb_repl_buf[MB_PDU_REPL_N] = mb_pdu_calc_N(fn, quantity);
		// mb_process_read_fn_vals(mb_repl_buf, mb_req_buf);
	}
}

static void mb_mbap_copy(char *mb_repl_buf, char *mb_req_buf) {
	mb_repl_buf[MB_MBAP_TRANSACTION_ID_H] = mb_req_buf[MB_MBAP_TRANSACTION_ID_H];
	mb_repl_buf[MB_MBAP_TRANSACTION_ID_L] = mb_req_buf[MB_MBAP_TRANSACTION_ID_L];
	mb_repl_buf[MB_MBAP_PROTOCOL_ID_H] = mb_req_buf[MB_MBAP_PROTOCOL_ID_H];
	mb_repl_buf[MB_MBAP_PROTOCOL_ID_L] = mb_req_buf[MB_MBAP_PROTOCOL_ID_L];
	mb_repl_buf[MB_MBAP_CLIENT_ID] = mb_req_buf[MB_MBAP_CLIENT_ID];
}

static int mb_process_start_address(uint16_t fn, uint16_t start_address, uint16_t quantity) {
	uint8_t exception_code = MB_EXCEPTION_OK;
	switch (fn) {
	case MB_FN_READ_COILS:
		if ((start_address + quantity) > MB_COILS_Q)
			exception_code = 2;
		break;
	case MB_FN_READ_DISCRETE:
		if ((start_address + quantity) > MB_DISCRETE_Q)
			exception_code = 2;
		break;
	case MB_FN_READ_HOLDING:
		if ((start_address + quantity * 2) > MB_HOLDING_Q)
			exception_code = 2;
		break;
	case MB_FN_READ_INPUT:
		if ((start_address + quantity * 2) > MB_INPUT_Q)
			exception_code = 2;
		break;
	}
	return exception_code;
}

static uint16_t mb_start_offset(uint8_t fn, uint16_t start_address) {
	switch (fn) {
	case MB_FN_READ_COILS:
		return start_address + MB_COILS_ST;
		break;
	case MB_FN_READ_DISCRETE:
		return start_address + MB_DISCRETE_ST;
		break;
	case MB_FN_READ_HOLDING:
		return start_address + MB_HOLDING_ST;
		break;
	case MB_FN_READ_INPUT:
		return start_address + MB_INPUT_ST;
		break;
	default:
		return -1;
	}
}

static uint16_t mb_calculate_len(char *mb_repl_buf) {
	uint16_t len = 0;
	while (mb_repl_buf[MB_MBAP_CLIENT_ID + len] != '\0') {
		++len;
	}
	return len;
}

static void mb_process_err(char *mb_repl_buf, uint8_t fn, uint16_t exceptionCode) {
	mb_repl_buf[MB_PDU_FN] = fn | 0x80;
	mb_repl_buf[MB_PDU_EXCEPTION_CODE] = exceptionCode;
}

static uint16_t mb_pdu_calc_N(uint16_t fn, uint16_t quantity) {
	switch (fn) {
		case MB_FN_READ_COILS:
		case MB_FN_READ_DISCRETE:
			if(quantity % 8 != 0) return ((quantity / 8) + 1);
			else return (quantity / 8);
			break;
		case MB_FN_READ_HOLDING:
		case MB_FN_READ_INPUT:
			return quantity * 2;
			break;
		default:
			return 0;
		}
}
