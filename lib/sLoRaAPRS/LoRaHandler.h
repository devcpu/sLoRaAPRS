/*
 * File: LoRaHandler.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-10 21:35
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

// https://wiki.oevsv.at/wiki/Kategorie:HAM-IoT
// QRG TX:    433,775 MHz
// QRG RX:    433,900 MHz
// BW:        125K
// SF:        12
// CR:        4/5
// Sync Word: 18 (0x12)
// Lora Explizit Modus

// Protokoll Beschreibung OE-Lora Format:
// 3 Byte Header "<\xFF\x01" + APRS Protokoll (Header: < = Typ, 255=Destination, 1=Source)
// [7] http://www.aprs.org/doc/APRS101.PDF Seite 36
// Chapter 9: Compressed Position Report Data Formats

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

class LoRaHandler {
  public:
  
  bool begin(void);
  void sendMessage(char *outgoing, boolean toDigi);
  void tick(void);
  void processMessage(void);


  void setReciveMode(void);
  
  volatile uint64_t msg_wait = 0; // time in millis() for wait until msg is send

  volatile boolean isSend = false; // true if LoRa is still sendig
  
  volatile int isMessage = 0; // > 0 if there is a message arrived
};

void onReceive(int packetSize);







#endif // LIB_SLORAAPRS_LORAHANDLER_H_
