/*!
 *  @file Adafruit_Si4713.cpp
 *
 *  @mainpage Adafruit Si4713 breakout
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for Si4713 breakout
 *
 * 	This is a library for the Adafruit Si4713 breakout:
 * 	http://www.adafruit.com/products/1958
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Adafruit_Si4713.h"

/*!
 *    @brief  Instantiates a new Si4713 class
 *    @param  resetpin
 *            number of pin where reset is connected
 *
 */
Adafruit_Si4713::Adafruit_Si4713(int8_t resetpin) { _rst = resetpin; }

/*!
 *    @brief  Setups the i2c and calls powerUp function.
 *    @param  addr
 *            i2c address
 *    @param  theWire
 *            wire object
 *    @return True if initialization was successful, otherwise false.
 *
 */
bool Adafruit_Si4713::begin(uint8_t addr, TwoWire *theWire) {
  _i2caddr = addr;
  _wire = theWire;

  _wire->begin();

  reset();

  powerUp();

  // check for Si4713
  if (getRev() != 13)
    return false;

  return true;
}

/*!
 *    @brief  Resets the registers to default settings and puts chip in
 * powerdown mode
 */
void Adafruit_Si4713::reset() {
  if (_rst > 0) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(10);
    digitalWrite(_rst, LOW);
    delay(10);
    digitalWrite(_rst, HIGH);
  }
}

/*!
 *    @brief  Set chip property over I2C
 *    @param  property
 *            prooperty that will be set
 *    @param  value
 *            value of property
 */
void Adafruit_Si4713::setProperty(uint16_t property, uint16_t value) {
  _i2ccommand[0] = SI4710_CMD_SET_PROPERTY;
  _i2ccommand[1] = 0;
  _i2ccommand[2] = property >> 8;
  _i2ccommand[3] = property & 0xFF;
  _i2ccommand[4] = value >> 8;
  _i2ccommand[5] = value & 0xFF;
  sendCommand(6);

#ifdef SI4713_CMD_DEBUG
  Serial.print("Set Prop ");
  Serial.print(property);
  Serial.print(" = ");
  Serial.println(value);
#endif
}

/*!
 *    @brief  Send command stored in _i2ccommand to chip.
 *    @param  len
 *            length of command that will be send
 */
void Adafruit_Si4713::sendCommand(uint8_t len) {
#ifdef SI4713_CMD_DEBUG
  Serial.print("\n*** Command:");
#endif
  _wire->beginTransmission(_i2caddr);
  for (uint8_t i = 0; i < len; i++) {
#ifdef SI4713_CMD_DEBUG
    Serial.print(" 0x");
    Serial.print(_i2ccommand[i], HEX);
#endif
    _wire->write(_i2ccommand[i]);
  }
  _wire->endTransmission();
#ifdef SI4713_CMD_DEBUG
  Serial.println();
#endif
  // Wait for status CTS bit
  uint8_t status = 0;
  while (!(status & SI4710_STATUS_CTS)) {
    _wire->requestFrom((uint8_t)_i2caddr, (uint8_t)1);
    status = _wire->read();
#ifdef SI4713_CMD_DEBUG
    Serial.print("status: ");
    Serial.println(status, HEX);
#endif
  }
}

/*!
 *    @brief  Tunes to given transmit frequency.
 *    @param  freqKHz
 *            frequency in KHz
 */
void Adafruit_Si4713::tuneFM(uint16_t freqKHz) {
  _i2ccommand[0] = SI4710_CMD_TX_TUNE_FREQ;
  _i2ccommand[1] = 0;
  _i2ccommand[2] = freqKHz >> 8;
  _i2ccommand[3] = freqKHz;
  sendCommand(4);
  while ((getStatus() & 0x81) != 0x81) {
    delay(10);
  }
}

/*!
 *    @brief  Sets the output power level and tunes the antenna capacitor
 *    @param  pwr
 *            power value
 *    @param  antcap
 * 	          antenna capacitor (default to 0)
 */
void Adafruit_Si4713::setTXpower(uint8_t pwr, uint8_t antcap) {
  _i2ccommand[0] = SI4710_CMD_TX_TUNE_POWER;
  _i2ccommand[1] = 0;
  _i2ccommand[2] = 0;
  _i2ccommand[3] = pwr;
  _i2ccommand[4] = antcap;
  sendCommand(5);
}
/*!
 *    @brief  Queries the TX status and input audio signal metrics.
 */
