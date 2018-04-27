#include <Arduino.h>
#include <BMI160Gen.h>

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767

  float g = (gRaw * 250.0) / 32768.0;

  return g;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // initialize device
  Serial.println("Initializing IMU device...");
  BMI160.begin(BMI160GenClass::I2C_MODE);
  //BMI160.begin(BMI160GenClass::I2C_MODE);
  uint8_t dev_id = BMI160.getDeviceID();
  Serial.print("DEVICE ID: ");
  Serial.println(dev_id, HEX);

   // Set the accelerometer range to 250 degrees/second
  BMI160.setGyroRange(250);
  Serial.println("Initializing IMU device...done.");
}

void loop() {
  int gxRaw, gyRaw, gzRaw;         // raw gyro values
  float gx, gy, gz;

  // read raw gyro measurements from device
  BMI160.readGyro(gxRaw, gyRaw, gzRaw);

  // convert the raw gyro data to degrees/second
  gx = convertRawGyro(gxRaw);
  gy = convertRawGyro(gyRaw);
  gz = convertRawGyro(gzRaw);

  // display tab-separated gyro x/y/z values
  Serial.print("g:\t");
  Serial.print(gx);
  Serial.print("\t");
  Serial.print(gy);
  Serial.print("\t");
  Serial.print(gz);
  Serial.print("\t");

  BMI160.readAccelerometer(gxRaw, gyRaw, gzRaw);
  Serial.print("a:\t");
  Serial.print(gxRaw);
  Serial.print("\t");
  Serial.print(gyRaw);
  Serial.print("\t");
  Serial.print(gzRaw);
  Serial.println();

  delay(10);
}
