#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "espressif/espconn.h"
#include "user_config.h"
#include "uart.h"
#include "gpio.h"
#include "my_types.h"

#define PIN_PIR_IN	(5)
#define PIN_RELAY1_OUT	(2)
// #define PIN_RELAY2_OUT	(4)

LOCAL xQueueHandle	printQueue = NULL;

void ICACHE_FLASH_ATTR
relay_task(void *pvParameters) {
	short val;
	portBASE_TYPE xStatus;

	if (printQueue == NULL) {
		printf("Print queue is not created!\n");
		vTaskDelete(NULL);
	}

	printf("Print task started\n");

	while (1) {
		// Get queue each 1ms
		xStatus = xQueueReceive(printQueue,&val,1 / portTICK_RATE_MS);
		if (xStatus == pdPASS){
			printf("-%d\n", val);
			GPIO_OUTPUT_SET(GPIO_ID_PIN(PIN_RELAY1_OUT), val);
		}
	}
	printf("QUIT!!!\n");
	vTaskDelete(NULL);
}

void ICACHE_FLASH_ATTR
pir_output_int_handler() {
	portBASE_TYPE xHigherPriorityTaskWoken = 0;
	short gpio_status = (short)GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	short tmp = 0;
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);
	if (printQueue) {
		if (gpio_status & BIT(PIN_PIR_IN)) {
			tmp = (GPIO_INPUT_GET(GPIO_ID_PIN(PIN_PIR_IN)));
			xQueueSendFromISR(printQueue, (void *)&tmp, &xHigherPriorityTaskWoken);
			portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
		}
	}
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_init(void)
{
	UART_SetBaudrate(UART0, BIT_RATE_115200);
	printf("SDK version:%s\n", system_get_sdk_version());

	/* need to set opmode before you set config */
	wifi_set_opmode(NULL_MODE);

	printQueue = xQueueCreate(10, sizeof(short));
	if (printQueue == 0) {
		printf("Queue create failed!!!\n");
	} else {
		printf("Queue created ok\n");
	}

	// Setup GPIO pins
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO5_U);
	PIN_PULLDWN_DIS(PERIPHS_IO_MUX_GPIO5_U);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO2_U);

	// set gpio 2 as output and low. set gpio 5 as input
	gpio_output_conf(0, GPIO_ID_PIN(PIN_RELAY1_OUT), GPIO_ID_PIN(PIN_RELAY1_OUT), GPIO_ID_PIN(PIN_PIR_IN));

	// Disable interrupts by GPIO
	_xt_isr_mask(1<<ETS_GPIO_INUM);
	// Register GPIO interrupt handler
	gpio_intr_handler_register(pir_output_int_handler, NULL);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(PIN_PIR_IN));
	gpio_pin_intr_state_set(GPIO_ID_PIN(PIN_PIR_IN), GPIO_PIN_INTR_ANYEDGE);
	// Enable GPIO interrupt
	_xt_isr_unmask(1<<ETS_GPIO_INUM);

	// Create Queue
	if (printQueue == NULL)
		printQueue = xQueueCreate(10, sizeof(short));

	// Create task
	xTaskCreate(relay_task, "relay_task", 256, NULL, 3, NULL);
}

