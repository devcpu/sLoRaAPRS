#include <EEPROM.h>
#include <LoRaAPRSConfig.h>
#include <Registry.h>

Registry reg;

void RegistryInit(void) {
  
  EEPROM.begin(EEPROM_SIZE);
  String systemstring = EEPROMreadString(EEPROM_ADDR_SYSTEM);
  if (systemstring != String(SYSTEM_STRING)) { // fist run!
    EEPROMwriteString(EEPROM_ADDR_SYSTEM, String(SYSTEM_STRING));
    
    EEPROM.write(EEPROM_ADDR_BOOT_COUNT, 0);
    EEPROM.commit();
    EEPROM.write(EEPROM_ADDR_CURRENT_WIFI_MODE, int(wifi_ap));
    EEPROM.commit();
    EEPROM.write(EEPROM_ADDR_CURRENT_SYSTEM_MODE, int(mode_tracker));
    EEPROM.commit();
    EEPROM.write(EEPROM_ADDR_APRS_SYMBOL, '[');
    EEPROM.commit();
  }
  
  uint32_t tmpi = EEPROM.read(EEPROM_ADDR_BOOT_COUNT);
  Serial.printf("Found %i for boot_count  in EEPROM@%d\n", tmpi,
                EEPROM_ADDR_BOOT_COUNT);
  reg.boot_count = tmpi + 1;
  EEPROM.write(EEPROM_ADDR_BOOT_COUNT, reg.boot_count);
  EEPROM.commit();

  reg.current_wifi_mode = (wifi_mode) EEPROM.read(EEPROM_ADDR_CURRENT_WIFI_MODE);
  Serial.printf("Found %d for current_wifi_mode in EEPROM@%d\n", reg.current_wifi_mode, 
                EEPROM_ADDR_CURRENT_WIFI_MODE);
 

  reg.current_system_mode = (system_mode) EEPROM.read(EEPROM_ADDR_CURRENT_SYSTEM_MODE);
  Serial.printf("Found %d for current_system_mode in EEPROM@%d\n", reg.current_system_mode,
                EEPROM_ADDR_CURRENT_SYSTEM_MODE);

  reg.aprs_symbol = EEPROM.read(EEPROM_ADDR_APRS_SYMBOL);
  Serial.printf("Found %c for aprs_symbol in EEPROM@%d\n", reg.aprs_symbol,
                EEPROM_ADDR_APRS_SYMBOL);

  //char buf[32] = {0};
  reg.Call = EEPROMrestoreString(EEPROM_ADDR_CALL, "CHANGEME", "call");
  if (reg.Call == "CHANGEME") {
    reg.current_wifi_mode = wifi_ap;
    reg.current_system_mode = mode_tracker;
  }
  reg.wx_call_ext = EEPROMrestoreString(EEPROM_ADDR_WX_EXT, "3", "wx_call_ex");
  reg.aprs_call_ext =
      EEPROMrestoreString(EEPROM_ADDR_APRS_EXT, "12", "aprs_call_ext");

  //reg.Call.toCharArray(buf, 32);
  reg.WebCredentials[0] = EEPROMrestoreString(
      EEPROM_ADDR_WEB_CREDENTIAL00, "admin", "WebCredentials Username");
  reg.WebCredentials[1] = EEPROMrestoreString(
      EEPROM_ADDR_WEB_CREDENTIAL01, "letmein4711", "WebCredentials Password");

  reg.APCredentials[0] = EEPROMrestoreString(EEPROM_ADDR_AP_CREDENTIAL00,
                                             "sLoRaAPRS", "APCredentials SSID");
  reg.APCredentials[1] = EEPROMrestoreString(
      EEPROM_ADDR_AP_CREDENTIAL01, "letmein4711", "APCredentials Password");

  reg.APRSPassword =
      EEPROMrestoreString(EEPROM_ADDR_APRS_PASSWORD, "4711", "APRSPassword");

  for (uint8_t x = 0; x < 2; x++) {
    reg.APRSServer[x] = EEPROMrestoreString(EEPROM_ADDR_APRS_SERVER + x * 20,
                                            "192.168.0.1", "APRSServer");
  }

  Serial.println("++++++++++++");
  char namebuf[42] = {0};
  char bufx[23] = {0};
  char defbuf[23] = {0};
  for (uint8_t x = 0; x < 4; x++) {
    for (uint8_t y = 0; y < 2; y++) {
      uint16_t address = 0;
      if (y % 2 == 0) {
        address = int(EEPROM_ADDR_WIFI_CREDENTIAL00 +
                      x * (EEPROM_USER_LENGTH + EEPROM_PASSWORD_LENGTH));
        snprintf(bufx, 23, "WLAN_SSID%d", x);
        snprintf(defbuf, 23, "%s%d", "WLAN_SSID", x);
      } else {
        address = int(EEPROM_ADDR_WIFI_CREDENTIAL00 +
                      x * (EEPROM_USER_LENGTH + EEPROM_PASSWORD_LENGTH) +
                      EEPROM_USER_LENGTH);
        snprintf(bufx, 23, "WLAN_PASSWD%d", x);
        snprintf(defbuf, 23, "%s%d", "changeme", x);
      }
      snprintf(namebuf, 23, "[%d][%d] %s", x, y, bufx);

      reg.WifiCrendentials[x][y] =
          EEPROMrestoreString(address, defbuf, namebuf);
    }
  }
  Serial.println("++++++++++++");
  Serial.println("Registry init ready\n");
}

