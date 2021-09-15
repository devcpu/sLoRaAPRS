/*
 * File: main.cpp
 * Project: attic
 * File Created: 2021-09-15 2:45
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-15 3:14
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2021 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

// /*
//  *  This sketch demonstrates how to scan WiFi networks.
//  *  The API is almost the same as with the WiFi Shield library,
//  *  the most obvious difference being the different file you need to include:
//  */

// #include <FreeRTOS.h>
// #include "AsyncTCP.h"
// #include "WiFi.h"
// #include <Arduino.h>

// void ProcessMessage(char* value);
// void SendToServer(char* value);
// void clienttcp(void* c);

// const char* ssid = "yourssidwifi";
// const char* password = "yourwifipassword";
// const char* host = "192.168.1.4";
// const uint16_t port = 8888;
// const int led1 = 12;
// const int led2 = 14;
// const int led3 = 33;
// const int led4 = 32;
// const int boto1 = 35;
// const int boto2 = 34;
// const int boto3 = 39;
// const int boto4 = 36;
// int buttonState = 0;
// int button1state = LOW;
// int button2state = LOW;
// int button3state = LOW;
// int button4state = LOW;
// QueueHandle_t xQueuesend, xQueuereceive;

// void setup() {
//   pinMode(led1, OUTPUT);
//   pinMode(led2, OUTPUT);
//   pinMode(led3, OUTPUT);
//   pinMode(led4, OUTPUT);
//   pinMode(boto1, INPUT);
//   pinMode(boto2, INPUT);
//   pinMode(boto3, INPUT);
//   pinMode(boto4, INPUT);
//   // task variable to run on core 0 and controls socket communication
//   TaskHandle_t Task1;
//   Serial.begin(115200);

//   // Set WiFi to station mode and disconnect from an AP if it was previously
//   // connected
//   WiFi.mode(WIFI_STA);
//   WiFi.disconnect();
//   delay(100);

//   Serial.println("Setup done");
//   // try to connect to wifi, led 4 blinks
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     digitalWrite(led4, HIGH);
//     delay(200);
//     digitalWrite(led4, LOW);
//     //Serial.print(".");
//     delay(200);
//   }
//   // wifi connection done, now try to connect to tcp/server
//   delay(500);
//   Serial.println("\nConnect done");
//   digitalWrite(led4, HIGH);
//   // create queues for communication between processes

//   /* Create a queue capable of containing  10 char [50] */
//   // void *message = malloc( sizeof(char) * 50 );
//   xQueuesend = xQueueCreate(10, sizeof(char) * 50);
//   xQueuereceive = xQueueCreate(10, sizeof(char) * 50);

//   if (xQueuesend == NULL || xQueuereceive == NULL) {
//     Serial.println("\nError creating queues");
//   }

//   xTaskCreatePinnedToCore(clienttcp, /* Function to implement the task */
//                           "Task1",   /* Name of the task */
//                           10000,     /* Stack size in words */
//                           NULL,      /* Task input parameter */
//                           0,         /* Priority of the task */
//                           &Task1,    /* Task handle. */
//                           0);        /* Core where the task should run */
// }
// void clienttcp(void* c) {
//   // asyncronous tcp client
//   AsyncClient client;

//   client.onError([](void* arg, AsyncClient* c, int8_t error) {
//     // Serial.printf("Error: %s\n\n", c->errorToString(error));
//     digitalWrite(led4, HIGH);
//     c->close();
//   });
//   client.onTimeout([](void* arg, AsyncClient* c, uint32_t time) {
//     //  Serial.printf("Timeout\n\n");
//   });
//   client.onConnect([](void* arg, AsyncClient* c) {
//     // Serial.println("-");
//     //  Serial.printf("Connected\n"); // Sending data.\n");
//     digitalWrite(led4, LOW);
//     // c->write(("?(B1,1)\r" +  String(host)  )
//     //          .c_str());
//   });
//   client.onData([](void* arg, AsyncClient* c, void* data, size_t len) {
//     //  Serial.printf("Data received with length: %d\n", len);

//     // char subbuff[len];
//     // memcpy( subbuff, &((char*)data)[0], len );
//     // Serial.printf("%s", (char*)data);
//     if (xQueueSend(xQueuereceive, (char*)data, (TickType_t)10) != pdPASS) {
//       /* Failed to post the message, even after 10 ticks. */
//     }

