#ifndef LORA_HANDLER_H
#define LORA_HANDLER_H

#include <LoRa.h>
#include <LoRaAPRSConfig.h>

#ifndef LoRaRXFREQ
#define LoRaRXFREQ 433900000L
#endif

#ifndef LoRaRXFREQ
#define LoRaRXFREQ 433775000L
#endif

#ifndef LoRaSpreadingFactor
#define LoRaSpreadingFactor 12
#endif

#ifndef LoRaSignalBandwidth
#define LoRaSignalBandwidth 125000
#endif

#ifndef LoRaCodingRate4
#define LoRaCodingRate4 5
#endif

#ifndef LoRaenableCrc
#define LoRaenableCrc true
#endif

#ifndef LoRaTXdbmW
#define LoRaTXdbmW 18
#endif

#ifndef LoRaCsPin
#define LoRaCsPin 18
#endif

#ifndef LoRaResetPin
#define LoRaResetPin 23
#endif

#ifndef LoRaIRQPin
#define LoRaIRQPin 26
#endif

struct LoRaRXControl {
  volatile uint64_t msg_wait = 0;
  volatile boolean isSend = false;
  volatile int isMessage = 0;
};


extern LoRaRXControl lora_control;


bool LoRa_init(void);
void sendMessage(char* outgoing, boolean toDigi);
void onReceive(int packetSize);
void LoRa_tick(void);
void processMessage(void);
void reciveMessages(void);

#endif