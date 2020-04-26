# Somfy RTS

## An Arduino Library to emulate several Somfy RTS remote controllers.

If you want to learn more about the Somfy RTS protocol, check out [Pushtack](https://pushstack.wordpress.com/somfy-rts-protocol/).

**How the hardware works:** You will need:
- An Arduino
- A 433.42 Mhz RF transmitter. Generally you can't find a 433.42 MHz transmitter, so you need to buy a 433.92 MHz transmitter and replace its crystal by a 433.42 MHz crystal.

  **OR**
- A RFM69(H)(C)(W) transceiver 

**Note:** the RFM69 module only accepts 3.3V power supply voltage. Be careful not to power it with 5V.

Arduino Uno pin              | RFM69 pin   | AM transmitter
---------------------------- | ---------   | -------------
10                           | NSS         | 
11                           | MOSI        | 
12                           | MISO        |              
13                           | SCK         |     
3 (user defined)             | DIO2        | Data      
                             | 3V3(power+) | VCC (power+)             
                             | GND (GND)   | GND (GND                 

**How the software works: ** 

Install first the library in your *libraries* folder.
Then you can simply create your own set of remotes as follows:

`SomfyRTS myRTS(3, TSR_RFM69);`
where *3* is the pin you want to use to drive the transmitter and *TSR_RFM69* is the transmitter type. The other choice is *TSR_AM*.

You are now ready to send commands to your Somfy RTS device. Example:
`myRTS.sendSomfy(0, DOWN);`
 
This emulates a press on the *DOWN* button on the remote controller 0. You can theoritically emukate up to 256 remotes.
Of course, the first command you need to send is *PROG* to pair your remote with your device. Make sure your device is in pairing mode when doing this.
`myRTS.sendSomfy(0, PROG);`

The rolling code values are stored in the EEPROM, so that you don't loose count of your rolling code after a reset.






