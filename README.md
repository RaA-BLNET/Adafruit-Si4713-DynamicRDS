# Adafruit-Si4713-DynamicRDS
With this code you can send messages over serial and they will appear in the radiotext and RDS-PS.<br/>
You can control the RDS data with serial commands. Following commands are available in this version:

main<br/>
Sets the initial RDS data (from the function initialRDSdata)<br/>

reg<br/>
Switches to regional mode.<br/>

title<br/>
You can set the artist & title information with this command.<br/>

info<br/>
You can send any information with this command. That could be an announcement, the name of the currently running show or something else.<br/>

Notice: There are character limits: 64 characters for radiotext (RDS spec limitation), 96 characters or 12 messages for RDS-PS (hardware limitation of the Si4713 chip)<br/>

To change the station name, you need to change it in the rdspstextprocessor function too.<br/>

<img src="assets/rds_screenshot.jpg?raw=true" width="500px">
<hr>
<a href="https://www.adafruit.com/product/1958"><img src="assets/board.jpg?raw=true" width="500px"></a>

This is the Adafruit FM Transmitter with RDS/RBDS Breakout - Si4713 library

Tested and works great with the Adafruit Si4713 Breakout Board 
* http://www.adafruit.com/products/1958

These transmitters use I2C to communicate, plus reset pin, 3 pins are required to interface

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

Limor Fried/Ladyada (Adafruit Industries).
BSD license, check license.txt for more information
All text above must be included in any redistribution

To install, download this code, unpack it and open "adaradio.ino" in the folder /examples/adaradio/
