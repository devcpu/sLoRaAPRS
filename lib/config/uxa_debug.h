/*
 * File: uxa_debug.h
 * Project: config
 * File Created: 2020-09-26 21:08
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-27 23:55
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_CONFIG_UXA_DEBUG_H_
#define LIB_CONFIG_UXA_DEBUG_H_

#define MODDEBUG

#ifdef MODDEBUG
#define DDD(x) \
  Serial.printf("DDD>: %s::%d %s\n", __FILE__, __LINE__, String(x).c_str());
#else
#define DDD(x);
#endif

#ifdef MODDEBUG
#define DDE(x, y)                                              \
  Serial.printf("DDE>: %s::%d %s => %s\n", __FILE__, __LINE__, \
                String(x).c_str(), String(y).c_str());
#else
#define DDE(x, y);
#endif

#endif  // LIB_CONFIG_UXA_DEBUG_H_
