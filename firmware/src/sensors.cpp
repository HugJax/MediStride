#include "sensors.h"
#include "calibration.h"

// Définie dans main.cpp pour permettre la synchronisation d'horloge G/D.
extern uint32_t main_get_synced_ms();

void sensors_init() {
    analogReadResolution(ADC_RESOLUTION);

    for (uint8_t i = 0; i < FSR_COUNT; i++) {
        pinMode(FSR_PINS[i], INPUT);
    }

    DEBUG_PRINTLN("[SENSORS] Initialisés — 6x FlexiForce A301 sur ADC");
}

void sensors_read(PressureData &data) {
    data.timestamp_ms = main_get_synced_ms();

    for (uint8_t i = 0; i < FSR_COUNT; i++) {
        // Moyenne de 4 lectures pour réduire le bruit
        uint32_t sum = 0;
        for (uint8_t s = 0; s < 4; s++) {
            sum += analogRead(FSR_PINS[i]);
        }
        data.sensors[i].raw = sum / 4;
        data.sensors[i].force_n = calibration_adc_to_force(i, data.sensors[i].raw);
    }
}

float sensors_adc_to_force(uint16_t adc_value) {
    if (adc_value == 0) return 0.0f;

    // Tension mesurée
    // Vout = adc_value / ADC_MAX_VALUE * 3.3V
    float vout = (float)adc_value / ADC_MAX_VALUE * 3.3f;

    // Diviseur de tension : Vout = 3.3 * R_pull / (R_fsr + R_pull)
    // Donc : R_fsr = R_pull * (3.3 / Vout - 1)
    if (vout >= 3.29f) {
        // Saturation — force maximale
        return FSR_MAX_FORCE_N;
    }

    float r_fsr = PULLDOWN_RESISTANCE * (3.3f / vout - 1.0f);

    // Le FlexiForce A301 a une conductance (1/R) proportionnelle à la force
    // G = 1/R_fsr est proportionnel à F
    // Calibration linéaire simplifiée : F = FSR_MAX_FORCE_N * (G / G_max)
    // G_max correspond à R_fsr minimum (~1kΩ à 445N)
    float g_max = 1.0f / 1000.0f;  // Conductance à force maximale
    float g = 1.0f / r_fsr;

    float force = FSR_MAX_FORCE_N * (g / g_max);

    // Borner entre 0 et max
    if (force < 0.0f) force = 0.0f;
    if (force > FSR_MAX_FORCE_N) force = FSR_MAX_FORCE_N;

    return force;
}

uint8_t sensors_encode(const PressureData &data, uint8_t *buffer, uint8_t buffer_size) {
    // Format : 6x uint16 little-endian (valeurs ADC brutes) + 4 octets timestamp
    // Total : 16 octets
    const uint8_t required = FSR_COUNT * 2 + 4;
    if (buffer_size < required) return 0;

    uint8_t offset = 0;
    for (uint8_t i = 0; i < FSR_COUNT; i++) {
        buffer[offset++] = data.sensors[i].raw & 0xFF;
        buffer[offset++] = (data.sensors[i].raw >> 8) & 0xFF;
    }

    // Timestamp (32 bits little-endian)
    buffer[offset++] = data.timestamp_ms & 0xFF;
    buffer[offset++] = (data.timestamp_ms >> 8) & 0xFF;
    buffer[offset++] = (data.timestamp_ms >> 16) & 0xFF;
    buffer[offset++] = (data.timestamp_ms >> 24) & 0xFF;

    return offset;
}