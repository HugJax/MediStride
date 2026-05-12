#include "imu.h"

#ifdef IMU_ENABLED

// Registres MPU-6050
#define MPU6050_REG_PWR_MGMT_1   0x6B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_GYRO_CONFIG  0x1B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_WHO_AM_I     0x75

static void imu_write_reg(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(IMU_I2C_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

static uint8_t imu_read_reg(uint8_t reg) {
    Wire.beginTransmission(IMU_I2C_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)IMU_I2C_ADDR, (uint8_t)1);
    return Wire.read();
}

bool imu_init() {
    Wire.begin(IMU_SDA_PIN, IMU_SCL_PIN);
    Wire.setClock(400000);  // I2C 400 kHz

    // Vérifier la présence du MPU-6050
    uint8_t who_am_i = imu_read_reg(MPU6050_REG_WHO_AM_I);
    if (who_am_i != 0x68) {
        DEBUG_PRINTF("[IMU] MPU-6050 non détecté (WHO_AM_I: 0x%02X)\n", who_am_i);
        return false;
    }

    // Sortir du mode sleep
    imu_write_reg(MPU6050_REG_PWR_MGMT_1, 0x00);
    delay(10);

    // Accéléromètre : ±4g (sensibilité 8192 LSB/g)
    imu_write_reg(MPU6050_REG_ACCEL_CONFIG, 0x08);

    // Gyroscope : ±500°/s (sensibilité 65.5 LSB/°/s)
    imu_write_reg(MPU6050_REG_GYRO_CONFIG, 0x08);

    DEBUG_PRINTLN("[IMU] MPU-6050 initialisé (±4g, ±500°/s)");
    return true;
}

void imu_read(IMUData &data) {
    data.timestamp_ms = millis();

    // Lire 14 octets à partir de ACCEL_XOUT_H
    // (accel XYZ, temp, gyro XYZ)
    Wire.beginTransmission(IMU_I2C_ADDR);
    Wire.write(MPU6050_REG_ACCEL_XOUT_H);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)IMU_I2C_ADDR, (uint8_t)14);

    // Accéléromètre (raw → mg)
    // Sensibilité ±4g : 8192 LSB/g
    int16_t raw_ax = (Wire.read() << 8) | Wire.read();
    int16_t raw_ay = (Wire.read() << 8) | Wire.read();
    int16_t raw_az = (Wire.read() << 8) | Wire.read();

    // Température (ignorée)
    Wire.read();
    Wire.read();

    // Gyroscope (raw → °/s × 10)
    // Sensibilité ±500°/s : 65.5 LSB/°/s
    int16_t raw_gx = (Wire.read() << 8) | Wire.read();
    int16_t raw_gy = (Wire.read() << 8) | Wire.read();
    int16_t raw_gz = (Wire.read() << 8) | Wire.read();

    // Conversion
    data.accel_x = (int16_t)(raw_ax * 1000.0f / 8192.0f);
    data.accel_y = (int16_t)(raw_ay * 1000.0f / 8192.0f);
    data.accel_z = (int16_t)(raw_az * 1000.0f / 8192.0f);

    data.gyro_x = (int16_t)(raw_gx * 10.0f / 65.5f);
    data.gyro_y = (int16_t)(raw_gy * 10.0f / 65.5f);
    data.gyro_z = (int16_t)(raw_gz * 10.0f / 65.5f);
}

uint8_t imu_encode(const IMUData &data, uint8_t *buffer, uint8_t buffer_size) {
    // Format : 6x int16 little-endian + 4 octets timestamp = 16 octets
    const uint8_t required = 16;
    if (buffer_size < required) return 0;

    uint8_t offset = 0;
    auto write_int16 = [&](int16_t val) {
        buffer[offset++] = val & 0xFF;
        buffer[offset++] = (val >> 8) & 0xFF;
    };

    write_int16(data.accel_x);
    write_int16(data.accel_y);
    write_int16(data.accel_z);
    write_int16(data.gyro_x);
    write_int16(data.gyro_y);
    write_int16(data.gyro_z);

    buffer[offset++] = data.timestamp_ms & 0xFF;
    buffer[offset++] = (data.timestamp_ms >> 8) & 0xFF;
    buffer[offset++] = (data.timestamp_ms >> 16) & 0xFF;
    buffer[offset++] = (data.timestamp_ms >> 24) & 0xFF;

    return offset;
}

#endif // IMU_ENABLED