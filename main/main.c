#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "common.h"
#include "board.h"
#include "./Pair/QuickMode/SmartConfig.h"
#include "./Pair/HttpServer/WebServer.h"
#include "./jsonUser/json_user.h"
#include "./SPIFFS/spiffs_user.h"
#include "./WiFi/WiFi_proc.h"
#include "./Pair/CompatibleMode/AP.h"
#include "./heartbeat/heartbeat.h"
#include "./Mqtt/mqtt.h"
#include "./led_control/led.h"
#include "./light_driver/iot_led.h"
#include "./light_driver/light_driver.h"
#define TAG "MAIN"

__NOINIT_ATTR bool Flag_quick_pair = false;
__NOINIT_ATTR bool Flag_compatible_pair = false;
char brokerInfor[100];
Device Device_Infor;
light_control rail_bulb_control;
char topic_msg[70] = {'\0'};
char topic_cmd_set[70] = {'\0'};
char topic_deviceaction[70] = {'\0'};
char topic_heartbeat[70] = {'\0'};
char fwVersion[50] = {'\0'};
char svalue[200] = {'\0'};
extern QueueHandle_t mqtt_payload;
void manage_timing(void * arg)
{
	uint8_t restart_counter = 0;
	esp_err_t err;
	while(1){
		nvs_handle_t my_handle;
		err = nvs_open("storage", NVS_READWRITE, &my_handle);
		if (err != ESP_OK) {
			printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
			nvs_close(my_handle);
			vTaskDelete(NULL);
		} else {
			err = nvs_get_u8(my_handle, "restart_counter", &restart_counter);
			switch (err) {
				case ESP_OK:
					printf("Restart counter = %d\n", restart_counter);
					break;
				case ESP_ERR_NVS_NOT_FOUND:
					printf("The value is not initialized yet!\n");
					break;
				default :
					printf("Error (%s) reading!\n", esp_err_to_name(err));
			}
			restart_counter++;
			err = nvs_set_u8(my_handle, "restart_counter", restart_counter);
			printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
			err = nvs_commit(my_handle);
			printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
			vTaskDelay(2000/portTICK_RATE_MS);
			if(restart_counter == 5) {
				printf("restart_counter == 5, restart for paring\r\n");
				Flag_quick_pair = true;
				restart_counter = 0;
				err = nvs_set_u8(my_handle, "restart_counter", restart_counter);
				printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
				err = nvs_commit(my_handle);
				printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
				nvs_close(my_handle);
				esp_restart();
			} else if (restart_counter > 5) {
				restart_counter = 0;
				err = nvs_set_u8(my_handle, "restart_counter", restart_counter);
				printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
				err = nvs_commit(my_handle);
				printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
			}
			vTaskDelay(8000/portTICK_RATE_MS);
			restart_counter = 0;
			err = nvs_set_u8(my_handle, "restart_counter", restart_counter);
			printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
			err = nvs_commit(my_handle);
			printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
			nvs_close(my_handle);
			vTaskDelete(NULL);
		}
	}
}
void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
//    ESP_ERROR_CHECK( err );
    mqtt_payload = xQueueCreate(20, 250*sizeof(char));
	if( esp_reset_reason() == ESP_RST_UNKNOWN || esp_reset_reason() == ESP_RST_POWERON)
	{
		Flag_quick_pair = false;
		Flag_compatible_pair = false;
	}


    init_wifi();
    if(Flag_quick_pair) {
		xTaskCreate(timeout, "timeout", 1024, NULL, 10, NULL);
		start_smartconfig();
		ESP_LOGI(TAG, "Start quick pair");
    }
    xTaskCreate(manage_timing, "manage_timing", 2048, NULL, 3, NULL);
    xTaskCreate(init_led, "init_led", 4096 * 2, NULL, 3, NULL);

    mountSPIFFS();
	get_device_infor(&Device_Infor, brokerInfor);
	if (strlen(brokerInfor) == 0) {
		memcpy(brokerInfor, BROKER, strlen(BROKER) + 1);
	}
	ESP_LOGI(TAG, "BROKER: %s, ID: %s, TOK: %s", brokerInfor, Device_Infor.id, Device_Infor.token);
	sprintf(topic_cmd_set, "ont2mqtt/%s/commands/set", Device_Infor.id);
	sprintf(topic_deviceaction, "ont2mqtt/%s/deviceaction", Device_Infor.id);
	sprintf(topic_msg, "messages/%s/attribute", Device_Infor.id);
	sprintf(topic_heartbeat, "ont2mqtt/%s/heartbeat", Device_Infor.id);
	sprintf(fwVersion, "{\"fwVersion\":\"%s\"}", VERSION);

	if (Flag_quick_pair == false && Flag_compatible_pair == false)
	{
		wifi_config_t wifi_config = {
			.sta = {
				.threshold.authmode = WIFI_AUTH_WPA2_PSK,
				.pmf_cfg = {
					.capable = true,
					.required = false
				},
			},
		};
		if (esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config) == ESP_OK)
		{
			memcpy(wifi_config.sta.ssid, "ggwp", strlen("ggwp") + 1);
			memcpy(wifi_config.sta.password, "123456789", strlen("123456789") + 1);
			ESP_LOGI(TAG, "Wifi configuration already stored in flash partition called NVS");
			ESP_LOGI(TAG, "%s" ,wifi_config.sta.ssid);
			ESP_LOGI(TAG, "%s" ,wifi_config.sta.password);
			wifi_init_sta(wifi_config, WIFI_MODE_STA);
			mqtt_app_start(brokerInfor, Device_Infor.id, Device_Infor.token);
			xTaskCreate(heartbeat, "heartbeat", 2048, NULL, 3, NULL);
			ESP_LOGI("wifi", "free Heap:%d,%d", esp_get_free_heap_size(), heap_caps_get_free_size(MALLOC_CAP_8BIT));
		}
	}
}
