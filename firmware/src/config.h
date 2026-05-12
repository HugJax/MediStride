#ifndef MEDISTRIDE_CONFIG_H
#define MEDISTRIDE_CONFIG_H

#include <Arduino.h>

// ============================================================
// Identification du module
// ============================================================
// Le nom BLE est désormais déterminé automatiquement au boot par device_id.cpp :
//  - GPIO9 (D10) relié à GND   → "MediStride-L" (gauche)
//  - GPIO9 (D10) laissé flottant → "MediStride-R" (droit)
// Le nom peut aussi être forcé par BLE via la caractéristique de configuration.
// DEVICE_NAME ci-dessous n'est plus utilisé que comme fallback.
#define DEVICE_NAME "MediStride-?"

// ============================================================
// Broches ADC — Capteurs FlexiForce A301
// ============================================================
// Correspondance : voir hardware/schematics/README.md
static const uint8_t FSR_PINS[] = {
    A0,  // GPIO1 — FSR1 : Talon (centre)
    A1,  // GPIO2 — FSR2 : Talon (latéral)
    A2,  // GPIO3 — FSR3 : Milieu du pied (arche)
    A3,  // GPIO4 — FSR4 : 1er métatarse
    A4,  // GPIO5 — FSR5 : 3e-4e métatarse
    A5,  // GPIO6 — FSR6 : Gros orteil
};
static const uint8_t FSR_COUNT = sizeof(FSR_PINS) / sizeof(FSR_PINS[0]);

// ============================================================
// Configuration ADC
// ============================================================
#define ADC_RESOLUTION     12         // 12 bits (0-4095)
#define ADC_ATTEN          ADC_11db   // Plage 0-3.3V
#define ADC_MAX_VALUE      4095

// ============================================================
// Échantillonnage
// ============================================================
#define SAMPLE_RATE_HZ     20        // Fréquence d'échantillonnage par capteur
#define SAMPLE_INTERVAL_MS (1000 / SAMPLE_RATE_HZ)  // 50 ms

// ============================================================
// Calibration capteurs
// ============================================================
// Force maximale du FlexiForce A301 en Newtons
#define FSR_MAX_FORCE_N    445.0f

// Résistance pull-down en Ohms
#define PULLDOWN_RESISTANCE 10000.0f

// ============================================================
// BLE — Identifiants du service GATT
// ============================================================
// UUID personnalisés pour MediStride
#define SERVICE_UUID        "4d454449-5354-5249-4445-000000000001"
#define PRESSURE_CHAR_UUID  "4d454449-5354-5249-4445-000000000002"
#define IMU_CHAR_UUID       "4d454449-5354-5249-4445-000000000003"
#define BATTERY_CHAR_UUID   "4d454449-5354-5249-4445-000000000004"
// Caractéristique de configuration en écriture (calibration, choix côté, sync horloge)
#define CONFIG_CHAR_UUID    "4d454449-5354-5249-4445-000000000005"

// Intervalle de connexion BLE (ms)
#define BLE_CONN_MIN_INTERVAL  15
#define BLE_CONN_MAX_INTERVAL  30

// ============================================================
// IMU — MPU-6050 (optionnel, activer via -DIMU_ENABLED)
// ============================================================
#ifdef IMU_ENABLED
    #define IMU_SDA_PIN    43   // GPIO43 (D6)
    #define IMU_SCL_PIN    44   // GPIO44 (D7)
    #define IMU_I2C_ADDR   0x68 // AD0 → GND
    #define IMU_SAMPLE_RATE_HZ 20
#endif

// ============================================================
// Batterie
// ============================================================
#define BATTERY_CHECK_INTERVAL_MS 30000  // Vérifier toutes les 30s
#define BATTERY_LOW_THRESHOLD     10     // Seuil bas en pourcentage

// ============================================================
// Debug
// ============================================================
// Décommenter pour activer les logs série
#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
    #define DEBUG_PRINT(x)   Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(...)
#endif

#endif // MEDISTRIDE_CONFIG_H