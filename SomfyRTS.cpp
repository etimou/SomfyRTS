// **********************************************************************************
// Somfy RTS Arduino library compatible with generic AM 433.42Mhz transmitter or RFM69
// **********************************************************************************
// Copyright Etienne Mouragnon (2020)
// Based on previous work:
// https://github.com/Nickduino/Somfy_Remote
// https://github.com/kobuki/RFM69OOK 
//
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License as published by the Free Software
// Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General
// Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//
// Licence can be viewed at
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************


#include "SomfyRTS.h"
#include <EEPROM.h>
#include <SPI.h>
#include "RFM69OOK.h"
#include "RFM69OOKregisters.h"

#if defined(ESP8266) || defined(ESP32)
//  #define TRANSMIT_HIGH(pin) (digitalWrite(pin, HIGH))
//  #define TRANSMIT_LOW(pin) (digitalWrite(pin, LOW))
  #define TRANSMIT_HIGH(pin) (GPOS = 1<<pin)
  #define TRANSMIT_LOW(pin) (GPOC = 1<<pin)
#else
  #define TRANSMIT_HIGH(pin) (PORTD |= 1<<pin)
  #define TRANSMIT_LOW(pin) (PORTD &= !(1<<pin))
#endif


RFM69OOK radio;

void SomfyRTS::initRadio() {
  pinMode(_pinTx, OUTPUT);

  if (_transmitterType == TSR_RFM69)
  {

    radio.initialize();
    //radio.setFrequencyMHz(868.88);
    radio.setFrequencyMHz(433.42);
    radio.setPowerLevel(20);
  }
}

void SomfyRTS::configRTS(unsigned int EEPROM_address, unsigned long RTS_address) {
  _EEPROM_address = EEPROM_address;
  _RTS_address = RTS_address;
}

void SomfyRTS::setHighPower(bool onOFF){ //have to call it after initialize for RFM69HW
  radio.setHighPower(onOFF);
}

void SomfyRTS::buildFrameSomfy() {
  uint16_t Code;
  EEPROM.get(_EEPROM_address + 2 * _virtualRemoteNumber, Code);
  frame[0] = 0xA7; // Encryption key. Doesn't matter much
  frame[1] = _actionCommand << 4;  // Which button did  you press? The 4 LSB will be the checksum
  frame[2] = Code >> 8;    // Rolling code (big endian)
  frame[3] = Code;         // Rolling code
  frame[4] = _RTS_address + _virtualRemoteNumber >> 16; // Remote address
  frame[5] = _RTS_address + _virtualRemoteNumber >>  8; // Remote address
  frame[6] = _RTS_address + _virtualRemoteNumber;     // Remote address

  //Serial.print("Frame         : ");
  for (byte i = 0; i < 7; i++) {
    if (frame[i] >> 4 == 0) { //  Displays leading zero in case the most significant
      //Serial.print("0");     // nibble is a 0.
    }
    //Serial.print(frame[i],HEX); Serial.print(" ");
  }

  // Checksum calculation: a XOR of all the nibbles
  byte checksum = 0;
  for (byte i = 0; i < 7; i++) {
    checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
  }
  checksum &= 0b1111; // We keep the last 4 bits only


  //Checksum integration
  frame[1] |= checksum; //  If a XOR of all the nibbles is equal to 0, the blinds will
  // consider the checksum ok.

  //Serial.println(""); Serial.print("With checksum : ");
  for (byte i = 0; i < 7; i++) {
    if (frame[i] >> 4 == 0) {
      //Serial.print("0");
    }
    //Serial.print(frame[i],HEX); Serial.print(" ");
  }


  // Obfuscation: a XOR of all the bytes
  for (byte i = 1; i < 7; i++) {
    frame[i] ^= frame[i - 1];
  }

  //Serial.println(""); Serial.print("Obfuscated    : ");
  for (byte i = 0; i < 7; i++) {
    if (frame[i] >> 4 == 0) {
      //Serial.print("0");
    }
    //Serial.print(frame[i],HEX); Serial.print(" ");
  }
  //Serial.println("");
  //Serial.print("Rolling Code  : "); Serial.println(code);
  EEPROM.put(_EEPROM_address + 2 * _virtualRemoteNumber, ++Code); //  We store the value of the rolling code in the
  // EEPROM. It should take up to 2 adresses but the
  // Arduino function takes care of it.
  #ifdef ESP8266
  EEPROM.commit();
  #endif
}

void SomfyRTS::sendCommandSomfy(byte sync) {
  if (sync == 2) { // Only with the first frame.
    //Wake-up pulse & Silence
    TRANSMIT_HIGH(_pinTx);
    delayMicroseconds(9415);
    TRANSMIT_LOW(_pinTx);
    //delayMicroseconds(89565U);
    delay(89);
  }

  // Hardware sync: two sync for the first frame, seven for the following ones.
  for (int i = 0; i < sync; i++) {
    TRANSMIT_HIGH(_pinTx);
    delayMicroseconds(4 * SYMBOL);
    TRANSMIT_LOW(_pinTx);
    delayMicroseconds(4 * SYMBOL);
  }

  // Software sync
  TRANSMIT_HIGH(_pinTx);
  delayMicroseconds(4550);
  TRANSMIT_LOW(_pinTx);
  delayMicroseconds(SYMBOL);


  //Data: bits are sent one by one, starting with the MSB.
  for (byte i = 0; i < 56; i++) {
    if (((frame[i / 8] >> (7 - (i % 8))) & 1) == 1) {
      TRANSMIT_LOW(_pinTx);
      delayMicroseconds(SYMBOL);
      TRANSMIT_HIGH(_pinTx);
      delayMicroseconds(SYMBOL);
    }
    else {
      TRANSMIT_HIGH(_pinTx);
      delayMicroseconds(SYMBOL);
      TRANSMIT_LOW(_pinTx);
      delayMicroseconds(SYMBOL);
    }
  }

  TRANSMIT_LOW(_pinTx);
  delayMicroseconds(30415); // Inter-frame silence
}

void SomfyRTS::sendSomfy(unsigned char virtualRemoteNumber, unsigned char actionCommand) {
  _virtualRemoteNumber = virtualRemoteNumber;
  _actionCommand = actionCommand;

  if (_transmitterType == TSR_RFM69) {
    radio.transmitBegin(); // Emitter in TX mode
  }

  buildFrameSomfy();
  noInterrupts();
  sendCommandSomfy(2);
  for (int i = 0; i < 2; i++) {
    sendCommandSomfy(7);
  }
  interrupts();

  if (_transmitterType == TSR_RFM69) {
    radio.transmitEnd(); // Emitter in standby mode
  }
}

