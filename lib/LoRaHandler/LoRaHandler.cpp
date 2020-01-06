#include <LoRaHandler.h>

void initLoRa() {
  LoRa.setPins(LoRaCsPin, LoRaResetPin, LoRaIRQPin);
  if (!LoRa.begin(LoRaTXFREQ)) {
    Serial.println("Starting LoRa failed!");
  }
  LoRa.setSpreadingFactor(LoRaSpreadingFactor);
  LoRa.setSignalBandwidth(LoRaSignalBandwidth);
  LoRa.setCodingRate4(LoRaCodingRate4);
  if (LoRaenableCrc) {
    LoRa.enableCrc();
  }
  LoRa.setTxPower(LoRaTXdbmW);
  Serial.println("Init LoRa ready!");

}


void sendMessage(char* outgoing) {
  Serial.printf("sendMessage '%s'\n", outgoing);
  static uint64_t msgCount;
  char destination = 0x3C; // '<' it seems that we have to use it, but really?
  char localAddress = 0xFF; // in LoRa it stands for broadcat 
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(0x01);                     // head end?
  //LoRa.write(msgCount);                 // add message ID

  /* 
    LoRa.print adds some cryptic char to head, 
    differnt textse trigger different chars,
    so we write char by char
  */
  for (uint8_t i=0; i<strlen(outgoing); i++) { 
    LoRa.write(outgoing[i]);
  }

  // LoRa.write(strlen(out));        // add payload length
  // LoRa.print(out);                 // add payload
  LoRa.endPacket(true);                     // finish packet and send it
  msgCount++;                           // increment message ID
}

