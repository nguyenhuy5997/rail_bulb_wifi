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
	while(1){
		sprintf(svalue, "{\"uptime\":\"%d\",\"ip\":\"%s\"}", uptime, wifi_ip);
		esp_mqtt_client_publish(client, topic_heartbeat, svalue, strlen(svalue), 0, 0);
		vTaskDelay((HEARTBEAT_TIMEOUT - 1000)/portTICK_RATE_MS);
		uptime +=(HEARTBEAT_TIMEOUT/1000);
	}
}


