// This sketch will cycle one window blind every minute

#include <SomfyRTS.h>

SomfyRTS myRTS(3, TSR_RFM69); //Tx pin number, transmitter type
                              //pin number : pin connected to the transmitter DATA pin or to the DIO2 pin on RFM69
                              //transmitter type can be TSR_RFM69 or TSR_AM (for a generic AM 433.42MHZ transmitter)

void setup() {

  //myRTS.configRTS(myEEPROM_address, myRTS_address); //uncomment and change values only if you don't want to use default configuration
  myRTS.setHighPower(true); //have to call it after initialize for RFM69HW
  
}

void loop() {

  myRTS.sendSomfy(0, DOWN); // remote 0, down
  delay(60000);
  myRTS.sendSomfy(0, UP); // remote 0, up
  delay(60000);

}
