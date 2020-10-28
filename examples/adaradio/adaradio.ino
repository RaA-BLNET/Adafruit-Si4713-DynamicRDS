/*************************************************** 
  This is an example for the Si4713 FM Radio Transmitter with RDS

  Designed specifically to work with the Si4713 breakout in the
  adafruit shop
  ----> https://www.adafruit.com/products/1958

  These transmitters use I2C to communicate, plus reset pin. 
  3 pins are required to interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

  Many thx to https://github.com/phrm/fmtx/blob/master/firmware/firmware.ino !

 ****************************************************/

#include <Wire.h>
#include <Adafruit_Si4713.h>
#define _BV(n) (1 << n)

#define RESETPIN 12

#define FMSTATION 10230      // 10230 == 102.30 MHz

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

void initialRDSdata() {
  radio.beginRDS(0x4F87, 1);
  radio.setRDSstation("RaABLNET");
  radio.setRDSbuffer("Adafruit-Si4713-DynamicRDS repository from GitHub.com           ");
}

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit Radio - Si4713 Test");

  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    while (1);
  }

  // Uncomment to scan power of entire range from 87.5 to 108.0 MHz
  /*
  for (uint16_t f  = 8750; f<10800; f+=10) {
   radio.readTuneMeasure(f);
   Serial.print("Measuring "); Serial.print(f); Serial.print("...");
   radio.readTuneStatus();
   Serial.println(radio.currNoiseLevel);
   }
   */

  Serial.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max

  Serial.print("\nTuning into "); 
  Serial.print(FMSTATION/100); 
  Serial.print('.'); 
  Serial.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION); // 102.3 mhz

  // This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();
  Serial.print("\tCurr freq: "); 
  Serial.println(radio.currFreq);
  Serial.print("\tCurr freqdBuV:"); 
  Serial.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:"); 
  Serial.println(radio.currAntCap);
  // begin the RDS/RDBS transmission
  initialRDSdata();
  Serial.println("RDS on!");  
  radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
  Serial.println("Enter help for help.");
}



void loop() {
  radio.readASQ();
  Serial.print("\tCurr ASQ: 0x"); 
  Serial.println(radio.currASQ, HEX);
  Serial.print("\tCurr InLevel:"); 
  Serial.println(radio.currInLevel);
  // toggle GPO1 and GPO2
  radio.setGPIO(_BV(1));
  delay(1000);
  radio.setGPIO(_BV(2));
  delay(1000);
}
// Sending RDS Data over Serial
void serialEvent() {
  
            String getTextSerial = Serial.readString();
            getTextSerial.trim();
            if (getTextSerial == "help"){
                Serial.println(F("You can control the RDS data with serial commands. Following commands are available in this version:"));
                Serial.println(F("---------------------------------"));
                Serial.println(F("main"));
                Serial.println(F("Sets the initial RDS data (from the function initialRDSdata)"));
                Serial.println(F("---------------------------------"));
                Serial.println(F("reg"));
                Serial.println(F("Switches to regional mode"));
                Serial.println(F("---------------------------------"));
                Serial.println(F("title"));
                Serial.println(F("You can set the artist & title information with this command."));
                Serial.println(F("---------------------------------"));
                Serial.println(F("info"));
                Serial.println(F("You can send any information with this command. That could be an announcement, the name of the currently running show or something else."));
                Serial.println(F("---------------------------------"));
                Serial.println(F("Notice: The inputted text must be less than 100 characters. If you want to change that, edit it in line 165 of this file. Otherwise, strings will get corrupted."));
            } else if (getTextSerial == "main") {
                initialRDSdata();
                Serial.println(F("Applied changes"));
            } else if (getTextSerial == "reg") {
                radio.beginRDS(0x4E87, 1);
                radio.setRDSstation("Regional");
                radio.setRDSbuffer("Regional RDS data                                               ");
                Serial.println(F("Applied changes"));
            } else if (getTextSerial == "title") {
              Serial.println("Please enter artist & title information");
              while(!Serial.available() ){}
              String getTitleSerial = Serial.readString();
              getTitleSerial.trim();
              String PSSerial =  "RaABLNETRaABLNETRaABLNET" + getTitleSerial;
            String RTSerial = "Now playing: " + getTitleSerial;
            int PSlength = PSSerial.length();
            int RTlength = RTSerial.length();
            int append = 64 - RTlength;
             for (int i = 0; i<append; i++) {
                RTSerial.concat(" ");
              }
              if(RTSerial.length() > 64) {
               RTSerial[64] = '\0';
            }
              int PSappend = PSlength % 8; 
            switch (PSappend) {
              case 1:
              PSSerial.concat("       ");
              break;
              case 2:
              PSSerial.concat("      ");
              break;
              case 3:
              PSSerial.concat("     ");
              break;
              case 4:
              PSSerial.concat("    ");
              break;
              case 5:
              PSSerial.concat("   ");
              break;
              case 6:
              PSSerial.concat("  ");
              break;
              case 7:
              PSSerial.concat(" ");
              break;
              }
            int PSnewlength = PSSerial.length();
            int mesCount = (PSnewlength + (8-1))/8;
            char radiotext[100];
            char rdsps[PSnewlength];
            radio.beginRDS(0x4F87, mesCount);
            strcpy(rdsps, PSSerial.c_str());
            radio.setRDSstation(rdsps);
            strcpy(radiotext, RTSerial.c_str());
            radio.setRDSbuffer(radiotext);
            Serial.println("Radiotext: " + RTSerial);
            Serial.println("RDS-PS: " + PSSerial);
            Serial.println("Artist & title information: " + getTitleSerial);
            } else if (getTextSerial == "info") {
              Serial.println("Please enter information");
              while(!Serial.available() ){}
              String getInfoSerial = Serial.readString();
              getInfoSerial.trim();
              String PSSerial =  "RaABLNETRaABLNETRaABLNET" + getInfoSerial;
            String RTSerial = getInfoSerial;
            int PSlength = PSSerial.length();
            int RTlength = RTSerial.length();
            int append = 64 - RTlength;
             for (int i = 0; i<append; i++) {
                RTSerial.concat(" ");
              }
              if(RTSerial.length() > 64) {
               RTSerial[64] = '\0';
            }
              int PSappend = PSlength % 8; 
            switch (PSappend) {
              case 1:
              PSSerial.concat("       ");
              break;
              case 2:
              PSSerial.concat("      ");
              break;
              case 3:
              PSSerial.concat("     ");
              break;
              case 4:
              PSSerial.concat("    ");
              break;
              case 5:
              PSSerial.concat("   ");
              break;
              case 6:
              PSSerial.concat("  ");
              break;
              case 7:
              PSSerial.concat(" ");
              break;
              }
            int PSnewlength = PSSerial.length();
            int mesCount = (PSnewlength + (8-1))/8;
            char radiotext[100];
            char rdsps[PSnewlength];
            radio.beginRDS(0x4F87, mesCount);
            strcpy(rdsps, PSSerial.c_str());
            radio.setRDSstation(rdsps);
            strcpy(radiotext, RTSerial.c_str());
            radio.setRDSbuffer(radiotext);
            Serial.println("Radiotext: " + RTSerial);
            Serial.println("RDS-PS: " + PSSerial);
            Serial.println("Information: " + getInfoSerial);
            } else {
              Serial.println(F("Command not found. Enter help for help."));
            }
  }
