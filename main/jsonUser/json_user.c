
#include "json_user.h"
#include "../common.h"

void JSON_analyze_post(char* my_json_string, char * deviceid, char * devicetoken, char * brokerInfor)
{
	cJSON *root = cJSON_Parse(my_json_string);
	cJSON *current_element = NULL;
	cJSON_ArrayForEach(current_element, root)
	{
		if (current_element->string)
		{
			const char* string = current_element->string;
			if(strcmp(string, "broker") == 0)
			{
				memcpy(brokerInfor, current_element->valuestring, strlen(current_element->valuestring) + 1);
			}
			if(strcmp(string, "deviceid") == 0)
			{
				memcpy(deviceid, current_element->valuestring, strlen(current_element->valuestring) + 1);
			}
			if(strcmp(string, "devicetoken") == 0)
			{
				memcpy(devicetoken, current_element->valuestring, strlen(current_element->valuestring) + 1);
			}
		}
	}
	cJSON_Delete(root);
}

void JSON_analyze_SUB_MQTT(char* my_json_string, cmd * sub_cmd)
{
	cJSON *root = cJSON_Parse(my_json_string);
	cJSON *current_element = NULL;
	cJSON_ArrayForEach(current_element, root)
	{
		if (current_element->string)
		{
			const char* string = current_element->string;
			if(strcmp(string, "action") == 0)
			{
				memcpy(sub_cmd->action, current_element->valuestring, strlen(current_element->valuestring)+ 1);
			}
			if(strcmp(string, "value") == 0)
			{
				memcpy(sub_cmd->value, current_element->valuestring, strlen(current_element->valuestring) + 1);
			}
			if(strcmp(string,"url") == 0)
			{
				memcpy(sub_cmd->url, current_element->valuestring, strlen(current_element->valuestring) + 1);
			}
			if(strcmp(string,"ts") == 0)
			{
				sub_cmd->ts = current_element->valueint;
			}
		}
	}
	cJSON_Delete(root);
}

bool JSON_analyze_SUB_ZIGBEE(char* my_json_string, action * sub_action)
{
	bool flag_data_feild = false;
	cJSON *root = cJSON_Parse(my_json_string);
	if(root == NULL) return false;
	cJSON *current_element = NULL;
	cJSON_ArrayForEach(current_element, root)
	{
		if (current_element->string)
		{
			const char* string = current_element->string;

			if(strcmp(string, "action") == 0)
			{
				strcpy(sub_action->action, current_element->valuestring);
			}
			if(strcmp(string, "fwVersion") == 0)
			{
				strcpy(sub_action->fwVersion, current_element->valuestring);
			}
			if(strcmp(string, "code") == 0){
				sub_action->code = current_element->valueint;
			}
			if(strcmp(string, "nodeId") == 0){

				strcpy(sub_action->nodeId, current_element->valuestring);
			}
			if(strcmp(string, "endpoint") == 0){

				sub_action->endpoint = current_element->valueint;
			}
			if(strcmp(string, "data") == 0){
				cJSON *subData = cJSON_GetObjectItem(root, "data");
				cJSON *subElement = NULL;
				cJSON_ArrayForEach(subElement, subData){
					cJSON *clusterId = cJSON_GetObjectItem(subElement, "clusterId");
					if(clusterId == NULL) {
						cJSON_Delete(root);
						return false;
					}
					cJSON *attributeId = cJSON_GetObjectItem(subElement, "attributeId");
					if(attributeId == NULL) {
						cJSON_Delete(root);
						return false;
					}
					cJSON *attributeBuffer = cJSON_GetObjectItem(subElement, "attributeBuffer");
					if(attributeBuffer == NULL){
						cJSON_Delete(root);
						return false;
					}
					if((strcmp(clusterId->valuestring, "0006") == 0) && (strcmp(attributeId->valuestring, "0000") == 0)){
						strcpy(sub_action->clusterId, clusterId->valuestring);
						strcpy(sub_action->attributeId, attributeId->valuestring);
						if(strcmp(attributeBuffer->valuestring, "00") == 0){
							strcpy(sub_action->attributeBuff, "off");
						}
						if(strcmp(attributeBuffer->valuestring, "01") == 0){
							strcpy(sub_action->attributeBuff, "on");
						}
						flag_data_feild = true;
					}
				}
			}
		}
	}
	cJSON_Delete(root);
	return flag_data_feild;
}

