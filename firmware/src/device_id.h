#ifndef MEDISTRIDE_DEVICE_ID_H
#define MEDISTRIDE_DEVICE_ID_H

#include "config.h"

// Détermine le rôle du module au boot :
//  1. Lit la broche SIDE_SELECT_PIN avec pull-up interne :
//     - non connectée (lue HIGH) → pied droit (R)
//     - reliée à GND (lue LOW)   → pied gauche (L)
//  2. Si forcé via NVS (commande BLE de configuration), utilise cette valeur.
//
// Permet de distribuer un seul binaire firmware et de choisir le rôle
// par un simple pont de soudure sur la PCB.

enum DeviceSide {
    DEVICE_SIDE_LEFT,
    DEVICE_SIDE_RIGHT
};

void device_id_init();
DeviceSide device_id_get_side();

// Retourne "MediStride-L" ou "MediStride-R" selon le côté détecté.
const char *device_id_get_name();

// Force le côté en NVS (persistant après reboot). Utilisé pour reconfigurer
// un module sans modifier le pont de soudure.
void device_id_force_side(DeviceSide side);
void device_id_clear_forced();

#endif // MEDISTRIDE_DEVICE_ID_H