String EEPROMrestoreString(uint16_t addr, const char* def, const char* name) {
  String tmp = EEPROMreadString(addr);
  char buf[32];
  tmp.toCharArray(buf, 32);

  if (buf[0] == 0xff || buf[0] == 0)  // first usage of EEPROM
  {
    Serial.printf("No data for %s in EEPROM@%d! Use default %s\n", name, addr,
                  def);
    tmp = def;
    EEPROMwriteString(addr, tmp);
  } else {
    Serial.printf("Found %s for %s in EEPROM@%d\n", buf, name, addr);
  }
  return tmp;
}

void EEPROMwriteString(uint16_t addr, String data) {
  String got = EEPROMreadString(addr);
  if (got == data) {
    Serial.printf("data == EEPROM@%d, no uptate\n", addr);
    return;
  }

  char buf[32] = {0};
  data.toCharArray(buf, 32);
  Serial.printf("EEPROM.write %s to addr:%d\n", buf, addr);
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++) {
    EEPROM.write(addr + i, data[i]);
  }
  EEPROM.write(addr + _size,
               '\0');  // Add termination null character for String Data
  EEPROM.commit();
  delay(200);
}

String EEPROMreadString(uint16_t addr) {
  char data[32];  // Max 32 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(addr);
  while (k != '\0' && len < 30)  // Read until null character
  {
    k = EEPROM.read(addr + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

/**
 * @brief for programmer use only!
 * You have to know what you do!
 */
void EEPROMClear(void) {
  EEPROM.begin(EEPROM_SIZE);

  Serial.println("++++++++++++++++++++++++++++++++");
  Serial.println("WARNIG WARNIG! Will clear EEPROM");
  Serial.println("++++++++++++++++++++++++++++++++");

  for (uint16_t i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0xFF);
    Serial.print("x");
    if (i % 32 == 0 && i != 0) {
      Serial.println(i);
      EEPROM.commit();
      delay(300);
    }
  }
  EEPROM.commit();
  Serial.println("\nEEPROM clear!");
}

void dumpEEPROM(void) {
  char buf0[17] = {0};
  char bufout[128] = {0};
  String str;
  uint8_t b = 0;
  for (uint16_t a = 0; a < EEPROM_SIZE; a += 16) {
    for (b = 0; b < 16; b++) {
      buf0[b] = EEPROMreadString(a + b).charAt(0);
    }
    snprintf(
        bufout, 128,
        "%03d | %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x "
        "%02x %02x %02x %02x | %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c",
        a, buf0[0], buf0[1], buf0[2], buf0[3], buf0[4], buf0[5], buf0[6],
        buf0[7], buf0[8], buf0[9], buf0[10], buf0[11], buf0[12], buf0[13],
        buf0[14], buf0[15],

        (char)buf0[0], (char)buf0[1], (char)buf0[2], (char)buf0[3],
        (char)buf0[4], (char)buf0[5], (char)buf0[6], (char)buf0[7],
        (char)buf0[8], (char)buf0[9], (char)buf0[10], (char)buf0[11],
        (char)buf0[12], (char)buf0[13], (char)buf0[14], (char)buf0[15]);
    Serial.println(bufout);
    delay(100);
  }
};