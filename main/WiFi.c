/*
 * WiFi.c
 *
 *  Created on: 29 giu 2023
 *      Author: Mattia
 */



#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "WiFi.h"
#include "esp_http_client.h"

#include "esp_http_server.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "main.h"

#define LED_PIN 25

// Event group
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

static const char *TAG = "wifi station";


TaskHandle_t get=NULL;


#define SSID ""
#define PASSWORD ""





static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    	  ESP_LOGI(TAG, "in attesa di collegamento...");
          esp_wifi_connect();

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
            ESP_LOGE(TAG, "Wifi disconnesso... ");
            if(get!=NULL) { vTaskDelete(get);
            ESP_LOGI(TAG,"TASK eliminato %p", &get);
            get =  NULL;
            gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
            gpio_set_level(LED_PIN, 0);}
            ESP_LOGW(TAG, "Task ora corrente sospeso");
            esp_wifi_connect();

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

        xEventGroupSetBits(wifi_event_group, BIT0);
        if(get==NULL) {
        	xTaskCreate(&get_time,"ora",4096,NULL,2,&get);
        	ESP_LOGI(TAG,"TASK creato %p",&get);
        	gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
        	gpio_set_level(LED_PIN, 1);
        }

    }




}






void connectWifi() {

	wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	 wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	  esp_wifi_init(&wifi_init_config);

	    wifi_config_t wifi_config = {
	        .sta = {
			    .ssid = SSID,
	            .password = PASSWORD,

	        },
	    };



	    esp_event_handler_instance_t instance_any_id;
	        esp_event_handler_instance_t instance_got_ip;
	        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
	                                                            ESP_EVENT_ANY_ID,
	                                                            &event_handler,
	                                                            NULL,
	                                                            &instance_any_id));
	        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
	                                                            IP_EVENT_STA_GOT_IP,
	                                                            &event_handler,
	                                                            NULL,
	                                                            &instance_got_ip));


		    esp_wifi_set_mode(WIFI_MODE_APSTA);
		    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

		    esp_wifi_start();

		 EventBits_t bits=xEventGroupWaitBits(wifi_event_group, BIT0 | BIT1, false, false, portMAX_DELAY);
		 if (bits & BIT0) {
		        ESP_LOGI(TAG, "Connesso");

		    } else if (bits & BIT1) {
		        ESP_LOGI(TAG, "Connessione fallita");
		    } else {
		        ESP_LOGE(TAG, "Evento inatteso");
		    }


}