//     // c->close();
//   });
//   client.connect(host, port);
//   // Serial.println(xPortGetCoreID());
//   while (true) {
//     char missatge[50];
//     vTaskDelay(10);
//     if (xQueueReceive(xQueuesend, missatge, (TickType_t)10)) {
//       // Serial.println(missatge);
//       client.add(missatge, sizeof(missatge));
//       client.send();
//     }
//   }
// }
// void loop() {
//   char* cadena = "";
//   buttonState = digitalRead(boto1);
//   if (buttonState != button1state) {
//     button1state = buttonState;
//     cadena = "";
//     if (buttonState == LOW)
//       cadena = "?(B1,0)\r\n";
//     else
//       cadena = "?(B1,1)\r\n";

//     SendToServer(cadena);
//   }
//   buttonState = digitalRead(boto2);
//   if (buttonState != button2state) {
//     button2state = buttonState;
//     cadena = "";
//     if (buttonState == LOW)
//       cadena = "?(B2,0)\r\n";
//     else
//       cadena = "?(B2,1)\r\n";

//     SendToServer(cadena);
//   }
//   // this is the offline mode : esay to do
//   // if (buttonState == HIGH) {
//   //     // turn LED on:
//   //     digitalWrite(led1, HIGH);
//   //   } else {
//   //     // turn LED off:
//   //     digitalWrite(led1, LOW);
//   //   }
//   //  buttonState = digitalRead(boto2);
//   // if (buttonState == HIGH) {
//   //     // turn LED on:
//   //     digitalWrite(led2, HIGH);
//   //   } else {
//   //     // turn LED off:
//   //     digitalWrite(led2, LOW);
//   //   }
//   vTaskDelay(10);
//   char missatge[50];
//   if (xQueueReceive(xQueuereceive, missatge, (TickType_t)10)) {
//     // Serial.println(missatge);
//     ProcessMessage(missatge);
//   }
//   // while (true);//client.connected)
// }
// void SendToServer(char* value) {
//   Serial.println(value);
//   if (xQueueSend(xQueuesend, (char*)value, (TickType_t)10) != pdPASS) {
//     /* Failed to post the message, even after 10 ticks. */
//   }
// }
// void ProcessMessage(char* value) {
//   int longitud = 0;
//   char* ret = (char*)malloc(sizeof(char) * (50));
//   char* quotPtr = strchr(value, '(');
//   char* quotPtr2 = strchr(value, ')');
//   if (quotPtr != NULL) {
//     int position = quotPtr - value;
//     int position2 = quotPtr2 - value;
//     // select only string inside brakets"()"
//     if (position2 - position > 1) {
//       memcpy(ret, value + position + 1, position2 - position - 1);
//       ret[position2 - position - 1] = '\0';
//     }
//   }
//   // split string into variable / values arrays
//   char* tok;
//   char delim[] = ",";
//   char* ptr = strtok(ret, delim);

//   const char* var[20];
//   const char* val[20];
//   int counter = 0;
//   bool vari = true;
//   while (ptr != NULL) {
//     if (vari)
//       var[counter] = ptr;
//     else {
//       val[counter] = ptr;
//       counter++;
//     }
//     vari = !vari;
//     ptr = strtok(NULL, delim);
//   }
//   // process each variable /value and open or close led
//   for (int i = 0; i < counter; i++) {
//     int valor, led;
//     if (strcmp(val[i], "1") == 0) valor = HIGH;
//     if (strcmp(val[i], "0") == 0) valor = LOW;
//     if (strcmp(var[i], "L1") == 0) led = led1;
//     if (strcmp(var[i], "L2") == 0) led = led2;
//     if (strcmp(var[i], "L3") == 0) led = led3;
//     if (strcmp(var[i], "L4") == 0) led = led4;
//     digitalWrite(led, valor);
//     // Serial.printf("\nvariable: %s valor: %s",var[i], val[i]);
//     // Serial.printf("\nled: %d valor: %d",led, valor);
//   }
//   //  Serial.printf("\nvariable: %s valor: %s",var[i], val[i]);
// }
