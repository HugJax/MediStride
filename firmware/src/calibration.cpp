#include "calibration.h"
#include <Preferences.h>

static SensorCalibration s_cal[FSR_COUNT];
static const char *NVS_NAMESPACE = "medistride";
static const char *NVS_VERSION_KEY = "cal_v";
static const uint16_t CAL_VERSION = 1;

// Calibration linéaire par défaut, dérivée de la formule conductance / force du A301.
// pente ≈ FSR_MAX_FORCE_N / ADC_max_practique. Servira tant que l'utilisateur
// n'a pas calibré manuellement.
static const float DEFAULT_SLOPE = FSR_MAX_FORCE_N / 3700.0f;  // ≈ 0.120 N/LSB
static const float DEFAULT_INTERCEPT = 0.0f;

static void load_defaults() {
    for (uint8_t i = 0; i < FSR_COUNT; i++) {
        s_cal[i].slope = DEFAULT_SLOPE;
        s_cal[i].intercept = DEFAULT_INTERCEPT;
    }
}

void calibration_load() {
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, /* readonly = */ true)) {
        load_defaults();
        return;
    }

    uint16_t version = prefs.getUShort(NVS_VERSION_KEY, 0);
    if (version != CAL_VERSION) {
        prefs.end();
        load_defaults();
        return;
    }

    char key_slope[8];
    char key_off[8];
    for (uint8_t i = 0; i < FSR_COUNT; i++) {
        snprintf(key_slope, sizeof(key_slope), "s%u", i);
        snprintf(key_off, sizeof(key_off), "i%u", i);
        s_cal[i].slope = prefs.getFloat(key_slope, DEFAULT_SLOPE);
        s_cal[i].intercept = prefs.getFloat(key_off, DEFAULT_INTERCEPT);
    }
    prefs.end();

    DEBUG_PRINTLN("[CAL] Calibration chargée depuis NVS");
}

void calibration_save() {
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, /* readonly = */ false)) {
        DEBUG_PRINTLN("[CAL] Erreur ouverture NVS en écriture");
        return;
    }

    prefs.putUShort(NVS_VERSION_KEY, CAL_VERSION);
    char key_slope[8];
    char key_off[8];
    for (uint8_t i = 0; i < FSR_COUNT; i++) {
        snprintf(key_slope, sizeof(key_slope), "s%u", i);
        snprintf(key_off, sizeof(key_off), "i%u", i);
        prefs.putFloat(key_slope, s_cal[i].slope);
        prefs.putFloat(key_off, s_cal[i].intercept);
    }
    prefs.end();

    DEBUG_PRINTLN("[CAL] Calibration sauvegardée dans NVS");
}

SensorCalibration calibration_get(uint8_t sensor_index) {
    if (sensor_index >= FSR_COUNT) return { DEFAULT_SLOPE, DEFAULT_INTERCEPT };
    return s_cal[sensor_index];
}

void calibration_set(uint8_t sensor_index, float slope, float intercept) {
    if (sensor_index >= FSR_COUNT) return;
    s_cal[sensor_index].slope = slope;
    s_cal[sensor_index].intercept = intercept;
}

float calibration_adc_to_force(uint8_t sensor_index, uint16_t adc_value) {
    if (sensor_index >= FSR_COUNT) return 0.0f;
    float force = s_cal[sensor_index].slope * (float)adc_value + s_cal[sensor_index].intercept;
    if (force < 0.0f) force = 0.0f;
    if (force > FSR_MAX_FORCE_N) force = FSR_MAX_FORCE_N;
    return force;
}

void calibration_reset() {
    load_defaults();
    calibration_save();
}
