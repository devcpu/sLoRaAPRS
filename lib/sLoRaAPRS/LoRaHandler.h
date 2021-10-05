/*
 * File: LoRaHandler.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-29 1:18
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_LORAHANDLER_H_
#define LIB_SLORAAPRS_LORAHANDLER_H_

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
void sendMessage(char *outgoing, boolean toDigi);
void onReceive(int packetSize);
void LoRa_tick(void);
void processMessage(void);
void reciveMessages(void);

#endif // LIB_SLORAAPRS_LORAHANDLER_H_
