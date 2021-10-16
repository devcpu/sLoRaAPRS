/*
 * File: LoRaHandler.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-16 4:08
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <LoRaHandler.h>
#include <TrackerDisplay.h>

bool LoRaHandler::begin() {
  LoRa.setPins(LoRaCsPin, LoRaResetPin, LoRaIRQPin);
  if (!LoRa.begin(LoRaRXFREQ)) {
    ESP_LOGE(TAG, "Starting LoRa failed!");
    return false;
  }
  LoRa.setSpreadingFactor(LoRaSpreadingFactor);
  LoRa.setSignalBandwidth(LoRaSignalBandwidth);
  LoRa.setCodingRate4(LoRaCodingRate4);
  if (LoRaenableCrc) {
    LoRa.enableCrc();
  }
  LoRa.setTxPower(LoRaTXdbmW);
  LoRa.enableInvertIQ();
  LoRa.onReceive(onReceive);
  setReciveMode();
  return true;
}

void LoRaHandler::tick(void) {
  if (isMessage > 0) {
    processMessage();
    isMessage = 0;
  }
  if (!isSend) {
    return;
  }
  if (msg_wait > millis()) {
    return;
  }
  setReciveMode();
}

void LoRaHandler::setReciveMode() {
  ESP_LOGD(TAG, "set LoRa device to recive");
  LoRa.setFrequency(LoRaRXFREQ);
  LoRa.enableInvertIQ();
  LoRa.receive();
  isSend = false;
  td.writeUTC();
}

void LoRaHandler::sendMessage(char *outgoing, boolean toDigi) {
  char txmsgbuf[16] = {0};
  Serial.printf("sendMessage '%s'\n", outgoing);
  // 850 ms init plus header 35 / char both with spare
  msg_wait = strlen(outgoing) * 35 + millis() + 850; // time for ~ one char to send
  isSend = true;
  static uint64_t msgCount;
  if (toDigi) {
    LoRa.setFrequency(LoRaTXFREQ);
    LoRa.disableInvertIQ();
    strncpy(txmsgbuf, "  2digi", sizeof(txmsgbuf) - 1);
  } else {
    LoRa.setFrequency(LoRaRXFREQ);
    LoRa.enableInvertIQ();
    strncpy(txmsgbuf, " 2tracker", sizeof(txmsgbuf) - 1);
  }
  td.writeTX(txmsgbuf);

  char destination = 0x3C;  // '<' it seems that we have to use it, but really?
  char localAddress = 0xFF; // in LoRa it stands for broadcat
  LoRa.beginPacket();       // start packet returns false on error
  LoRa.write(destination);  // add destination address
  LoRa.write(localAddress); // add sender address
  LoRa.write(0x01);         // head end?
  // LoRa.write(msgCount);                 // add message ID

  /*
    LoRa.print adds some cryptic char to head,
    differnt textes trigger different chars,
    so we write char by char
  */
  for (uint8_t i = 0; i < strlen(outgoing); i++) {
    LoRa.write(outgoing[i]);
  }

  // LoRa.write(strlen(out));        // add payload length
  // LoRa.print(out);                 // add payload
  LoRa.endPacket(true); // async mod
  // LoRa.endPacket(false);                     // finish packet and send it
  // syncron blocking!
  msgCount++;
  // increment message ID
  // LoRa.receive();
}

void onReceive(int packetSize) {
  lora_handler.isMessage = packetSize;
  ESP_LOGD(TAG, "fire onReceive");
}

///////////////////////////////////////////////////////////////////////////////////////////////

void LoRaHandler::processMessage(void) {
  // if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();      // recipient address
  byte sender = LoRa.read();        // sender address
  byte incomingMsgId = LoRa.read(); // incoming msg ID
  // byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = ""; // payload of packet

  while (LoRa.available()) {                    // can't use readString() in callback, so
    incoming += static_cast<char>(LoRa.read()); // add bytes one by one
  }

  // if (incomingLength != incoming.length()) {   // check length for error
  //   Serial.println("error: message length does not match length");
  //   return;                             // skip rest of function
  // }
  Serial.println("/////////////////////////////////////////////////////////////////////");
  // Serial.printf("incomingLength=%d\n", incomingLength);
  Serial.printf("incoming.length():%d\n", incoming.length());

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  // Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println("/////////////////////////////////////////////////////////////////////");
  Serial.println();
}

void lorahandler_tickCB(void) { lora_handler.tick(); }

/*
  zerlege msg from/datum/uhrzeit/nachricht
  2json
  generate filename
  open file
  write content
  close file
  schreibe text zum client wenn connected via socket
*/

// Serial.println("Received from: 0x" + String(sender, HEX));
// Serial.println("Sent to: 0x" + String(recipient, HEX));
// Serial.println("Message ID: " + String(incomingMsgId));
// Serial.println("Message length: " + String(incomingLength));
// Serial.println("Message: " + incoming);
// Serial.println("RSSI: " + String(LoRa.packetRssi()));