void Adafruit_Si4713::readASQ() {
  _i2ccommand[0] = SI4710_CMD_TX_ASQ_STATUS;
  _i2ccommand[1] = 0x1;
  sendCommand(2);

  _wire->requestFrom((uint8_t)_i2caddr, (uint8_t)5);

  _wire->read();           // status
  currASQ = _wire->read(); // resp1
  _wire->read();           // resp2 unused
  _wire->read();           // resp3 unused
  currInLevel = _wire->read();
}

/*!
 *    @brief  Queries the status of a previously sent TX Tune Freq, TX Tune
 * Power, or TX Tune Measure using SI4710_CMD_TX_TUNE_STATUS command.
 */
void Adafruit_Si4713::readTuneStatus() {
  _i2ccommand[0] = SI4710_CMD_TX_TUNE_STATUS;
  _i2ccommand[1] = 0x1; // INTACK
  sendCommand(2);

  _wire->requestFrom((uint8_t)_i2caddr, (uint8_t)8);

  _wire->read();
  _wire->read();            // status and resp1
  currFreq = _wire->read(); // resp2
  currFreq <<= 8;
  currFreq |= _wire->read(); // resp3
  _wire->read();             // resp4
  currdBuV = _wire->read();
  currAntCap = _wire->read();
  currNoiseLevel = _wire->read();
}

/*!
 *    @brief  Measure the received noise level at the specified frequency using
 *            SI4710_CMD_TX_TUNE_MEASURE command.
 *    @param  freq
 *            frequency
 */
void Adafruit_Si4713::readTuneMeasure(uint16_t freq) {
  // check freq is multiple of 50khz
  if (freq % 5 != 0) {
    freq -= (freq % 5);
  }
  // Serial.print("Measuring "); Serial.println(freq);
  _i2ccommand[0] = SI4710_CMD_TX_TUNE_MEASURE;
  _i2ccommand[1] = 0;
  _i2ccommand[2] = freq >> 8;
  _i2ccommand[3] = freq;
  _i2ccommand[4] = 0;

  sendCommand(5);
  while (getStatus() != 0x81)
    delay(10);
}

/*!
 *    @brief  Begin RDS
 *            Sets properties as follows:
 *            SI4713_PROP_TX_AUDIO_DEVIATION: 66.25KHz,
 *            SI4713_PROP_TX_RDS_DEVIATION: 2KHz,
 *            SI4713_PROP_TX_RDS_INTERRUPT_SOURCE: 1,
 *            SI4713_PROP_TX_RDS_PS_MIX: 50% mix (default value),
 *            SI4713_PROP_TX_RDS_PS_MISC: 6152,
 *            SI4713_PROP_TX_RDS_PS_REPEAT_COUNT: 3,
 *            SI4713_PROP_TX_RDS_MESSAGE_COUNT: 1,
 *            SI4713_PROP_TX_RDS_PS_AF: 57568,
 *            SI4713_PROP_TX_RDS_FIFO_SIZE: 0,
 *            SI4713_PROP_TX_COMPONENT_ENABLE: 7
 *    @param  programID
 *            sets SI4713_PROP_TX_RDS_PI to parameter value
 */
void Adafruit_Si4713::beginRDS(uint16_t programID, int mesCt) {
  // 65.75KHz
  setProperty(SI4713_PROP_TX_AUDIO_DEVIATION, 6575);

  //6.75KHz (default is 675)
  setProperty(SI4713_PROP_TX_PILOT_DEVIATION, 675);
  // 2.5KHz (default is 300)
  setProperty(SI4713_PROP_TX_RDS_DEVIATION, 250);

  // RDS IRQ
  setProperty(SI4713_PROP_TX_RDS_INTERRUPT_SOURCE, 0x0001);
  // program identifier
  setProperty(SI4713_PROP_TX_RDS_PI, programID);
  // 50% mix (default)
  setProperty(SI4713_PROP_TX_RDS_PS_MIX, 0x03);
  // RDSD0 & RDSMS (default)
  setProperty(SI4713_PROP_TX_RDS_PS_MISC, 0x8D48);
  // 3 repeats (default)
  setProperty(SI4713_PROP_TX_RDS_PS_REPEAT_COUNT, 8);

  setProperty(SI4713_PROP_TX_RDS_MESSAGE_COUNT, mesCt);
  setProperty(SI4713_PROP_TX_RDS_PS_AF, 0xE194); // 102.3 MHz E194
  setProperty(SI4713_PROP_TX_RDS_FIFO_SIZE, 0);

  setProperty(SI4713_PROP_TX_COMPONENT_ENABLE, 0x0007);
}

