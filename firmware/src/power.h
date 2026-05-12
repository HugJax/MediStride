#ifndef MEDISTRIDE_POWER_H
#define MEDISTRIDE_POWER_H

#include "config.h"

// Initialise l'ADC pour la lecture batterie.
void power_init();

// Lecture du niveau batterie en pourcentage (0-100).
// Estimation linéaire entre BATTERY_MIN_MV (4 %) et BATTERY_MAX_MV (100 %).
uint8_t power_read_battery_percent();

// Retourne la tension batterie en millivolts.
uint16_t power_read_battery_mv();

// Endort le module si aucune connexion BLE depuis IDLE_SLEEP_TIMEOUT_MS.
// Réveil par bouton (broche BOOT) ou par horloge interne.
void power_check_idle_sleep(bool ble_connected);

#endif // MEDISTRIDE_POWER_H
