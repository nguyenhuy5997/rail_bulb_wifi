/* MQTT (over TCP) Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */



#include "../jsonUser/json_user.h"
#include "../common.h"
#include "../common.h"
#include "../Mqtt/mqtt.h"
#include "../led_control/led.c"

static const char *TAG = "MQTT";

extern bool LINK_BUTTON;

esp_mqtt_client_config_t mqtt_cfg;
esp_mqtt_client_handle_t client;
QueueHandle_t mqtt_payload;
bool Flag_updating_fw = false;
extern char topic_msg[70];
extern char topic_cmd_set[70];
extern char topic_heartbeat[70];
extern char fwVersion[50];
extern bool no_ack_hearbeat;

extern char wifi_ip[30];
extern uint32_t uptime;
extern char svalue[200];
extern light_control rail_bulb_control;
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
	esp_mqtt_event_handle_t event = event_data;
	esp_mqtt_client_handle_t client = event->client;
	switch ((esp_mqtt_event_id_t)event_id) {
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "STATE = NORMAL");
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		esp_mqtt_client_subscribe(client, topic_cmd_set, 0);
		esp_mqtt_client_subscribe(client, topic_heartbeat, 0);
		if (!Flag_updating_fw) {
			esp_mqtt_client_publish(client, topic_msg, fwVersion, strlen(fwVersion), 0, 0);
		}
		sprintf(svalue, "{\"uptime\":\"%d\",\"ip\":\"%s\"}", uptime, wifi_ip);
		esp_mqtt_client_publish(client, topic_heartbeat, svalue, strlen(svalue), 0, 0);
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "STATE = LOCAL_MODE");
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;

	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_DATA:
		no_ack_hearbeat = false;
		if (event->data_len > 0 && event->data) {
			event->data[event->data_len] = NULL;
			xQueueSendFromISR(mqtt_payload, (char *) event->data, 10/portTICK_RATE_MS);
		}
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		break;
	default:
		ESP_LOGI(TAG, "Other event id:%d", event->event_id);
		break;
	}
}

void mqtt_app_start(char *broker, char *client_id, char *passowrd)
{
	if (mqtt_payload == NULL) {
		ESP_LOGE(TAG, "Failed to create mqtt_payload\n");
	}
	mqtt_cfg.uri = broker;
	mqtt_cfg.username = client_id;
	mqtt_cfg.client_id = client_id;
//	mqtt_cfg.password = "0MWOLFtEBnRcRxKoVFNFb4OkvIkCKMg8";
	mqtt_cfg.password = passowrd;
	mqtt_cfg.keepalive = 60;
	mqtt_cfg.reconnect_timeout_ms = 1000;
	client = esp_mqtt_client_init(&mqtt_cfg);
//	xTaskCreate(mqtt_handle, "mqtt_handle", 8192, NULL, 3, NULL);
	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
	esp_mqtt_client_start(client);
}
