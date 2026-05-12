#ifndef MEDISTRIDE_CALIBRATION_H
#define MEDISTRIDE_CALIBRATION_H

#include "config.h"

// Coefficients de conversion ADC → Newtons par capteur.
// Modèle linéaire : force_n = slope * adc + intercept
struct SensorCalibration {
    float slope;       // N par unité ADC
    float intercept;   // N à ADC = 0
};

// Charge les calibrations depuis la NVS (Preferences). Si absente, valeurs par défaut.
void calibration_load();

// Sauvegarde les calibrations dans la NVS.
void calibration_save();

// Lecture / écriture d'une calibration capteur (index 0..FSR_COUNT-1).
SensorCalibration calibration_get(uint8_t sensor_index);
void calibration_set(uint8_t sensor_index, float slope, float intercept);

// Conversion ADC → Newtons en utilisant la calibration courante.
float calibration_adc_to_force(uint8_t sensor_index, uint16_t adc_value);

// Réinitialise toutes les calibrations aux valeurs par défaut (formule théorique).
void calibration_reset();

#endif // MEDISTRIDE_CALIBRATION_H
