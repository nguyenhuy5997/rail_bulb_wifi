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
#include "../mdf/mdf_err.h"

extern __NOINIT_ATTR bool Flag_quick_pair;
extern __NOINIT_ATTR bool Flag_compatible_pair;
extern QueueHandle_t mqtt_payload;
extern bool Flag_updating_fw;
QueueHandle_t queueLedControl;


void init_led(void * arg) {
    light_driver_config_t driver_config = {
        .gpio_red        = CONFIG_LIGHT_GPIO_RED,
        .gpio_green      = CONFIG_LIGHT_GPIO_GREEN,
        .gpio_blue       = CONFIG_LIGHT_GPIO_BLUE,
        .gpio_cold       = CONFIG_LIGHT_GPIO_COLD,
        .gpio_warm       = CONFIG_LIGHT_GPIO_WARM,
        .fade_period_ms  = CONFIG_LIGHT_FADE_PERIOD_MS,
        .blink_period_ms = CONFIG_LIGHT_BLINK_PERIOD_MS,
    };
	if(Flag_quick_pair) {
		light_driver_init_in_pairing_mode(&driver_config);
		while(1) {
		    vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}
    light_driver_init(&driver_config);
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
			else if (strstr(command_set.action, "on-off")) {
				light_driver_set_switch(strstr(command_set.value, "on") ? 1 : 0);
			}
			else if (strstr(command_set.action, "brightness")) {
				ESP_LOGI("CTT", "Brightness: %d - 0x%4x", atoi(command_set.value) * 100 / 0xFFFF);
				light_driver_set_brightness(atoi(command_set.value) * 100 / 0xFFFF);
			}
			else if (strstr(command_set.action, "temperature")) {
				ESP_LOGI("CTT", "Temperature: %d - 0x%4x", (atoi(command_set.value) - 8195)* 100 / (20000 - 8195));
				light_driver_set_color_temperature((atoi(command_set.value) - 8195)* 100 / (20000 - 8195));
			}
			else if (strstr(command_set.action, "light-rgb")) {
				ESP_LOGI("HSV", "Saturation: %" PRIu64"", (mParseHex(command_set.value, 14) >> 32 & 0xFFFF) * 100 / 0xFFFF);
				light_driver_set_saturation((mParseHex(command_set.value, 14) >> 32 & 0xFFFF) * 100 / 0xFFFF);

				ESP_LOGI("HSV", "Hue: %" PRIu64"", (mParseHex(command_set.value, 14) >> 16 & 0xFFFF) * 360 / 0xFFFF);
				light_driver_set_hue((mParseHex(command_set.value, 14) >> 16 & 0xFFFF) * 360 / 0xFFFF);

				ESP_LOGI("HSV", "Value: %" PRIu64"", (mParseHex(command_set.value, 14) & 0xFFFF) * 100 / 0xFFFF);
				light_driver_set_value((mParseHex(command_set.value, 14) & 0xFFFF) * 100 / 0xFFFF);
			}
		}
	}
}

