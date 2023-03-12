/*
 * led.c
 *
 *  Created on: Mar 6, 2023
 *      Author: nguyenphuonglinh
 */

#include "led.h"
#include "../light_driver/light_driver.h"
#include "../jsonUser/json_user.h"
#include "../OTA/fota.h"

extern __NOINIT_ATTR bool Flag_quick_pair;
extern __NOINIT_ATTR bool Flag_compatible_pair;
extern light_control rail_bulb_control;
extern QueueHandle_t mqtt_payload;
extern bool Flag_updating_fw;
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
			if (strcmp(command_set.action, "upgrade") == 0) {
				Flag_updating_fw = true;
				xTaskCreate(timeout, "timeout", 1024, NULL, 10, NULL);
				if(strlen(command_set.url) > 0){
					xTaskCreate(&esp_ota_task, "ota_task", 8192, command_set.url, 5, NULL);
				} else {
					xTaskCreate(&esp_ota_task, "ota_task", 8192, NULL, 5, NULL);
				}
			}
			else if (strstr(command_set.action, "light-on-off")) {
				light_driver_set_switch(strstr(command_set.value, "on") ? 1 : 0);
			}
			else if (strstr(command_set.action, "light-dim")) {
				light_driver_set_brightness(mParseHex(command_set.value, 6) * 100 / BRIGHTNESS_MAX);
			}
			else if (strstr(command_set.action, "light-cct")) {
				light_driver_set_color_temperature(mParseHex(command_set.value, 6) * 100 / TEMPERATURE_MAX);
			}
			else if (strstr(command_set.action, "light-rgb")) {
				ESP_LOGI("HSV", "Saturation: %d - 0x%x", mParseHex(command_set.value, 10) >> 16 & 0xFFFF);
				light_driver_set_hue(mParseHex(command_set.value, 10) >> 16 & 0xFFFF);

				ESP_LOGI("HSV", "Hue: %d - 0x%x", mParseHex(command_set.value, 10) >> 8 & 0xFF);
				light_driver_set_saturation(mParseHex(command_set.value, 10) >> 8 & 0xFF);

				ESP_LOGI("HSV", "Value: %d - 0x%d", mParseHex(command_set.value, 10) & 0xFF);
				light_driver_set_value(mParseHex(command_set.value, 10) & 0xFF);
			}
		}
	}
}

