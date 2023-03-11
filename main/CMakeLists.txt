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
					"../main/iot_led.c"
					"../main/light_driver.c"
					"../main/mdf_err_to_name.c"
					"../main/mdf_event_loop.c"
					"../main/mdf_info_store.c"
					"../main/mdf_mem.c"
					"common.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
