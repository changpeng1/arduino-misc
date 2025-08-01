// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

#define LED_PIN 13
bool blinkState = false;

int16_t standard_ax = 0,standard_ay = 0,standard_az = 0;
int16_t standard_gx = 0,standard_gy = 0,standard_gz = 0;

int16_t max_ax_drift = 0;
void calibration()
{
  int16_t ax[10], ay[10], az[10];
  int16_t gx[10], gy[10], gz[10];
  int32_t t_ax = 0,t_ay = 0,t_az = 0;
  int32_t t_gx = 0,t_gy = 0,t_gz = 0;
  int16_t max_ax = 0;
  for(int i = 0;i<10;i++)
  {
    accelgyro.getMotion6(&ax[i], &ay[i], &az[i], &gx[i], &gy[i], &gz[i]);
    t_ax += ax[i];
    t_ay += ay[i];
    t_az += az[i];
    t_gx += gx[i];
    t_gy += gy[i];
    t_gz += gz[i];
    //max_ax = max_ax > ax[i]?max_ax
    Serial.print(ax[i]); Serial.print("\t");
  }
  standard_ax = t_ax/10;
  standard_ay = t_ay/10;
  standard_az = t_az/10;
  standard_gx = t_gx/10;
  standard_gy = t_gy/10;
  standard_gz = t_gz/10;
  for(int i = 0;i< 9;i++)
  {
    int16_t drift = abs(ax[i]-standard_ax);
    int16_t drift1 = abs(ax[i+1]-standard_ax);
    max_ax_drift = drift > drift1?drift:drift1;
    }
    Serial.print(t_ax);Serial.print("\t");
    Serial.print(max_ax_drift);Serial.print("\t");
    Serial.println(standard_ax);
  }
void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(9600);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    
        accelgyro.setXAccelOffset(-5466);
    accelgyro.setYAccelOffset(-1103);
    accelgyro.setZAccelOffset(1095);
    accelgyro.setXGyroOffset(-17);
    accelgyro.setYGyroOffset(-39);
    accelgyro.setZGyroOffset(-60);
    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);
    calibration();
}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    // display tab-separated accel/gyro x/y/z values
    
    /*
    if(abs(ax-standard_ax)> max_ax_drift * 3)
    {
      Serial.print("a/g:\t");
      Serial.print(standard_ax); Serial.print("\t");
      Serial.print(max_ax_drift*3); Serial.print("\t");
      Serial.println(ax);
    }
    */
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);
    //Serial.println(standard_ax);
    
    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}
