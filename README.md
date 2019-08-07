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

<div class="parts-table">

| Part Name | Function / Description | Data Sheet | Purchase Sources / Prices |
|-----------|------------------------|------------|---------------------------|
| DHT-22 | **Temperature / humidity sensing.**<br>- 2.5mA max current use during conversion<br>- 0-100% humidity readings with 2-5% accuracy<br>- 40 to 80°C temperature readings ±0.5°C accuracy<br>- No more than 0.5 Hz sampling rate (once every 2 seconds) | [DHT22 Datasheet (Sparkfun.com)](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf) | [Creatron](https://www.creatroninc.com/product/dht22-humidity-and-temperature-sensor/) ($13.00 CAD)<br>[Adafruit](https://www.adafruit.com/product/385) ($9.95 USD)<br>[Sparkfun](https://www.sparkfun.com/products/10167) ($9.95 USD)|
| BMP280 (BME280) | **Barometric pressure sensor.**<br>- ±1.0°C accuracy<br>- ±1 hPa accuracy<br>- I2C or SPI interface<br>- altitude, ±1 meter accuracy | [BST-BMP280-DS001 (bosch.com)](https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP280-DS001.pdf) | [Creatron](https://www.creatroninc.com/product/bmp280-barometric-sensor-300-to-1100hpa/) ($7.90 CAD)<br>[Adafruit](https://www.adafruit.com/product/2651) ($9.95 USD)<br>[Sparkfun - BME280](https://www.sparkfun.com/products/13676) ($19.95 USD)|
| Rev. P wind sensor | **Wind speed sensor.**<br>- Wind speeds measured: 0-150 MPH<br>- Ambient temperature compensation<br>- Accuracy of +/- 3 MPH (estimated) | [No Datasheet but info available here](https://moderndevice.com/uncategorized/calibrating-rev-p-wind-sensor-new-regression/?preview=true) | [Modern Device](https://moderndevice.com/product/wind-sensor-rev-p/) ($24 USD)|
| AMBI (GA1A1S201WP) | **Ambient light sensor.**<br>- Wide operating temperature range (-40 - +85 C)<br>- Gives consistent and sensible output readings (0 is pitch black, maximum sunlight is 1023)<br>- Won't over saturate in daylight | [GA1A1S201WP OPIC Light Detector (Mouser.ca)](https://www.mouser.ca/datasheet/2/365/GA1A1S201WP_Spec-184717.pdf) | [Modern Device](https://moderndevice.com/product/ambi-light-sensor/) ($3 USD)|
| LSM303DLHC | **Compass / accelerometer.**<br>- Compass and accelerometer all in one<br>- I2C interface<br>- Triple axis sensing | [LSM303DLHC (adafruit.ca)](https://cdn-shop.adafruit.com/datasheets/LSM303DLHC.PDF) | [Adafruit](https://www.adafruit.com/product/1120) ($14.95 USD)|


</div>

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