/*!
 *    @brief  Set up the RDS station string
 *    @param  *s
 *            string to load
 */
void Adafruit_Si4713::setRDSstation(char *s) {
  if (strlen(s) >= 96) {
  s[96] = '\0';
  }
  uint8_t i, len = strlen(s);
  uint8_t slots = (len + 3) / 4;

  for (uint8_t i = 0; i < slots; i++) {
    memset(_i2ccommand, ' ', 6); // clear it with ' '
    memcpy(_i2ccommand + 2, s, min(4, (int)strlen(s)));
    s += 4;
    _i2ccommand[6] = 0;
    // Serial.print("Set slot #"); Serial.print(i);
    // char *slot = (char *)( _i2ccommand+2);
    // Serial.print(" to '"); Serial.print(slot); Serial.println("'");
    _i2ccommand[0] = SI4710_CMD_TX_RDS_PS;
    _i2ccommand[1] = i; // slot #
    sendCommand(6);
  }
}

/*!
 *    @brief  Queries the status of the RDS Group Buffer and loads new data into
 * buffer.
 *    @param  *s
 *            string to load
 */
void Adafruit_Si4713::setRDSbuffer(char s[64], int gcCounterPT) {
  if (strlen(s) < 64) {
      char carriageReturn[2];
      carriageReturn[0] = 0x0D;
      
      carriageReturn[1] = '\0';
      strcat(s, carriageReturn);
  } else {
    s[64] = '\0';
  }
  uint8_t i, len = strlen(s);
  uint8_t gcValue;
  uint8_t slots = ((len + 3) / 4);
  char slot[5];
  if (gcCounterPT % 2 == 0)
    gcValue = 0;
  else if (gcCounterPT % 2 == 1)
    gcValue = 16;
  for (uint8_t i = gcValue; i < slots + gcValue; i++) {
    memset(_i2ccommand, ' ', 8); // clear/fill the rest
    memcpy(_i2ccommand + 4, s, min(4, (int)strlen(s)));
    s += 4;
    _i2ccommand[8] = 0;
    // Serial.print("Set buff #"); Serial.print(i);
    // char *slot = (char *)( _i2ccommand+4);
    // Serial.print(" to '"); Serial.print(slot); Serial.println("'");
    _i2ccommand[0] = SI4710_CMD_TX_RDS_BUFF;
    if (i == gcValue)
      _i2ccommand[1] = 0x06;
    else
      _i2ccommand[1] = 0x04;

    _i2ccommand[2] = 0x20;
    _i2ccommand[3] = i;
    sendCommand(8);
  }

  /*
  // set time
   _i2ccommand[0] = SI4710_CMD_TX_RDS_BUFF;
   _i2ccommand[1] = 0x04;
   _i2ccommand[2] = 0x40; // RTC
   _i2ccommand[3] = 01;
   _i2ccommand[4] = 0xCD;
   _i2ccommand[5] = 0x0B;
   _i2ccommand[6] = 0x2A;
   _i2ccommand[7] = 0x04;
   sendCommand(8);
  */
   Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)6);
   Wire.read(); // status
   Serial.print("FIFO overflow: 0x"); Serial.println(Wire.read(), HEX);
   Serial.print("Circ avail: "); Serial.println(Wire.read());
   Serial.print("Circ used: "); Serial.println(Wire.read());
   Serial.print("FIFO avail: "); Serial.println(Wire.read());
   Serial.print("FIFO used: "); Serial.println(Wire.read());
   

  // enable!
  // Serial.println("Enable RDS");
  // setProperty(SI4713_PROP_TX_COMPONENT_ENABLE, 0x0007); // stereo, pilot+rds
  /*
  // wait till ready
  while (getStatus() != 0x80) {
    Serial.println(getStatus(), HEX);
    delay(100);
  }
  delay(500);
  _i2ccommand[0] = SI4710_CMD_TX_RDS_BUFF;
  _i2ccommand[1] = 0x01; // clear RDSINT
  _i2ccommand[2] = 0x0;
  _i2ccommand[3] = 0x0;
  _i2ccommand[4] = 0x0;
  _i2ccommand[5] = 0x0;
  _i2ccommand[6] = 0x0;
  _i2ccommand[7] = 0x0;
   sendCommand(8);
   // get reply!
   Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)6);
   for (uint8_t x=0; x<7; x++) {
     Wire.read();
   }
   */
  /*   or you can read the status
  Wire.read(); // status
  Serial.print("FIFO overflow: 0x"); Serial.println(Wire.read(), HEX);
  Serial.print("Circ avail: "); Serial.println(Wire.read());
  Serial.print("Circ used: "); Serial.println(Wire.read());
  Serial.print("FIFO avail: "); Serial.println(Wire.read());
  Serial.print("FIFO used: "); Serial.println(Wire.read());
  */
}

