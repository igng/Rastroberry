// MPU-6050 Filtering
// By Andrea Monti (igng)
// Base on code of Arduino User JohnChi
// Public Domain
#include <Wire.h>

const int MPU_addr = 0x68;                      // I2C address of the MPU-6050
int16_t raw_x_acc[3]    = {0};
int16_t raw_y_acc[3]    = {0};
int16_t raw_z_acc[3]    = {0};
int16_t raw_temp[3]     = {0};
int16_t raw_x_gyro[3]   = {0};
int16_t raw_y_gyro[3]   = {0};
int16_t raw_z_gyro[3]   = {0};
int16_t filt_x_acc[3]   = {0};
int16_t filt_y_acc[3]   = {0};
int16_t filt_z_acc[3]   = {0};
int16_t filt_temp[3]    = {0};
int16_t filt_x_gyro[3]  = {0};
int16_t filt_y_gyro[3]  = {0};
int16_t filt_z_gyro[3]  = {0};

void update()
{
    raw_x_acc[2]  = raw_x_acc[1];
    raw_y_acc[2]  = raw_y_acc[1];
    raw_z_acc[2]  = raw_z_acc[1];
    raw_temp[2]   = raw_temp[1];
    raw_x_gyro[2] = raw_x_gyro[1];
    raw_y_gyro[2] = raw_y_gyro[1];
    raw_z_gyro[2] = raw_z_gyro[1] ;
    
    raw_x_acc[1]  = raw_x_acc[0];
    raw_y_acc[1]  = raw_y_acc[0];
    raw_z_acc[1]  = raw_z_acc[0];
    raw_temp[1]   = raw_temp[0];
    raw_x_gyro[1] = raw_x_gyro[0];
    raw_y_gyro[1] = raw_y_gyro[0];
    raw_z_gyro[1] = raw_z_gyro[0];
    
    filt_x_acc[2]  = filt_x_acc[1];
    filt_y_acc[2]  = filt_y_acc[1];
    filt_z_acc[2]  = filt_z_acc[1];
    filt_temp[2]   = filt_temp[1];
    filt_x_gyro[2] = filt_x_gyro[1];
    filt_y_gyro[2] = filt_y_gyro[1];
    filt_z_gyro[2] = filt_z_gyro[1];
    
    filt_x_acc[1]  = filt_x_acc[0];
    filt_y_acc[1]  = filt_y_acc[0];
    filt_z_acc[1]  = filt_z_acc[0];
    filt_temp[1]   = filt_temp[0];
    filt_x_gyro[1] = filt_x_gyro[0];
    filt_y_gyro[1] = filt_y_gyro[0];
    filt_z_gyro[1] = filt_z_gyro[0];
}

void setup()
{
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);                            // PWR_MGMT_1 (Power Menagement register)
    Wire.write(0);                               // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    Serial.begin(9600);
}
void loop()
{
    update();                                   // update of the local variables for the filter
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);                           // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr ,14, true);       // request a total of 14 registers
    raw_x_acc[0]  = (Wire.read() << 8) | Wire.read();
    raw_y_acc[0]  = (Wire.read() << 8) | Wire.read();
    raw_z_acc[0]  = (Wire.read() << 8) | Wire.read();
    raw_temp[0]   = (Wire.read() << 8) | Wire.read();
    raw_x_gyro[0] = (Wire.read() << 8) | Wire.read();
    raw_y_gyro[0] = (Wire.read() << 8) | Wire.read();
    raw_z_gyro[0] = (Wire.read() << 8) | Wire.read();
    
    filt_x_acc[0]  = 1.8*filt_x_acc[1] - 0.81*filt_x_acc[2] + 0.0022676*(raw_x_acc[0] + 2*raw_x_acc[1] + raw_x_acc[2]);
    filt_y_acc[0]  = 1.8*filt_y_acc[1] - 0.81*filt_y_acc[2] + 0.0022676*(raw_y_acc[0] + 2*raw_y_acc[1] + raw_y_acc[2]);
    filt_z_acc[0]  = 1.8*filt_z_acc[1] - 0.81*filt_z_acc[2] + 0.0022676*(raw_z_acc[0] + 2*raw_z_acc[1] + raw_z_acc[2]);
    filt_temp[0]   = 1.8*filt_temp[1] - 0.81*filt_temp[2] + 0.0022676*(raw_temp[0] + 2*raw_temp[1] + raw_temp[2]);
    filt_x_gyro[0] = 1.8*filt_x_gyro[1] - 0.81*filt_x_gyro[2] + 0.0022676*(raw_x_gyro[0] + 2*raw_x_gyro[1] + raw_x_gyro[2]);
    filt_y_gyro[0] = 1.8*filt_y_gyro[1] - 0.81*filt_y_gyro[2] + 0.0022676*(raw_y_gyro[0] + 2*raw_y_gyro[1] + raw_y_gyro[2]);
    filt_z_gyro[0] = 1.8*filt_z_gyro[1] - 0.81*filt_z_gyro[2] + 0.0022676*(raw_z_gyro[0] + 2*raw_z_gyro[1] + raw_z_gyro[2]);

    Serial.print(filt_x_acc[0]); Serial.print(" ");
    Serial.print(filt_y_acc[0]); Serial.print(" ");
    Serial.print(filt_z_acc[0]); Serial.print(" ");
    Serial.print(filt_temp[0]/340 + 36.53); Serial.print(" ");
    Serial.print(filt_x_gyro[0]); Serial.print(" ");
    Serial.print(filt_y_gyro[0]); Serial.print(" ");
    Serial.println(filt_z_gyro[0]);

    delay(1);
}
