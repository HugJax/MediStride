#include "config.h"
#include "sensors.h"
#include "ble_server.h"
#include "imu.h"
#include "calibration.h"
#include "power.h"
#include "device_id.h"

// Données globales
static PressureData pressureData;
static uint8_t bleBuffer[20];

static unsigned long lastSampleTime = 0;
static unsigned long lastBatteryCheck = 0;
static unsigned long timeOffsetMs = 0;  // Offset appliqué au timestamp (sync app)

#ifdef IMU_ENABLED
static IMUData imuData;
static uint8_t imuBuffer[20];
static bool imuAvailable = false;
#endif

// Décale toutes les futures lectures vers une horloge commune envoyée par l'app.
// Appelée par ble_server quand l'app écrit la caractéristique CONFIG.
void main_set_time_offset(uint32_t app_ms_now) {
    timeOffsetMs = app_ms_now - millis();
    DEBUG_PRINTF("[MAIN] Sync horloge appliquée : offset = %lu ms\n", timeOffsetMs);
}

uint32_t main_get_synced_ms() {
    return millis() + timeOffsetMs;
}

void setup() {
    #ifdef DEBUG_SERIAL
    Serial.begin(115200);
    delay(1000);
    DEBUG_PRINTLN("================================");
    DEBUG_PRINTLN("  MediStride Firmware v1.1");
    DEBUG_PRINTLN("================================");
    #endif

    device_id_init();
    DEBUG_PRINTF("[MAIN] Module : %s\n", device_id_get_name());

    calibration_load();
    sensors_init();
    power_init();
    ble_init();

    #ifdef IMU_ENABLED
    imuAvailable = imu_init();
    if (imuAvailable) {
        DEBUG_PRINTLN("[MAIN] IMU MPU-6050 activée");
    } else {
        DEBUG_PRINTLN("[MAIN] IMU non détectée — mode pression seule");
    }
    #endif

    DEBUG_PRINTLN("[MAIN] Initialisation terminée");
    DEBUG_PRINTF("[MAIN] Échantillonnage: %d Hz\n", SAMPLE_RATE_HZ);
}

void loop() {
    unsigned long now = millis();

    if (now - lastSampleTime < SAMPLE_INTERVAL_MS) {
        // Vérifie périodiquement la mise en veille si non connecté
        power_check_idle_sleep(ble_is_connected());
        return;
    }
    lastSampleTime = now;

    // Lecture des capteurs (utilise la calibration NVS)
    sensors_read(pressureData);

    if (ble_is_connected()) {
        uint8_t len = sensors_encode(pressureData, bleBuffer, sizeof(bleBuffer));
        if (len > 0) {
            ble_notify_pressure(bleBuffer, len);
        }

        #ifdef IMU_ENABLED
        if (imuAvailable) {
            imu_read(imuData);
            uint8_t imuLen = imu_encode(imuData, imuBuffer, sizeof(imuBuffer));
            if (imuLen > 0) {
                ble_notify_imu(imuBuffer, imuLen);
            }
        }
        #endif

        if (now - lastBatteryCheck >= BATTERY_CHECK_INTERVAL_MS) {
            lastBatteryCheck = now;
            uint8_t battery = power_read_battery_percent();
            ble_update_battery(battery);

            if (battery <= BATTERY_LOW_THRESHOLD) {
                DEBUG_PRINTF("[MAIN] Batterie faible : %u%%\n", battery);
            }
        }
    }

    #ifdef DEBUG_SERIAL
    static unsigned long lastDebug = 0;
    if (now - lastDebug >= 1000) {
        lastDebug = now;
        DEBUG_PRINTF("[DATA] t=%lu | ", pressureData.timestamp_ms);
        for (uint8_t i = 0; i < FSR_COUNT; i++) {
            DEBUG_PRINTF("FSR%d:%d(%.1fN) ", i + 1,
                pressureData.sensors[i].raw,
                pressureData.sensors[i].force_n);
        }
        DEBUG_PRINTLN();

        #ifdef IMU_ENABLED
        if (imuAvailable) {
            DEBUG_PRINTF("[IMU]  ax:%d ay:%d az:%d | gx:%d gy:%d gz:%d\n",
                imuData.accel_x, imuData.accel_y, imuData.accel_z,
                imuData.gyro_x, imuData.gyro_y, imuData.gyro_z);
        }
        #endif
    }
    #endif

    power_check_idle_sleep(ble_is_connected());
}