/*!
 *    @brief  Read interrupt status bits.
 *    @return status bits
 */
uint8_t Adafruit_Si4713::getStatus() {
  _wire->beginTransmission(_i2caddr);
  _wire->write(SI4710_CMD_GET_INT_STATUS);
  _wire->endTransmission();
  _wire->requestFrom((uint8_t)_i2caddr, (uint8_t)1);
  return _wire->read();
}

/*!
 *    @brief  Sends power up command to the breakout, than CTS and GPO2 output
 * is disabled and than enable xtal oscilator. Also It sets properties:
 *            SI4713_PROP_REFCLK_FREQ: 32.768
 *            SI4713_PROP_TX_PREEMPHASIS: 74uS pre-emph (USA standard)
 *            SI4713_PROP_TX_ACOMP_GAIN: max gain
 *            SI4713_PROP_TX_ACOMP_ENABLE: turned on limiter and AGC
 */
void Adafruit_Si4713::powerUp() {
  _i2ccommand[0] = SI4710_CMD_POWER_UP;
  _i2ccommand[1] = 0x12;
  // CTS interrupt disabled
  // GPO2 output disabled
  // Boot normally
  // xtal oscillator ENabled
  // FM transmit
  _i2ccommand[2] = 0x50; // analog input mode
  sendCommand(3);

  // configuration! see page 254
  setProperty(SI4713_PROP_REFCLK_FREQ, 32768); // crystal is 32.768
  setProperty(SI4713_PROP_TX_PREEMPHASIS, 1);  // 50uS pre-emph (Europe std)
  setProperty(SI4713_PROP_TX_ACOMP_GAIN, 10);  // max gain?
  // setProperty(SI4713_PROP_TX_ACOMP_ENABLE, 0x02); // turn on limiter, but no
  // dynamic ranging
  setProperty(SI4713_PROP_TX_ACOMP_ENABLE, 0x02); // turn on limiter and AGC
}

/*!
 *    @brief  Get the hardware revision code from the device using
 * SI4710_CMD_GET_REV
 *    @return revision number
 */
uint8_t Adafruit_Si4713::getRev() {
  _i2ccommand[0] = SI4710_CMD_GET_REV;
  _i2ccommand[1] = 0;

  sendCommand(2);

  _wire->requestFrom((uint8_t)_i2caddr, (uint8_t)9);

  _wire->read();
  uint8_t pn = _wire->read();
  uint8_t fw = _wire->read();
  fw <<= 8;
  fw |= _wire->read();
  uint8_t patch = _wire->read();
  patch <<= 8;
  patch |= _wire->read();
  uint8_t cmp = _wire->read();
  cmp <<= 8;
  cmp |= _wire->read();
  uint8_t chiprev = _wire->read();

  /*
  Serial.print("Part # Si47"); Serial.print(pn);
  Serial.print("-"); Serial.println(fw, HEX);

  Serial.print("Firmware 0x"); Serial.println(fw, HEX);
  Serial.print("Patch 0x"); Serial.println(patch, HEX);
  Serial.print("Chip rev "); Serial.write(chiprev); Serial.println();
  */

  return pn;
}

/*!
 *    @brief  Configures GP1 / GP2 as output or Hi-Z.
 *    @param  x
 *            bit value
 */
void Adafruit_Si4713::setGPIOctrl(uint8_t x) {
  // Serial.println("GPIO direction");
  _i2ccommand[0] = SI4710_CMD_GPO_CTL;
  _i2ccommand[1] = x;

  sendCommand(2);
}

/*!
 *    @brief  Sets GP1 / GP2 output level (low or high).
 *    @param  x
 *            bit value
 */
void Adafruit_Si4713::setGPIO(uint8_t x) {
  // Serial.println("GPIO set");
  _i2ccommand[0] = SI4710_CMD_GPO_SET;
  _i2ccommand[1] = x;

  sendCommand(2);
}
