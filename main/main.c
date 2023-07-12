#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "driver/i2c.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "WiFi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"









int count=0;

char *ora;


void get_time();
esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt);




void app_main(void)
{
	ESP_ERROR_CHECK(nvs_flash_init());
	connectWifi();

}



esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
    	if(count==0)
    	{
    		ora= (char*)malloc(sizeof(char)*5);
    		count+=1;
    		char *sntnc=evt->data;
            char *string2=strstr(sntnc, "datetime");
            char *string3;
            string3= (char *)malloc(sizeof(char)*60);
            strncpy(string3, string2, 30);
            char *string4= (char*)malloc(sizeof(char)*60);
            string4=strstr(string3, "T");
            char *string5= (char *)malloc(sizeof(char)*60);
            strncpy(string5, string4, 8);
            printf("Ora attuale: ");
            for (int a1=1;a1<6;a1++)
            {
            printf("%c",string5[a1]);
            ora[a1-1]=string5[a1];
            }
           printf("\n");
    	}
        break;

    default:
        break;
    }
    return ESP_OK;
}

void get_time()
{
while(1) {

	  esp_http_client_config_t config_get = {
	        .url = "http://worldtimeapi.org/api/timezone/Europe/Rome/",


			  .method = HTTP_METHOD_GET,
	        .cert_pem = NULL,
	        .event_handler = client_event_get_handler};

	    esp_http_client_handle_t client = esp_http_client_init(&config_get);
	    esp_http_client_perform(client);
	    esp_http_client_cleanup(client);


	     vTaskDelay(5000/portTICK_PERIOD_MS);
	     count=0;
 }

}

