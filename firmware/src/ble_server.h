#ifndef MEDISTRIDE_BLE_SERVER_H
#define MEDISTRIDE_BLE_SERVER_H

#include "config.h"
#include <NimBLEDevice.h>

// Initialise le serveur BLE et crée le service GATT MediStride
void ble_init();

// Retourne true si un client est connecté
bool ble_is_connected();

// Envoie les données de pression via notification BLE
// buffer : données encodées par sensors_encode()
// length : taille du buffer
void ble_notify_pressure(const uint8_t *buffer, uint8_t length);

// Envoie les données IMU via notification BLE (optionnel)
void ble_notify_imu(const uint8_t *buffer, uint8_t length);

// Met à jour le niveau de batterie (0-100%)
void ble_update_battery(uint8_t level);

#endif // MEDISTRIDE_BLE_SERVER_H