// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include <Wire.h>

const int MPU_addr = 0x68;  // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
void setup()
{
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    Serial.begin(9600);
}
void loop()
{
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr ,14, true);  // request a total of 14 registers
    AcX = (Wire.read() << 8) | Wire.read();
    AcY = (Wire.read() << 8) | Wire.read();
    AcZ = (Wire.read() << 8) | Wire.read();
    Tmp = (Wire.read() << 8) | Wire.read();
    GyX = (Wire.read() << 8) | Wire.read();
    GyY = (Wire.read() << 8) | Wire.read();
    GyZ = (Wire.read() << 8) | Wire.read();

    Serial.print(AcX); Serial.print(" ");
    Serial.print(AcY); Serial.print(" ");
    Serial.print(AcZ); Serial.print(" ");
    Serial.print(Tmp/340 + 36.53); Serial.print(" ");
    Serial.print(GyX); Serial.print(" ");
    Serial.print(GyY); Serial.print(" ");
    Serial.println(GyZ);

    delay(10);        // Delay for the DLPF
}
