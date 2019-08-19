## Software Information:

### Arduino Libraries in Use
* Wire (built in I2C library) - [See Arduino Reference Site](https://www.arduino.cc/en/Reference/Wire)
* BME280 and BMP280 Digital Pressure Sensor Library (Version 0.0.2) - *by Gregor Christandl* - [Source Code Here](https://bitbucket.org/christandlg/bmx280mi)
* Adafruit Unified Sensor Library (Version 1.0.3) - *by Adafruit* - [Source Code Here](https://github.com/adafruit/Adafruit_Sensor)
* Adafruit DHT-sensor-library (Version 1.3.7) - *by Adafruit* - [Source Code Here](https://github.com/adafruit/DHT-sensor-library)
* SerialDebug (Version 0.9.2) - *by Joao Lopes* - [Source Code Here](https://github.com/JoaoLopesF/SerialDebug)

### Tools in Use
* [SerialDebugApp](https://github.com/JoaoLopesF/SerialDebug/tree/master/SerialDebugApp) for helping read output from SerialDebug library.
* [Visual Micro](https://www.visualmicro.com/) for working with Arduino in Visual Studio.
    
## Hardware Information

### Sensors / Peripherals
<div class="parts-table">

| Part Name | Function / Description | Data Sheet | Purchase Sources / Prices |
|-----------|------------------------|------------|---------------------------|
| AMBI (GA1A1S201WP) | **Ambient light sensor.**<br>- Wide operating temperature range (-40 - +85 C)<br>- Gives consistent and sensible output readings (0 is pitch black, maximum sunlight is 1023)<br>- Won't over saturate in daylight | [GA1A1S201WP OPIC Light Detector (Mouser.ca)](https://www.mouser.ca/datasheet/2/365/GA1A1S201WP_Spec-184717.pdf) | [Modern Device](https://moderndevice.com/product/ambi-light-sensor/) ($3 USD) |
| BMP280 (BME280) | **Barometric pressure sensor.**<br>- ±1.0°C accuracy<br>- ±1 hPa accuracy<br>- I2C or SPI interface<br>- Altitude, ±1 meter accuracy | [BST-BMP280-DS001 (Bosch.com)](https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP280-DS001.pdf) | [Creatron](https://www.creatroninc.com/product/bmp280-barometric-sensor-300-to-1100hpa/) ($7.90 CAD)<br>[Adafruit](https://www.adafruit.com/product/2651) ($9.95 USD)<br>[Sparkfun - BME280](https://www.sparkfun.com/products/13676) ($19.95 USD)|
| LSM303DLHC | **Compass / accelerometer.**<br>- Compass and accelerometer all in one<br>- I2C interface<br>- Triple axis sensing | [LSM303DLHC (Adafruit.com)](https://cdn-shop.adafruit.com/datasheets/LSM303DLHC.PDF) | [Adafruit](https://www.adafruit.com/product/1120) ($14.95 USD)|
| MH-RD / FC-37 / YL-83 | **Rain sensor.**<br>- Simple interface<br>- Adjustable sensitivity<br>| [No datasheet but info available from Random Nerd Tutorials](https://randomnerdtutorials.com/guide-for-rain-sensor-fc-37-or-yl-83-with-arduino/) | [Creatron](https://www.creatroninc.com/product/water-rain-sensor/?search_query=rain+sensor&results=10) ($5.99 CAD)|
| DHT-22 | **Temperature / humidity sensing.**<br>- 2.5mA max current use during conversion<br>- 0-100% humidity readings with 2-5% accuracy<br>- 40 to 80°C temperature readings ±0.5°C accuracy<br>- No more than 0.5 Hz sampling rate (once every 2 seconds) | [DHT22 Datasheet (Sparkfun.com)](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf) | [Creatron](https://www.creatroninc.com/product/dht22-humidity-and-temperature-sensor/) ($13.00 CAD)<br>[Adafruit](https://www.adafruit.com/product/385) ($9.95 USD)<br>[Sparkfun](https://www.sparkfun.com/products/10167) ($9.95 USD)|
| AS5040 | **Wind direction (magenetic rotary encoder).**<br>- Absolute positioning<br>- Contact free<br>- Very long life<br> - Multi interface | [AS5040 Datasheet (Digikey.com)](https://media.digikey.com/pdf/Data%20Sheets/Austriamicrosystems%20PDFs/AS5040.pdf) | [Digikey](https://www.digikey.ca/product-detail/en/ams/AS5040-SS_EK_AB/AS5040-EK-AB-ND/2339617) ($23.83 CAD)|
| Rev. P wind sensor | **Wind speed sensor.**<br>- Wind speeds measured: 0-150 MPH<br>- Ambient temperature compensation<br>- Accuracy of +/- 3 MPH (estimated) | [No Datasheet but info available here](https://moderndevice.com/uncategorized/calibrating-rev-p-wind-sensor-new-regression/?preview=true) | [Modern Device](https://moderndevice.com/product/wind-sensor-rev-p/) ($24 USD)|
| OCR TM | **Water temperature (thermistor).**<br>- Very inexpensive<br>- Simple interface (it's basically a resistor).<br>- Fairly accurate| [No datasheet but info available from Adafruit](https://learn.adafruit.com/thermistor/using-a-thermistor) | [Amazon.ca](https://www.amazon.ca/Ocr-Waterproof-Digital-Thermal-Temperature/dp/B0185P4MVO/ref=sr_1_12?keywords=thermistor&qid=1566073227&s=gateway&sr=8-12) ($15.99 CAD per 5 units)|
| RFM69HCW  | **Transceiver radio for data transfer (x2).**<br>- Inexpensive<br>- Simple to use<br>- Low power consumption<br>- Long range (at least 1km, up to 10+ km with proper configuration)| [RFM69HCW ( Adafruit.com)](https://cdn-shop.adafruit.com/product-files/3076/RFM69HCW-V1.1.pdf) | [Adafruit.com](https://www.adafruit.com/product/3070) ($9.95 USD each)<br>[Digikey.ca](https://www.digikey.ca/product-detail/en/adafruit-industries-llc/3070/1528-1665-ND/6005355) ($14.15 CAD each)|

</div>

### Microcontrollers
<div class="parts-table">

| Device Name | Function / Description | Guide | Purchase Sources / Prices |
|-----------|------------------------|------------|---------------------------|
| Arduino Mega  | **Main controller for data collection.**<br>- Tonnes of IO<br>- Lots of memory (relative to other microcontrollers)<br>- Good power handling | [Arduino.cc](https://www.arduino.cc/) | [Amazon.ca (clone)](https://www.amazon.ca/keyestudio-Board-Cable-Compatible-Arduino/dp/B016JWNYBE/ref=sr_1_3?keywords=arduino+mega&qid=1566075134&s=gateway&sr=8-3) ($19.99 CAD)<br>[Arduino.cc (official)](https://store.arduino.cc/usa/mega-2560-r3) ($38.50 USD)|
| Raspberry Pi Zero W | **Data receiving, relay, storage, and web server.**<br>- Built in WIFI<br>- Very powerful<br>- Inexpensive | [Arduino.cc](https://www.arduino.cc/) | [Creatron](https://www.creatroninc.com/product/raspberry-pi-zero-w/) ($28.00 CAD)<br>[Amazon.com](https://www.amazon.com/Raspberry-Pi-Zero-Wireless-model/dp/B06XFZC3BX/ref=sr_1_4?keywords=raspberry+pi+zero&qid=1566075868&s=gateway&sr=8-4) ($18.89 USD)|

</div>

### Misc
<div class="parts-table">

| Device Name | Function / Description | Guide | Purchase Sources / Prices |
|-----------|------------------------|------------|---------------------------|
| SanDisk Ultra 32GB  | **SD card for Raspberry Pi.**<br>- Cheap<br>- Lots of reliable storage (32 GB)| N/A | [Amazon.ca](https://www.amazon.ca/gp/product/B073JWXGNT/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1) ($11.47 CAD) |
| ACJ D7S | **Battery power for weather station.**<br>- 12 V<br>- 7Ah<br>- Inexpensive| [AJCBattery.com](https://ajcbattery.com/product/ajc-12v-7ah/#pane-1) | [Amazon.ca](https://www.amazon.ca/gp/product/B00A82GD60/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1) ($25.89 CAD)|
| NOMA 2.5W | **Solar power for weather station.**<br>- 2.5W<br>- Built-in diode<br> - Using because on sale for $10 CAD / panel | N/A | [CanadianTire.ca](https://www.canadiantire.ca/en/pdp/noma-2-5w-solar-battery-maintainer-0110010p.html#srp) ($34.99 CAD, was on sale for $9.99 CAD)|

</div>

*Note that alternative SD cards, batteries, and solar panel(s) may be used. SD cards should be at least 4GB if you want to have some data storage capabilities for the Raspberry Pi. A large capacity battery may also be used, if available. The solar panels used on my setup were chosen because they were a good price, but panel(s) with greater power output would probably be recommended. 

<style>
.parts-table table tr td:nth-child(1) {
        vertical-align: text-top;
        width: 25%;
        max-width: 100px;
}
.parts-table table tr td:nth-child(2) {
        vertical-align: text-top;
        width: 40%;
        max-width: 100px;
}
.parts-table table tr td:nth-child(3) {
        vertical-align: text-top;
        width: 15%;
        overflow: hidden;
        max-width: 100px;
}
.parts-table table tr td:nth-child(4) {
        vertical-align: text-top;
        width: 20%;
        overflow: hidden;
        max-width: 100px;
}
</style>



