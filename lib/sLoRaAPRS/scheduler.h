/*
 * File: scheduler.h
 * Project: sLoRaAPRS
 * File Created: 2021-09-15 1:16
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-15 2:32
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt 
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_SCHEDULER_H_
#define LIB_SLORAAPRS

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include <APRSWebServer.h>
#include <xOneButton.h>
#include <LoRaAPRSConfig.h>

/**
 * @brief xTask callback function for default button tick
 * 
 * @param pvParameters optional 
 */
void xbutton_tick(void *pvParameters);


/**
 * @brief xTask callback function for webserver tick
 * 
 * @param pvParameters optional
 */
void xws_tick(void *pvParameters);

void DisplayTimerCB(TimerHandle_t pxTimer);


void scheduler_init();

#endif /* LIB_SLORAAPRS_SCHEDULER_H_ */