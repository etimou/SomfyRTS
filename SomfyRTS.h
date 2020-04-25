#ifndef SOMFY_RTS_H
#define SOMFY_RTS_H
#include <Arduino.h>

#define SYMBOL 640
#define UP 0x2
#define STOP 0x1
#define DOWN 0x4
#define PROG 0x8

#define TSR_RFM69 1
#define TSR_AM 2

class SomfyRTS {
  public:
    void initRadio();
    void sendSomfy(unsigned char virtualRemoteNumber, unsigned char actionCommand);
    void configRTS(unsigned int EEPROM_address, unsigned long RTS_address);
    void setHighPower(bool onOFF=true); //have to call it after initialize for RFM69HW

    SomfyRTS(byte pinTx, unsigned char transmitterType) {

      _pinTx = pinTx;
      _EEPROM_address = 0;
      _RTS_address = 0x121300;
      _actionCommand = STOP;
      _virtualRemoteNumber = 0;
      _transmitterType = transmitterType;

      initRadio();
    }

  protected:
    void sendCommandSomfy(byte sync);
    void buildFrameSomfy();


    byte _pinTx;
    unsigned int _EEPROM_address;
    unsigned long _RTS_address;
    unsigned char _actionCommand;
    unsigned char _virtualRemoteNumber;
    byte frame[7]; // frame for Somfy protocol
    unsigned char _transmitterType;

};

#endif
