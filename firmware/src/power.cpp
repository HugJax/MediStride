#include "power.h"
#include <esp_sleep.h>
#include <esp_adc_cal.h>

// Pour le XIAO ESP32-S3, la tension batterie est mesurée via un diviseur
// 2× 220 kΩ entre BAT+ et GND, milieu vers GPIO8 (D9, A7 selon les variantes).
// Vbat = 2 × Vadc.
#define BATTERY_ADC_PIN     8     // GPIO8 (D9 sur XIAO ESP32-S3)
#define BATTERY_DIVIDER     2.0f
#define BATTERY_MAX_MV      4200  // LiPo pleine
#define BATTERY_MIN_MV      3400  // LiPo « vide » (déchargée)

// Temps d'inactivité (BLE non connecté) avant deep sleep
#define IDLE_SLEEP_TIMEOUT_MS    (5 * 60 * 1000UL)  // 5 minutes

static unsigned long last_ble_seen_ms = 0;
static bool adc_calibrated = false;
static esp_adc_cal_characteristics_t adc_chars;

void power_init() {
    analogReadResolution(ADC_RESOLUTION);
    pinMode(BATTERY_ADC_PIN, INPUT);

    // Caractérisation ADC pour conversion mV.
    // ADC_ATTEN_DB_12 = plage ~0-3,3 V sur ESP32-S3 (remplace ADC_ATTEN_DB_11 déprécié
    // dans platform-espressif32 >= 6.5, épinglé dans platformio.ini).
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12,
                              1100, &adc_chars);
    adc_calibrated = true;

    last_ble_seen_ms = millis();
    DEBUG_PRINTLN("[POWER] Module batterie initialisé");
}

uint16_t power_read_battery_mv() {
    // Moyenne de 8 lectures pour réduire le bruit
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 8; i++) sum += analogRead(BATTERY_ADC_PIN);
    uint16_t raw = sum / 8;

    uint32_t mv_adc = adc_calibrated
        ? esp_adc_cal_raw_to_voltage(raw, &adc_chars)
        : (raw * 3300UL / ADC_MAX_VALUE);

    return (uint16_t)(mv_adc * BATTERY_DIVIDER);
}

uint8_t power_read_battery_percent() {
    uint16_t mv = power_read_battery_mv();
    if (mv >= BATTERY_MAX_MV) return 100;
    if (mv <= BATTERY_MIN_MV) return 0;
    return (uint8_t)(((uint32_t)(mv - BATTERY_MIN_MV) * 100UL)
                     / (BATTERY_MAX_MV - BATTERY_MIN_MV));
}

void power_check_idle_sleep(bool ble_connected) {
    if (ble_connected) {
        last_ble_seen_ms = millis();
        return;
    }
    if (millis() - last_ble_seen_ms < IDLE_SLEEP_TIMEOUT_MS) return;

    DEBUG_PRINTLN("[POWER] Inactivité prolongée — passage en deep sleep");
    DEBUG_PRINTLN("[POWER] Appuyer sur RESET pour réveiller");
    delay(100);

    // Réveil possible par bouton BOOT (GPIO0 sur XIAO ESP32-S3)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
    esp_deep_sleep_start();
}
