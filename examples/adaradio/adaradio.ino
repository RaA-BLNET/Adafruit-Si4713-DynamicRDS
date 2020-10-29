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
  Serial.println(F("Adafruit Radio - Si4713 Test"));

  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println(F("Couldn't find radio?"));
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
  radio.setTXpower(115);  // dBuV, 88-120 max

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
  Serial.println(F("RDS on!"));  
  radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
  Serial.println("Enter help for help.");
}


//String-into-StringArray-Splitter from https://stackoverflow.com/questions/9072320/split-string-into-string-array
void loop() {
}
 String getValue(String data, char separator, int index) {
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  int found = 0;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// RDS-PS text processor
String rdspstextprocessor(String getTitleSerialp) {
                 String PSSerial[20];
               String PSfinal;
               int psloopcounter = 0;
               int psloopcounter2 = 0;
               do { 
               PSSerial[psloopcounter] = getValue(getTitleSerialp, ' ', psloopcounter);
               psloopcounter++;
              } while (psloopcounter <= 19);
              do {
               if ((PSSerial[psloopcounter2].length()) == 0) {
                } else if ((PSSerial[psloopcounter2].length()) == 1){
                    if ((PSSerial[psloopcounter2 + 1].length()) == 1) {
                    if ((PSSerial[psloopcounter2 + 2].length()) == 1) {
                      if ((PSSerial[psloopcounter2 + 3].length()) == 1) {
                      PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " " + PSSerial[psloopcounter2 + 3] + " ");
                      psloopcounter2++;
                      psloopcounter2++;
                      psloopcounter2++;
                      } else if ((PSSerial[psloopcounter2 + 3].length()) == 2) {
                      PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " " + PSSerial[psloopcounter2 + 3]);
                      psloopcounter2++;
                      psloopcounter2++;
                      psloopcounter2++;
                      } else {
                      PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + "  ");
                      psloopcounter2++;
                      psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 2].length()) == 2) {
                      if ((PSSerial[psloopcounter2 + 3].length()) == 1) {
                      PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " " + PSSerial[psloopcounter2 + 3]);
                      psloopcounter2++;
                      psloopcounter2++;
                      psloopcounter2++;
                      } else {
                      PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " ");
                      psloopcounter2++;
                      psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 2].length()) == 3) {
                      PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " ");
                      psloopcounter2++;
                      psloopcounter2++;
                    } else if ((PSSerial[psloopcounter2 + 2].length()) == 4) {
                      PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2]);
                      psloopcounter2++;
                      psloopcounter2++;
                    } else {
                      PSfinal.concat("  " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "   ");
                      psloopcounter2++;
                    }
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 2) {
                      if ((PSSerial[psloopcounter2 + 2].length()) == 1) {
                        if ((PSSerial[psloopcounter2 + 3].length()) == 1) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " " + PSSerial[psloopcounter2 + 3]);
                        psloopcounter2++;
                        psloopcounter2++;
                        psloopcounter2++;
                        } else {
                        PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " ");
                        psloopcounter2++;
                        psloopcounter2++;
                        }
                      } else if ((PSSerial[psloopcounter2 + 2].length()) == 2) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " ");
                        psloopcounter2++;
                        psloopcounter2++;
                      } else if ((PSSerial[psloopcounter2 + 2].length()) == 3) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2]);
                        psloopcounter2++;
                        psloopcounter2++;
                      } else {
                        PSfinal.concat("  " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "  ");
                        psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 3) {
                      if ((PSSerial[psloopcounter2 + 2].length()) == 1) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " ");
                        psloopcounter2++;
                        psloopcounter2++;
                      } else if ((PSSerial[psloopcounter2 + 2].length()) == 2) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2]);
                        psloopcounter2++;
                        psloopcounter2++;
                      } else {
                        PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "  ");
                        psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 4) {
                      if ((PSSerial[psloopcounter2 + 2].length()) == 1) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2]);
                        psloopcounter2++;
                        psloopcounter2++;
                      } else {
                        PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " ");
                        psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 5) {
                    PSfinal.concat(PSSerial[psloopcounter2] + " " +PSSerial[psloopcounter2 + 1] + " ");
                    psloopcounter2++;
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 6) {
                    PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1]);
                    psloopcounter2++;
                    } else {
                    PSfinal.concat("   " + PSSerial[psloopcounter2] + "    ");
                    }
                  } else if ((PSSerial[psloopcounter2].length()) == 2){
                    if ((PSSerial[psloopcounter2 + 1].length()) == 1) {
                      if ((PSSerial[psloopcounter2 + 2].length()) == 1) {
                        if ((PSSerial[psloopcounter2 + 3].length()) == 1) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + PSSerial[psloopcounter2 + 3]);
                        psloopcounter2++;
                        psloopcounter2++;
                        psloopcounter2++;
                        } else {
                        PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " ");
                        psloopcounter2++;
                        psloopcounter2++;
                        }
                      } else if ((PSSerial[psloopcounter2 + 2].length()) == 2) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2] + " ");
                        psloopcounter2++;
                        psloopcounter2++;
                      } else if ((PSSerial[psloopcounter2 + 2].length()) == 3) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2]);
                        psloopcounter2++;
                        psloopcounter2++;
                      } else {
                        PSfinal.concat("  " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "  ");
                        psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 2) {
                      if ((PSSerial[psloopcounter2 + 2].length()) == 1) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "  " + PSSerial[psloopcounter2 + 2] + " ");
                        psloopcounter2++;
                        psloopcounter2++;
                      } else if ((PSSerial[psloopcounter2 + 2].length()) == 2) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "  " + PSSerial[psloopcounter2 + 2]);
                        psloopcounter2++;
                        psloopcounter2++;
                      } else {
                        PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "  ");
                        psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 3) {
                      if ((PSSerial[psloopcounter2 + 2].length()) == 1) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2]);
                        psloopcounter2++;
                        psloopcounter2++;
                      } else {
                        PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " ");
                        psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 4) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " ");
                        psloopcounter2++;
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 5) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1]);
                        psloopcounter2++;
                    } else {
                    PSfinal.concat("   " + PSSerial[psloopcounter2] + "   ");
                    }
                  } else if ((PSSerial[psloopcounter2].length()) == 3){
                    if ((PSSerial[psloopcounter2 + 1].length()) == 1) {
                      if ((PSSerial[psloopcounter2 + 2].length()) == 1) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "  " + PSSerial[psloopcounter2 + 2] + " ");
                        psloopcounter2++;
                        psloopcounter2++;
                      } else if ((PSSerial[psloopcounter2 + 2].length()) == 2) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "  " + PSSerial[psloopcounter2 + 2]);
                        psloopcounter2++;
                        psloopcounter2++;
                      } else {
                        PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + "  ");
                        psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 2) {
                      if ((PSSerial[psloopcounter2 + 2].length()) == 1) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2]);
                        psloopcounter2++;
                        psloopcounter2++;
                      } else {
                        PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " ");
                        psloopcounter2++;
                      }
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 3) {
                    PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " ");
                    psloopcounter2++;
                    } else if ((PSSerial[psloopcounter2 + 1].length()) == 4) {
                    PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1]);
                    psloopcounter2++;
                    } else {
                    PSfinal.concat("  " + PSSerial[psloopcounter2] + "   ");
                    }
                  } else if ((PSSerial[psloopcounter2].length()) == 4){
                      if ((PSSerial[psloopcounter2 + 1].length()) == 1) {
                          if ((PSSerial[psloopcounter2 + 2].length()) == 1){
                          PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " " + PSSerial[psloopcounter2 + 2]);
                          psloopcounter2++;
                          psloopcounter2++;
                          } else {
                          PSfinal.concat(" " + PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " ");
                          psloopcounter2++;
                          }
                      } else if ((PSSerial[psloopcounter2 + 1].length()) == 2) {
                          PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " ");
                          psloopcounter2++;
                      } else if ((PSSerial[psloopcounter2 + 1].length()) == 3) {
                          PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1]);
                          psloopcounter2++;
                      } else {
                    PSfinal.concat("  " + PSSerial[psloopcounter2] + "  ");
                      }
                  } else if ((PSSerial[psloopcounter2].length()) == 5){
                      if ((PSSerial[psloopcounter2 + 1].length()) == 1) {
                        PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1] + " ");
                        psloopcounter2++;
                      } else if ((PSSerial[psloopcounter2 + 1].length()) == 2) {
                            PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1]);
                            psloopcounter2++;
                      } else {
                            PSfinal.concat(" " + PSSerial[psloopcounter2] + "  ");
                      }
                  } else if ((PSSerial[psloopcounter2].length()) == 6){
                      if ((PSSerial[psloopcounter2 + 1].length()) == 1) {
                          PSfinal.concat(PSSerial[psloopcounter2] + " " + PSSerial[psloopcounter2 + 1]);
                          psloopcounter2++;
                      } else {
                    PSfinal.concat(" " + PSSerial[psloopcounter2] + " ");
                      }
                  } else if ((PSSerial[psloopcounter2].length()) == 7){
                    PSfinal.concat(PSSerial[psloopcounter2] + " ");
                  } else if ((PSSerial[psloopcounter2].length()) >= 8){
                    PSSerial[psloopcounter2].remove(8);
                    PSfinal.concat(PSSerial[psloopcounter2]);
                  }
                  psloopcounter2++;
              } while (psloopcounter2 <= 19);
            
            PSfinal.concat(F("RaABLNETRaABLNETRaABLNET"));
  return PSfinal;
}
// Sending RDS Data over Serial
void serialEvent() {
  
            String getTextSerial = Serial.readString();
            getTextSerial.trim();
            String appliedchanges = F("Applied changes");
            String radiotextoutputtitle = F("Radiotext: ");
            String rdspsoutputtitle = F("RDS-PS: ");
            if (getTextSerial == F("help")){
                String line = F("---------------------------------");
                Serial.println(F("You can control the RDS data with serial commands. Following commands are available in this version:"));
                Serial.println(line);
                Serial.println(F("main"));
                Serial.println(F("Sets the initial RDS data (from the function initialRDSdata)"));
                Serial.println(line);
                Serial.println(F("reg"));
                Serial.println(F("Switches to regional mode"));
                Serial.println(line);
                Serial.println(F("title"));
                Serial.println(F("You can set the artist & title information with this command."));
                Serial.println(line);
                Serial.println(F("info"));
                Serial.println(F("You can send any information with this command. That could be an announcement, the name of the currently running show or something else."));
                Serial.println(line);
                Serial.println(F("Notice: The inputted text must be less than 100 characters. If you want to change that, edit it in line 395 of this file. Otherwise, strings will get corrupted."));
            } else if (getTextSerial == F("main")) {
                initialRDSdata();
                Serial.println(appliedchanges);
            } else if (getTextSerial == F("reg")) {
                radio.beginRDS(0x4E87, 1);
                radio.setRDSstation("Regional");
                radio.setRDSbuffer("Regional RDS data                                               ");
                Serial.println(appliedchanges);
            } else if (getTextSerial == F("title")) {
              Serial.println(F("Please enter artist & title information"));
              while(!Serial.available() ){}
              String getTitleSerial = Serial.readString();
              getTitleSerial.trim();
              String nowplaying = F("Now playing: ");
            String RTSerial = nowplaying + getTitleSerial;
            int RTlength = RTSerial.length();
            int append = 64 - RTlength;
             for (int i = 0; i<append; i++) {
                RTSerial.concat(F(" "));
              }
              if(RTSerial.length() > 64) {
               RTSerial[64] = '\0';
            }
            String PSfinal2 = rdspstextprocessor(getTitleSerial);
            int PSnewlength = PSfinal2.length();
            int mesCount = PSnewlength / 8;
            char radiotext[100];
            char rdsps[200];
            radio.beginRDS(0x4F87, mesCount);
            strcpy(rdsps, PSfinal2.c_str());
            radio.setRDSstation(rdsps);
            strcpy(radiotext, RTSerial.c_str());
            radio.setRDSbuffer(radiotext);
            Serial.println(radiotextoutputtitle + RTSerial);
            Serial.println(rdspsoutputtitle + PSfinal2);
            String titleinfooutputtitle = F("Artist & title information: ");
            Serial.println(titleinfooutputtitle + getTitleSerial);
            } else if (getTextSerial == F("info")) {
              Serial.println(F("Please enter information"));
              while(!Serial.available() ){}
              String getInfoSerial = Serial.readString();
              getInfoSerial.trim();
            String RTSerial = getInfoSerial;
            int RTlength = RTSerial.length();
            int append = 64 - RTlength;
             for (int i = 0; i<append; i++) {
                RTSerial.concat(F(" "));
              }
              if(RTSerial.length() > 64) {
               RTSerial[64] = '\0';
            }
            String PSfinal2 = rdspstextprocessor(getInfoSerial);
            int PSnewlength = PSfinal2.length();
            int mesCount = PSnewlength / 8;
            char radiotext[100];
            char rdsps[200];
            radio.beginRDS(0x4F87, mesCount);
            strcpy(rdsps, PSfinal2.c_str());
            radio.setRDSstation(rdsps);
            strcpy(radiotext, RTSerial.c_str());
            radio.setRDSbuffer(radiotext);
            String infooutputtitle = F("Information: ");
            Serial.println(radiotextoutputtitle + RTSerial);
            Serial.println(rdspsoutputtitle + PSfinal2);
            Serial.println(infooutputtitle + getInfoSerial);
            } else {
              Serial.println(F("Command not found. Enter help for help."));
            }
  }
