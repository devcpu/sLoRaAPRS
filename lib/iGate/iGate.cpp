#include <Arduino.h>
#include <iGate.h>
#include <AsyncUDP.h>
#include <AsyncTCP.h>
#include <string.h>

AsyncUDP udp;
AsyncClient aTCPClient;

void iGate_udp_connect(void) {
  Serial.printf("Try to connect udp ...\n");
  IPAddress ip = IPAddress(18,197,125,125);
  while ((udp.connect(ip, 8080)) == 0) {
    Serial.printf("ERROR: can't connected (UDP)\n");
    delay(1000);
  }
}



void iGate_process_udp(void) {
    udp.onPacket([](AsyncUDPPacket packet) {
      Serial.print("UDP Packet Type: ");
      Serial.print(packet.isBroadcast()
                       ? "Broadcast"
                       : packet.isMulticast() ? "Multicast" : "Unicast");
      Serial.print(", From: ");
      Serial.print(packet.remoteIP());
      Serial.print(":");
      Serial.print(packet.remotePort());
      Serial.print(", To: ");
      Serial.print(packet.localIP());
      Serial.print(":");
      Serial.print(packet.localPort());
      Serial.print(", Length: ");
      Serial.print(packet.length());
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
    });


}

