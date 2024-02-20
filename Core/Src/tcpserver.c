#include "lwip/opt.h"

#include "lwip/api.h"
#include "lwip/sys.h"

#include "tcpserver.h"
#include "string.h"

#include "modbus.h"

static struct netconn *conn, *newconn;
static struct netbuf *netbuf;
char mb_req_buf[MB_ADU_MAXSIZE];

void copyString(char* dest, char* src, unsigned num);

static void tcp_thread(void *arg) {
	err_t err, accept_err;

	/* Create a new connection identifier. */
	conn = netconn_new(NETCONN_TCP);

	if (conn != NULL) {
		/* Bind connection to the modbus tcp port . */
		err = netconn_bind(conn, IP_ADDR_ANY, MB_TCP_PORT);

		if (err == ERR_OK) {
			/* Tell connection to go into listening mode. */
			netconn_listen(conn);

			while (1) {
				/* Grab new connection. */
				accept_err = netconn_accept(conn, &newconn);

				/* Process the new connection. */
				if (accept_err == ERR_OK) {

					/* receive the data from the client */
					while (netconn_recv(newconn, &netbuf) == ERR_OK) {

						/* If there is some data remaining to be sent, the following process will continue */
						do {
							copyString(mb_req_buf, (netbuf->p->payload), netbuf->p->len); // get the message from the client

							u16_t mb_req_len = netbuf->p->len;
							//mb_process(mb_req_buf, mb_req_len);

							netconn_write(newconn, mb_req_buf, netbuf->p->len, NETCONN_COPY); // send the message back to the client
						} while (netbuf_next(netbuf) > 0);

						netbuf_delete(netbuf);
					}

					/* Close connection and discard connection identifier. */
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
	sys_thread_new("tcp_thread", tcp_thread, NULL, DEFAULT_THREAD_STACKSIZE,
			osPriorityNormal);
}

void copyString(char* dest, char* src, unsigned num){
	for(unsigned i = 0; i != num; ++i){
		dest[i] = src[i];
	}
}
