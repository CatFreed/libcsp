//Common includes
#include <stdio.h>
#include <csp/drivers/usart.h>
#include <csp/arch/csp_thread.h>
#include <csp/csp.h>
#include <kubos-hal/uart.h>

//HAL-specific includes

//Function Initialization

//Variables

//Actual code starts here

/*Common functions
 * putstr
 * insert
 * set_callback
 * init
 */
void usart_putstr(char* buf, int len){
	k_uart_write(uart_num, buf, len); //Replace with write or sendData function
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
//TODO: this. While(1) read and stuff
}

void usart_init(struct usart_conf * conf){

	csp_thread_handle_t rxThread;
	//Configure port
	//TODO:  Probably need to cast all of these to the appropriate kubos data type?
	KUARTConf config = {0};
	config.dev_path = conf->device;
	config.baud_rate = conf->baudrate;
	config.parity = conf->paritysetting;
	config.stop_bits = conf->stopbits;
	config.word_len = conf->databits;

	//Initialization
	//Creates the KUART structure and then initializes at the device level
	k_uart_init(uart_num, config);

	//Start RX thread?  Do we have threads?
	//NOTE: Assuming this is running on one of the libcsp architectures (probably FreeRTOS)
	//      so csp_thread_create should allocate everything appropriately for me?
	//      Seems like a fairly large assumption...
	//      Copying parameters from the kiss.c example's SERVER/CLIENT threads
	csp_thread_create(uart_listen, "uartRXThread", 1000, NULL, 0, &rxThread);
}
