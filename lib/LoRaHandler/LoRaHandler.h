#ifndef LORA_HANDLER_H
#define LORA_HANDLER_H

#include <LoRa.h>
#include <LoRaAPRSConfig.h>

bool initLoRa(void);
void sendMessage(char* outgoing);

#endif