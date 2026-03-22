#ifndef MEDISTRIDE_IMU_H
#define MEDISTRIDE_IMU_H

#include "config.h"

#ifdef IMU_ENABLED

#include <Wire.h>

// Données brutes de l'IMU
struct IMUData {
    int16_t accel_x;    // Accélération X (mg)
    int16_t accel_y;    // Accélération Y (mg)
    int16_t accel_z;    // Accélération Z (mg)
    int16_t gyro_x;     // Vitesse angulaire X (°/s × 10)
    int16_t gyro_y;     // Vitesse angulaire Y (°/s × 10)
    int16_t gyro_z;     // Vitesse angulaire Z (°/s × 10)
    uint32_t timestamp_ms;
};

// Initialise le MPU-6050 via I2C
// Retourne true si le capteur est détecté
bool imu_init();

// Lit les données accéléromètre + gyroscope
void imu_read(IMUData &data);

// Encode les données IMU dans un buffer pour BLE (16 octets : 6x int16 + timestamp)
uint8_t imu_encode(const IMUData &data, uint8_t *buffer, uint8_t buffer_size);

#endif // IMU_ENABLED
#endif // MEDISTRIDE_IMU_H