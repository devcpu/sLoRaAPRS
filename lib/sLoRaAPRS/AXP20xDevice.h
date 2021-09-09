/*
 * File: AXP20xDevice.h
 * Project: sLoRaAPRS
 * File Created: 2021-03-29 0:39
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-09 23:19
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_AXP20XDEVICE_H_
#define LIB_SLORAAPRS_AXP20XDEVICE_H_

#include "LoRaAPRSConfig.h"

#ifdef T_BEAM_V1_0

#include <axp20x.h>

AXP20X_Class axp;

bool initAXP() {
  if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) { // "!" is correct here
    axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
    axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
    axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
    axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
    axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
    axp.setDCDC1Voltage(3300);
    return true;
  } else {
    return false;
  }
}

#endif  // T_BEAM_V1_0

#endif  // LIB_SLORAAPRS_AXP20XDEVICE_H_
