#include <stdio.h>
#include <csp/drivers/usart.h>
#include <csp/arch/csp_thread.h>
#include <csp/csp.h>
#include <kubos-hal/uart.h>

//Function Initialization

//Variables
usart_callback_t usart_callback = NULL;

//TODO: Move this to somewhere so that multiple UART ports can be open and communicating at the same time.
// This is how the Windows and Linux drivers currently do it, though.
KUARTNum uart_num;

/*Common functions
 * putstr
 * insert
 * set_callback
 * init
 */
void usart_putstr(char* buf, int len){
	k_uart_write(uart_num, buf, len);
}

void usart_putc(char c){
	k_uart_write_immediate(uart_num, c);
}

void usart_insert(char c, void * pxTaskWoken){
	//Redirect debug output to stdout
	printf("%c", c);
}

void usart_set_callback(usart_callback_t callback){
	usart_callback = callback;
}

int usart_messages_waiting(int handle){
	return k_uart_rx_queue_len((KUARTNUM) handle);
}

CSP_DEFINE_TASK(uart_listen) {

	int length;
	char recvBuffer[300];

	//Receive loop
	//TODO: While(!exit) instead of while(1)?
	while (1){

		length = k_uart_read(uart_num, recvBuffer, 300);

		if (length <=0){
			log("Connection Closed");
			exit(1);
		}

		if (usart_callback){
			usart_callback(recvBuffer, length, NULL);
		}
	}
}

void usart_init(struct usart_conf * conf){

	csp_thread_handle_t rxThread;

	//Configure port
	KUARTConf config = {0};
	config.dev_path = conf->device;
	config.baud_rate = conf->baudrate;
	config.parity = conf->paritysetting;
	config.stop_bits = conf->stopbits;
	config.word_len = conf->databits;

	//Initialization
	//Creates the KUART structure and then initializes at the device level
	k_uart_init(uart_num, config);

	//NOTE: csp_thread_create is an arch-dependent call, so should end up calling
	// the FreeRTOS thread function.  Will need to change if incorrect.
	csp_thread_create(uart_listen, "uartRXThread", 1000, NULL, 0, &rxThread);
}
