/*
 * Common.h
 *
 *  Created on: Jun 15, 2022
 *      Author: ASUS
 */

#ifndef MAIN_COMMON_H_
#define MAIN_COMMON_H_

#include <stdio.h>

#include "./SPIFFS/spiffs_user.h"

#define BROKER 						"mqtt://mqtt.innoway.vn:1883"
#define VERSION 					"0.0.0"

#define TIMEOUT						 180000 // timeout 3 minutes for pairing
//#define KIT_TEST

typedef struct _dvinfor
{
	char id[50];
	char token[50];
}Device;

enum system_state_t {
	UNKNOW,
	NORMAL,
	WIFI_DISCONNECTED,
	LOCAL_MODE,
	QUICK_MODE,
	COMPATIBLE_MODE,
	ESP_FOTA,
	ZIGBEE_FOTA,
	SUB_DEVICE_PAIR
};
enum process_state_t {
	PRO_UNKNOW,
	PRO_ESP_FOTA,
	PRO_ZIGBEE_FOTA
};
typedef struct _cmd
{
	char action[50];
	char nodeID[10]; 	// 10
	char clusterID[10];
	char attributeID[10];
	char EUI64[18];		// 18
	uint8_t endpoint;
	uint8_t ble_params;
	char value[20];
	char url[100];
	char protocol[10];	// 10
	uint32_t ts;
}cmd;

typedef struct _action
{
	char action[20];
	char nodeId[10];
	uint8_t	endpoint;
	char clusterId[10];
	char attributeId[10];
	char attributeBuff[10];
	char fwVersion[20];
	uint8_t code;
}action;
void get_device_infor(Device * _device, char * brokerInfor);

uint64_t mParseHex(char *data, size_t max_len);

void timeout(void *arg);
#endif /* MAIN_COMMON_H_ */
