/*
 * File: iGate.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-29 1:12
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include "iGate.h"

#include <Arduino.h>
#include <AsyncTCP.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

void clienttcp(void* c);
void SendToServer(char* value);
void ProcessMessage(char* value);

const char* host = "igates.aprs.fi";
// const char* host = "85.214.27.31";
const uint16_t port = 14580;
const char* call = "DL7UXA";
const uint8_t call_ext = 10;
const char* passcode = "22714";
const char* clientversion = "sLoRaAPRS 0.1-alpha";
const char* filter = "t/poimqstunw";
const float filterlat = 52.5;
const float filterlng = 13.6;
const uint16_t filterrange = 25;
char range_filter[32] = {0};

QueueHandle_t xQueuesend, xQueuereceive;

uint64_t tt = 0;

void setup() {
  snprintf(range_filter, sizeof(range_filter), "r/%02.1f/%03.1f/%d", filterlat,
           filterlng, filterrange);

  // task variable to run on core 0 and controls socket communication
  TaskHandle_t Task1;

  Serial.printf("Erstelle Queues in line %d\n", __LINE__);

  /* Create a queue capable of containing  10 char [50] */
  // void *message = malloc( sizeof(char) * 128 );
  xQueuesend = xQueueCreate(3, sizeof(char) * 128);
  xQueuereceive = xQueueCreate(5, sizeof(char) * 128);

  if (xQueuesend == NULL || xQueuereceive == NULL) {
    Serial.println("\nError creating queues");
  }

  Serial.printf("packe clienttcp auf core 0\n");

  xTaskCreatePinnedToCore(clienttcp, /* Function to implement the task */
                          "Task1",   /* Name of the task */
                          30000,     /* Stack size in words */
                          NULL,      /* Task input parameter */
                          0,         /* Priority of the task */
                          &Task1,    /* Task handle. */
                          0);        /* Core where the task should run */

  Serial.printf("setup ready! in line %d\n", __LINE__);
  delay(1000);
}

void clienttcp(void* c) {
  // asyncronous tcp client
  Serial.println("\n}start clienttcp ...");
  AsyncClient client;

  client.onError([](void* arg, AsyncClient* c, int8_t error) {
    Serial.printf("\n}Error: %s\n\n", c->errorToString(error));
    c->close();
  });

  client.onTimeout([](void* arg, AsyncClient* c, uint32_t time) {
    Serial.printf("\n}Timeout %d\n\n", (uint32_t)time);
  });

  client.onConnect([](void* arg, AsyncClient* c) {
    char authbuf[255] = {0};
    snprintf(authbuf, sizeof(authbuf),
             "user %s-%d pass %s vers %s filter m/25 \r\n", call, call_ext,
             passcode, clientversion);
    Serial.printf("\nConnected! Sending data. %s\n", authbuf);
    c->write(authbuf);
    delay(2000);
    c->write("DL7UXA-10>APRS,TCPIP*:!5229.07N/01334.53E_\r\n");
  });
  client.onData([](void* arg, AsyncClient* c, void* data, size_t len) {
    // Serial.printf("\n}Data received with length: %d\n", len);
    // Serial.printf("\n->data: %s in line %d", (char*)data, __LINE__);

    char subbuff[len];  // NOLINT(runtime/arrays)
    memcpy(subbuff, &(reinterpret_cast<char*>(data))[0], len);

    if (xQueueSend(xQueuereceive, reinterpret_cast<char*>(data),
                   (TickType_t)10) != pdPASS) {
      Serial.printf("\nProblema Problema, %d\n", __LINE__);
    }
  });
  client.connect(host, port);
  while (true) {
    char missatge[255] = {0};
    vTaskDelay(10);
    if (xQueueReceive(xQueuesend, missatge, (TickType_t)10)) {
      Serial.printf("\nDDD:> in clienttcp line %d got from xQueuesend: %s\n",
                    __LINE__, missatge);
      Serial.printf("\nDDD:> in clienttcp line %d sizeof mess: %d\n", __LINE__,
                    sizeof(missatge));
      // strncpy(missatge, "Hallo Test!!\n", 255);
      Serial.printf("\nDDD:> in clienttcp line %d sizeof mess: %d\n", __LINE__,
                    strlen(missatge));
      client.add(missatge, strlen(missatge));
      client.send();
    }
  }

  vTaskDelete(NULL);
}

