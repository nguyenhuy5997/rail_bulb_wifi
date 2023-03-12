/*
 * board.h
 *
 *  Created on: Mar 12, 2023
 *      Author: admin
 */

#ifndef MAIN_BOARD_H_
#define MAIN_BOARD_H_


#define DEV_KIT		1
#define RAIL		0

#if DEV_KIT
	#define CONFIG_LIGHT_GPIO_RED				3
	#define CONFIG_LIGHT_GPIO_GREEN				4
	#define CONFIG_LIGHT_GPIO_BLUE				5
	#define CONFIG_LIGHT_GPIO_COLD     			18
	#define CONFIG_LIGHT_GPIO_WARM				19
	#define CONFIG_LIGHT_FADE_PERIOD_MS 		(1000)
	#define CONFIG_LIGHT_BLINK_PERIOD_MS		(500)
	#define BRIGHTNESS_MIN	0x0000
	#define BRIGHTNESS_MAX	0xffff
	#define TEMPERATURE_MIN	0x0320 // ~0
	#define TEMPERATURE_MAX	0x4e20 // ~100
	#define URL_OTA 			"http://202.191.56.104:5551/uploads/devkit_wifi_bulb.bin"
#elif RAIL
	#define CONFIG_LIGHT_GPIO_RED				3
	#define CONFIG_LIGHT_GPIO_GREEN				4
	#define CONFIG_LIGHT_GPIO_BLUE				5
	#define CONFIG_LIGHT_GPIO_COLD     			18
	#define CONFIG_LIGHT_GPIO_WARM				19
	#define CONFIG_LIGHT_FADE_PERIOD_MS 		(1000)
	#define CONFIG_LIGHT_BLINK_PERIOD_MS		(500)
	#define BRIGHTNESS_MIN	0x0000
	#define BRIGHTNESS_MAX	0xffff
	#define TEMPERATURE_MIN	0x0320 // ~0
	#define TEMPERATURE_MAX	0x4e20 // ~100
	#define URL_OTA 			"http://202.191.56.104:5551/uploads/rail_wifi_bulb.bin"
#endif


#endif /* MAIN_BOARD_H_ */
