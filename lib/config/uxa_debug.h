#ifndef UXA_DEBUG_H
#define UXA_DEBUG_H

//#define MODDEBUG

#ifdef MODDEBUG
#define DDD(x) \
  Serial.printf("DDD>: %s::%d %s\n", __FILE__, __LINE__, String(x).c_str());
#else
#define DDD(x) ;
#endif

#ifdef MODDEBUG
#define DDE(x,y) \
  Serial.printf("DDE>: %s::%d %s => %s\n", __FILE__, __LINE__, String(x).c_str(), String(y).c_str());
#else
#define DDE(x,y) ;
#endif



#endif