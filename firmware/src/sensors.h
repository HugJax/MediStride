#ifndef MEDISTRIDE_SENSORS_H
#define MEDISTRIDE_SENSORS_H

#include "config.h"

// Données d'un capteur FSR
struct FSRReading {
    uint16_t raw;       // Valeur ADC brute (0-4095)
    float    force_n;   // Force estimée en Newtons
};

// Données complètes d'une lecture (6 capteurs)
struct PressureData {
    FSRReading sensors[FSR_COUNT];
    uint32_t   timestamp_ms;
};

// Initialise les broches ADC
void sensors_init();

// Lit les 6 capteurs et remplit la structure PressureData
void sensors_read(PressureData &data);

// Convertit une valeur ADC brute en force (Newtons)
float sensors_adc_to_force(uint16_t adc_value);

// Encode les données de pression dans un buffer pour BLE (12 octets : 6x uint16)
// Retourne la taille du buffer
uint8_t sensors_encode(const PressureData &data, uint8_t *buffer, uint8_t buffer_size);

#endif // MEDISTRIDE_SENSORS_H