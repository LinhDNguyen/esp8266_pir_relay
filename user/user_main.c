#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "espressif/espconn.h"
#include "user_config.h"
#include "my_types.h"

void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)
{
	switch(status) {
		case SC_STATUS_WAIT:
			printf("SC_STATUS_WAIT\n");
			break;
		case SC_STATUS_FIND_CHANNEL:
			printf("SC_STATUS_FIND_CHANNEL\n");
			break;
		case SC_STATUS_GETTING_SSID_PSWD:
			printf("SC_STATUS_GETTING_SSID_PSWD\n");
			sc_type *type = pdata;
			if (*type == SC_TYPE_ESPTOUCH) {
				printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
			} else {
				printf("SC_TYPE:SC_TYPE_AIRKISS\n");
			}
			break;
		case SC_STATUS_LINK:
			printf("SC_STATUS_LINK\n");
			struct station_config *sta_conf = pdata;

			wifi_station_set_config(sta_conf);
			wifi_station_disconnect();
			wifi_station_connect();
			break;
		case SC_STATUS_LINK_OVER:
			printf("SC_STATUS_LINK_OVER\n");
			if (pdata != NULL) {
				//SC_TYPE_ESPTOUCH
				uint8 phone_ip[4] = {0};

				memcpy(phone_ip, (uint8*)pdata, 4);
				printf("Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
			}
			smartconfig_stop();
			break;
	}

}

void ICACHE_FLASH_ATTR
smartconfig_task(void *pvParameters)
{
	smartconfig_start(smartconfig_done);

	vTaskDelete(NULL);
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
	UART_ConfigTypeDef uart1Config = {
		.baud_rate = BIT_RATE_115200,
		.data_bits = UART_WordLength_8b,
		.parity = USART_Parity_None,
		.stop_bits = USART_StopBits_2,
		.flow_ctrl = USART_HardwareFlowControl_None,
		.UART_RxFlowThresh = 0,
		.UART_InverseMask = 0,
	};
	/* Configure uart1, then os printf through this uart */
	UART_ParamConfig(UART1,  &uart1Config);
	UART_SetPrintPort(UART1);

	UART_SetBaudrate(UART0, BIT_RATE_115200);
	printf("SDK version:%s\n", system_get_sdk_version());

	/* need to set opmode before you set config */
	wifi_set_opmode(STATION_MODE);

	xTaskCreate(smartconfig_task, "smartconfig_task", 256, NULL, 2, NULL);
}

