// This sketch will pair a Somfy device with a virtual remote controller 

#include <SomfyRTS.h>

SomfyRTS myRTS(3, TSR_RFM69); //Tx pin number, transmitter type
                              //pin number : pin connected to the transmitter DATA pin or to the DIO2 pin on RFM69
                              //transmitter type can be TSR_RFM69 or TSR_AM (for a generic AM 433.42MHZ transmitter)

void setup() {

  Serial.begin(115200);
  //myRTS.configRTS(myEEPROM_address, myRTS_address); //uncomment and change values only if you don't want to use default configuration
  myRTS.setHighPower(true); //have to call it after initialize for RFM69HW
  
  Serial.println("Enter something in the terminal to pair your device");

  //wait until something is received on Serial
  while (!Serial.available()){}
  Serial.print("Pairing...");
  myRTS.sendSomfy(0, PROG);
  Serial.println("Done");

 
}

void loop() {


}
