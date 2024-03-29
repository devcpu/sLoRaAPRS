/*
 * File: freertos0.cpp
 * Project: test
 * File Created: 2021-09-15 2:45
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-15 3:13
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2021 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <Arduino_FreeRTOS.h>

// define two tasks for Blink & AnalogRead
void TaskBlink(void *pvParameters);
void TaskAnalogRead(void *pvParameters);

// the setup function runs once when you press reset or power the board
void setup() {
  // Now set up two tasks to run independently.
  xTaskCreate(TaskBlink, (const portCHAR *)"Blink"  // A name just for humans
              ,
              128  // Stack size
              ,
              NULL, 2  // priority
              ,
              NULL);

  xTaskCreate(
      TaskAnalogRead, (const portCHAR *)"AnalogRead",
      128  // This stack size can be checked & adjusted by reading Highwater
      ,
      NULL, 1  // priority
      ,
      NULL);

  // Now the task scheduler, which takes over control of scheduling individual
  // tasks, is automatically started.
}

void loop() {
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters) {  // This is a task.
  (void)pvParameters;

  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);

  for (;;) {                 // A Task shall never return or exit.
    digitalWrite(13, HIGH);  // turn the LED on (HIGH is the voltage level)
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // wait for one second
    digitalWrite(13, LOW);  // turn the LED off by making the voltage LOW
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // wait for one second
  }
}

void TaskAnalogRead(void *pvParameters) {  // This is a task.
  (void)pvParameters;

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  for (;;) {
    // read the input on analog pin 0:
    int sensorValue = analogRead(A0);
    // print out the value you read:
    Serial.println(sensorValue);
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}
