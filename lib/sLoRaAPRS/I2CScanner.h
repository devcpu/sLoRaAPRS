/**
* I2CScanner.h
* @author Johannes Arlt
* @description 
* @created 2021-03-07T17:18:22.145Z+01:00
* @copyright © 2021 Johannes Arlt
* @last-modified 2021-03-07T17:58:16.051Z+01:00
*/

#ifndef I2CSANNER_H
#define I2CSANNER_H

#include <Arduino.h>
#include <ArduinoSTL.h>
#include <Wire.h>

class I2CScanner 
{
  public:
    std::vector<byte> devices;  
    byte device_count = 0;
    bool scannend = false;
    void I2CScanner::scan();
    bool I2CScanner::initialize();
    
        
};


#endif
