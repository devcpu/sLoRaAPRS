#ifndef LORA_HANDLER_H
#define LORA_HANDLER_H

#include <LoRa.h>
#include <LoRaAPRSConfig.h>

void initLoRa();
void sendMessage(char* outgoing);

#endif