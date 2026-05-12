#include "device_id.h"
#include <Preferences.h>

// Broche utilisée pour détecter le côté.
// HIGH (pull-up, non connectée)  → R (droit)
// LOW  (reliée à GND)             → L (gauche)
#ifndef SIDE_SELECT_PIN
#define SIDE_SELECT_PIN 9   // GPIO9 / D8 par défaut
#endif

static const char *NVS_NAMESPACE = "medistride";
static const char *NVS_KEY_FORCED = "side_forced";
static const char *NVS_KEY_SIDE = "side_value";

static DeviceSide s_side = DEVICE_SIDE_LEFT;
static char s_name[16] = "MediStride-?";

void device_id_init() {
    // Surcharge compile-time (utilisée par le CI pour produire des binaires
    // déjà figés en gauche/droit, distribués via flash.html).
    #if defined(FORCE_SIDE_LEFT)
        s_side = DEVICE_SIDE_LEFT;
        DEBUG_PRINTLN("[ID] Côté forcé à la compilation : gauche");
    #elif defined(FORCE_SIDE_RIGHT)
        s_side = DEVICE_SIDE_RIGHT;
        DEBUG_PRINTLN("[ID] Côté forcé à la compilation : droit");
    #else
        Preferences prefs;
        prefs.begin(NVS_NAMESPACE, /* readonly = */ true);
        bool forced = prefs.getBool(NVS_KEY_FORCED, false);
        if (forced) {
            s_side = prefs.getUChar(NVS_KEY_SIDE, 0) == 0 ? DEVICE_SIDE_LEFT : DEVICE_SIDE_RIGHT;
            prefs.end();
            DEBUG_PRINTF("[ID] Côté forcé via NVS : %s\n",
                         s_side == DEVICE_SIDE_LEFT ? "gauche" : "droit");
        } else {
            prefs.end();
            pinMode(SIDE_SELECT_PIN, INPUT_PULLUP);
            delay(2);
            int v = digitalRead(SIDE_SELECT_PIN);
            s_side = (v == LOW) ? DEVICE_SIDE_LEFT : DEVICE_SIDE_RIGHT;
            DEBUG_PRINTF("[ID] Côté détecté via GPIO%d : %s\n",
                         SIDE_SELECT_PIN, s_side == DEVICE_SIDE_LEFT ? "gauche" : "droit");
        }
    #endif

    snprintf(s_name, sizeof(s_name), "MediStride-%c",
             s_side == DEVICE_SIDE_LEFT ? 'L' : 'R');
}

DeviceSide device_id_get_side() { return s_side; }
const char *device_id_get_name() { return s_name; }

void device_id_force_side(DeviceSide side) {
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, false)) return;
    prefs.putBool(NVS_KEY_FORCED, true);
    prefs.putUChar(NVS_KEY_SIDE, side == DEVICE_SIDE_LEFT ? 0 : 1);
    prefs.end();
    DEBUG_PRINTF("[ID] Côté forcé sauvegardé : %s\n",
                 side == DEVICE_SIDE_LEFT ? "gauche" : "droit");
}

void device_id_clear_forced() {
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, false)) return;
    prefs.remove(NVS_KEY_FORCED);
    prefs.remove(NVS_KEY_SIDE);
    prefs.end();
    DEBUG_PRINTLN("[ID] Force côté effacée — relire au prochain boot");
}
