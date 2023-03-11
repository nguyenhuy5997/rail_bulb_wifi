/*
 * led.h
 *
 *  Created on: Mar 6, 2023
 *      Author: nguyenphuonglinh
 */

#ifndef MAIN_LED_CONTROL_LED_H_
#define MAIN_LED_CONTROL_LED_H_
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE

#define LEDC_LS_CH0_GPIO       (18)
#define LEDC_LS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_LS_CH1_GPIO       (19)
#define LEDC_LS_CH1_CHANNEL    LEDC_CHANNEL_1
#define LEDC_LS_CH2_GPIO       (3)
#define LEDC_LS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_LS_CH3_GPIO       (4)
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3
#define LEDC_LS_CH4_GPIO       (5)
#define LEDC_LS_CH4_CHANNEL    LEDC_CHANNEL_4

#define LEDC_TEST_CH_NUM       (5)
#define LEDC_TEST_DUTY         (pow(2,13))
#define LEDC_TEST_FADE_TIME    (1000)

typedef enum {
	CTT,
	RGB
} light_mode;
typedef struct {
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint16_t Dim;
	uint16_t CCT;
	light_mode mode;
} light_control;

void init_led(void * arg);

#endif /* MAIN_LED_CONTROL_LED_H_ */
