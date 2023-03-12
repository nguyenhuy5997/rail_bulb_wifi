/*
 * heartbeat.c
 *
 *  Created on: Jan 30, 2023
 *      Author: nguyenphuonglinh
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
#include "heartbeat.h"
#include "../Mqtt/mqtt.h"
#include "../common.h"

uint32_t uptime = 0;

extern char svalue[200];
extern char topic_heartbeat[70];
extern char wifi_ip[30];
extern esp_mqtt_client_handle_t client;

bool no_ack_hearbeat = true;
bool refresh_mqtt = true;
void esp_task_wdt_isr_user_handler(void)
{
	esp_restart();
}

void heartbeat(void *arg){
	esp_task_wdt_init((HEARTBEAT_TIMEOUT + 15000)/1000, true);
	printf("TWDT initialized\n");
	 esp_task_wdt_add(NULL);
	esp_task_wdt_status(NULL);
	while(1){
		esp_task_wdt_reset();
		no_ack_hearbeat = true;
		sprintf(svalue, "{\"uptime\":\"%d\",\"ip\":\"%s\"}", uptime, wifi_ip);
		esp_mqtt_client_publish(client, topic_heartbeat, svalue, strlen(svalue), 0, 0);
		//Delay 1s to determine whether there is ack from heartbeat or not
		vTaskDelay(1000/portTICK_RATE_MS);
		if(no_ack_hearbeat){
			//if there is no ack from heartbeat then try to publish heartbeat again
			ESP_LOGI("HB", "Retry send heartbeat 2nd time\r\n");
			sprintf(svalue, "{\"uptime\":\"%d\",\"ip\":\"%s\"}", uptime, wifi_ip);
			esp_mqtt_client_publish(client, topic_heartbeat, svalue, strlen(svalue), 0, 0);
			//Delay 1s to determine whether there is ack from heartbeat or not
			vTaskDelay(1000/portTICK_RATE_MS);
			//if there is still no ack from heartbeat then refresh mqtt
			if(no_ack_hearbeat) {
				esp_mqtt_client_disconnect(client);
				esp_mqtt_client_reconnect(client);
				ESP_LOGI("HB", "Refresh MQTT cause none ack from heartbeat");
			}
		}
		vTaskDelay((HEARTBEAT_TIMEOUT - 1000)/portTICK_RATE_MS);
		uptime +=(HEARTBEAT_TIMEOUT/1000);
	}
}


