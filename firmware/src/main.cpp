#include "config.h"
#include "sensors.h"
#include "ble_server.h"
#include "imu.h"

// Données globales
static PressureData pressureData;
static uint8_t bleBuffer[20];  // Buffer BLE (max 20 octets par notification)

static unsigned long lastSampleTime = 0;
static unsigned long lastBatteryCheck = 0;

#ifdef IMU_ENABLED
static IMUData imuData;
static uint8_t imuBuffer[20];
static bool imuAvailable = false;
#endif

// Estimation simplifiée du niveau de batterie via la tension
// Le XIAO ESP32-S3 n'a pas de broche dédiée pour la lecture batterie,
// cette fonction est un placeholder pour une future implémentation
static uint8_t read_battery_level() {
    // TODO: Implémenter la lecture de la tension batterie
    // via un diviseur de tension sur une broche ADC libre
    return 100;
}

void setup() {
    #ifdef DEBUG_SERIAL
    Serial.begin(115200);
    delay(1000);
    DEBUG_PRINTLN("================================");
    DEBUG_PRINTLN("  MediStride Firmware v1.0");
    DEBUG_PRINTF("  Module: %s\n", DEVICE_NAME);
    DEBUG_PRINTLN("================================");
    #endif

    // Initialisation des capteurs de pression
    sensors_init();

    // Initialisation BLE
    ble_init();

    // Initialisation IMU (optionnel)
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

    // Échantillonnage à fréquence fixe
    if (now - lastSampleTime < SAMPLE_INTERVAL_MS) return;
    lastSampleTime = now;

    // Lecture des capteurs de pression
    sensors_read(pressureData);

    // Envoi BLE si connecté
    if (ble_is_connected()) {
        uint8_t len = sensors_encode(pressureData, bleBuffer, sizeof(bleBuffer));
        if (len > 0) {
            ble_notify_pressure(bleBuffer, len);
        }

        // Lecture et envoi IMU (optionnel)
        #ifdef IMU_ENABLED
        if (imuAvailable) {
            imu_read(imuData);
            uint8_t imuLen = imu_encode(imuData, imuBuffer, sizeof(imuBuffer));
            if (imuLen > 0) {
                ble_notify_imu(imuBuffer, imuLen);
            }
        }
        #endif

        // Vérification batterie périodique
        if (now - lastBatteryCheck >= BATTERY_CHECK_INTERVAL_MS) {
            lastBatteryCheck = now;
            uint8_t battery = read_battery_level();
            ble_update_battery(battery);

            if (battery <= BATTERY_LOW_THRESHOLD) {
                DEBUG_PRINTLN("[MAIN] Batterie faible !");
            }
        }
    }

    // Debug : afficher les valeurs toutes les secondes
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
}