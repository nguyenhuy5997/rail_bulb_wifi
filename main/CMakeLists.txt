# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c"
					"../main/Pair/QuickMode/SmartConfig.c"
					"../main/Pair/HttpServer/WebServer.c"
					"../main/jsonUser/json_user.c"
					"../main/SPIFFS/spiffs_user.c"
					"../main/WiFi/WiFi_proc.c"
					"../main/Pair/CompatibleMode/AP.c"
					"../main/Mqtt/mqtt.c"
					"../main/heartbeat/heartbeat.c"
					"../main/led_control/led.c"
					"../main/light_driver/iot_led.c"
					"../main/light_driver/light_driver.c"
					"../main/mdf/mdf_err_to_name.c"
					"../main/mdf/mdf_event_loop.c"
					"../main/mdf/mdf_info_store.c"
					"../main/mdf/mdf_mem.c"
					"../main/OTA/fota.c"
					"common.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
