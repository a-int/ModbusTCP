#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "string.h"

#include "tcpserver.h"
#include "modbus.h"

static struct netconn *conn, *newconn;
static struct netbuf *netbuf;
char mb_req_buf[MB_ADU_MAXSIZE];
char mb_repl_buf[MB_ADU_MAXSIZE];

static void copyString(char* dest, char* src, unsigned num);

static void tcp_thread(void *arg) {
	err_t err, accept_err;
	conn = netconn_new(NETCONN_TCP);
	if (conn != NULL) {
		err = netconn_bind(conn, IP_ADDR_ANY, MB_TCP_PORT);
		if (err == ERR_OK) {
			netconn_listen(conn);
			while (1) {
				accept_err = netconn_accept(conn, &newconn);
				if (accept_err == ERR_OK) {
					while (netconn_recv(newconn, &netbuf) == ERR_OK) {
						do {
							copyString(mb_req_buf, netbuf->p->payload, netbuf->p->len); // get the message from the client
							uint16_t buf_len = netbuf->p->len;
							buf_len = mb_process(mb_repl_buf, mb_req_buf, buf_len); // process the data
							netconn_write(newconn, mb_repl_buf, buf_len, NETCONN_COPY); // send the message back to the client
						} while (netbuf_next(netbuf) > 0);
						netbuf_delete(netbuf);
					}
					netconn_close(newconn);
					netconn_delete(newconn);
				}
			}
		} else {
			netconn_delete(conn);
		}
	}
}

void tcpserver_init(void) {
	sys_thread_new("tcp_thread", tcp_thread, NULL, 8*DEFAULT_THREAD_STACKSIZE,
			osPriorityNormal);
}

static void copyString(char* dest, char* src, unsigned num){
	for(unsigned i = 0; i != num; ++i){
		dest[i] = src[i];
	}
}
