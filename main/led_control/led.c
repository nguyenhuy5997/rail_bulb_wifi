/*
 * led.c
 *
 *  Created on: Mar 6, 2023
 *      Author: nguyenphuonglinh
 */

#include "led.h"
#include "../light_driver.h"
#include "../jsonUser/json_user.h"

extern __NOINIT_ATTR bool Flag_quick_pair;
extern __NOINIT_ATTR bool Flag_compatible_pair;
extern light_control rail_bulb_control;
extern QueueHandle_t mqtt_payload;
QueueHandle_t queueLedControl;


void init_led(void * arg) {
    queueLedControl = xQueueCreate(30, sizeof(light_control));
	cmd command_set;
	char item[250];
	while(1) {
		if (xQueueReceive(mqtt_payload, &item, portMAX_DELAY) == pdPASS) {
			ESP_LOGI("MQTT", "PAYLOAD: %s", item);
			memset(&command_set, 0, sizeof(command_set));
			JSON_analyze_SUB_MQTT(item, &command_set);
			ESP_LOGI("MQTT", "action: %s\n", (char*)command_set.action);
			if(strstr(command_set.action, "light-on-off")) {
				light_driver_set_switch(strstr(command_set.value, "on") ? 1 : 0);
			}

		}
	}
}

