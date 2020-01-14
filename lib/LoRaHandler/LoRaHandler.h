#ifndef LORA_HANDLER_H
#define LORA_HANDLER_H

#include <LoRa.h>
#include <LoRaAPRSConfig.h>

struct LoRaRXControl {
  uint64_t msg_wait = 0;
  boolean isSend = false;
};

extern LoRaRXControl lora_control;


bool initLoRa(void);
void sendMessage(char* outgoing, boolean toDigi);
void onReceive(int packetSize);
void LoRa_tick(void);


#endif