void loop() {
  // char pos[64] = {0};
  // strncpy(pos, "DL7UXA>APRS,TCPIP*:!5252.37N/1332.29E[\n", 64);

  // char pos[64] = "DL7UXA>APRS,TCPIP*:!5252.37N/1332.29E[\n";

  //   if (tt + 10000 < millis()) {
  //     printf(".");
  //     tt = millis();
  //     Serial.printf("\n\nDDD:> in loop line: %d sending data: %s \n",
  //     __LINE__, pos); SendToServer(pos);
  //     //SendToServer("DB0ABC>APRS,TCPIP*:> Hallo Test");
  //   }

  // vTaskStartScheduler();

  vTaskDelay(10);
  char missatge[255];
  if (xQueueReceive(xQueuereceive, missatge, (TickType_t)10)) {
    // Serial.printf("\nDDD:> in loop line %d got data from xQueuereceive: %s
    // \n", __LINE__, missatge);
    ProcessMessage(missatge);
  }
  // while (true);//client.connected)
}

void SendToServer(char* value) {
  Serial.printf("\nDDD:> in SendToServer line: %d sending %s \n", __LINE__,
                value);
  if (xQueueSend(xQueuesend, reinterpret_cast<char*>(value), (TickType_t)10) !=
      pdPASS) {
    /* Failed to post the message, even after 10 ticks. */
    Serial.printf("unable to send message %s\n",
                  reinterpret_cast<char*>(value));
  }
}

void ProcessMessage(char* value) {
  /**
   * @var		string	Serial.printf(
   *//**
   * @var		mixed	__LINE__
   *//**
   * @var		mixed	value)
   */
  Serial.println(value);
  // int longitud = 0;
  // char* ret = (char*)malloc(sizeof(char) * (50));
  // char* quotPtr = strchr(value, '(');
  // char* quotPtr2 = strchr(value, ')');
  // if (quotPtr != NULL) {
  //   int position = quotPtr - value;
  //   int position2 = quotPtr2 - value;
  //   // select only string inside brakets"()"
  //   if (position2 - position > 1) {
  //     memcpy(ret, value + position + 1, position2 - position - 1);
  //     ret[position2 - position - 1] = '\0';
  //   }
  // }
  // // split string into variable / values arrays
  // char* tok;
  // char delim[] = ",";
  // char* ptr = strtok(ret, delim);

  // const char* var[20];
  // const char* val[20];
  // int counter = 0;
  // bool vari = true;
  // while (ptr != NULL) {
  //   if (vari)
  //     var[counter] = ptr;
  //   else {
  //     val[counter] = ptr;
  //     counter++;
  //   }
  //   vari = !vari;
  //   ptr = strtok(NULL, delim);
  // }
  // // process each variable /value and open or close led
  // for (int i = 0; i < counter; i++) {
  //   int valor, led;
  //   if (strcmp(val[i], "1") == 0) valor = HIGH;
  //   if (strcmp(val[i], "0") == 0) valor = LOW;
  //   if (strcmp(var[i], "L1") == 0) led = led1;
  //   if (strcmp(var[i], "L2") == 0) led = led2;
  //   if (strcmp(var[i], "L3") == 0) led = led3;
  //   if (strcmp(var[i], "L4") == 0) led = led4;
  //   digitalWrite(led, valor);
  //   // Serial.printf("\nvariable: %s valor: %s",var[i], val[i]);
  //   // Serial.printf("\nled: %d valor: %d",led, valor);
  // }
  // //  Serial.printf("\nvariable: %s valor: %s",var[i], val[i]);
}
