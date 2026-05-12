#include "ble_server.h"
#include "device_id.h"
#include "calibration.h"
#include <string.h>

// Définie dans main.cpp
void main_set_time_offset(uint32_t app_ms_now);

static NimBLEServer *pServer = nullptr;
static NimBLECharacteristic *pPressureChar = nullptr;
static NimBLECharacteristic *pIMUChar = nullptr;
static NimBLECharacteristic *pBatteryChar = nullptr;
static NimBLECharacteristic *pConfigChar = nullptr;
static bool deviceConnected = false;

// Codes des commandes acceptées via la caractéristique CONFIG.
// Format général : 1 octet de code, suivi des paramètres en little-endian.
enum ConfigCmd : uint8_t {
    CMD_SYNC_TIME       = 0x01,  // + uint32 ms_app
    CMD_SET_CAL         = 0x02,  // + uint8 sensor + float slope + float intercept
    CMD_SAVE_CAL        = 0x03,
    CMD_RESET_CAL       = 0x04,
    CMD_FORCE_SIDE_L    = 0x05,
    CMD_FORCE_SIDE_R    = 0x06,
    CMD_CLEAR_FORCE     = 0x07,
};

// NimBLE-Arduino 1.4.x : signatures avec ble_gap_conn_desc*.
// Si tu migres en NimBLE 2.x, il faudra revenir à NimBLEConnInfo&.
class ConfigCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *chr) override {
        std::string val = chr->getValue();
        if (val.empty()) return;
        const uint8_t *data = (const uint8_t *)val.data();
        size_t len = val.size();

        switch (data[0]) {
            case CMD_SYNC_TIME:
                if (len >= 5) {
                    uint32_t app_ms = (uint32_t)data[1]
                                    | ((uint32_t)data[2] << 8)
                                    | ((uint32_t)data[3] << 16)
                                    | ((uint32_t)data[4] << 24);
                    main_set_time_offset(app_ms);
                }
                break;

            case CMD_SET_CAL:
                if (len >= 10) {
                    uint8_t sensor = data[1];
                    float slope, intercept;
                    memcpy(&slope, &data[2], 4);
                    memcpy(&intercept, &data[6], 4);
                    calibration_set(sensor, slope, intercept);
                    DEBUG_PRINTF("[CFG] Cal capteur %u : slope=%f intercept=%f\n",
                                 sensor, slope, intercept);
                }
                break;

            case CMD_SAVE_CAL:
                calibration_save();
                break;

            case CMD_RESET_CAL:
                calibration_reset();
                break;

            case CMD_FORCE_SIDE_L:
                device_id_force_side(DEVICE_SIDE_LEFT);
                break;
            case CMD_FORCE_SIDE_R:
                device_id_force_side(DEVICE_SIDE_RIGHT);
                break;
            case CMD_CLEAR_FORCE:
                device_id_clear_forced();
                break;
        }
    }
};

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer *server, ble_gap_conn_desc *desc) override {
        deviceConnected = true;
        DEBUG_PRINTLN("[BLE] Client connecté");

        server->updateConnParams(
            desc->conn_handle,
            BLE_CONN_MIN_INTERVAL,
            BLE_CONN_MAX_INTERVAL,
            0,
            400
        );
    }

    void onDisconnect(NimBLEServer *server) override {
        deviceConnected = false;
        DEBUG_PRINTLN("[BLE] Client déconnecté");
        NimBLEDevice::startAdvertising();
        DEBUG_PRINTLN("[BLE] Advertising redémarré");
    }
};

void ble_init() {
    NimBLEDevice::init(device_id_get_name());
    NimBLEDevice::setPower(ESP_PWR_LVL_P6);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    NimBLEService *pService = pServer->createService(SERVICE_UUID);

    pPressureChar = pService->createCharacteristic(
        PRESSURE_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pIMUChar = pService->createCharacteristic(
        IMU_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pBatteryChar = pService->createCharacteristic(
        BATTERY_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pConfigChar = pService->createCharacteristic(
        CONFIG_CHAR_UUID,
        NIMBLE_PROPERTY::WRITE
    );
    pConfigChar->setCallbacks(new ConfigCallbacks());

    pService->start();

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setName(device_id_get_name());
    pAdvertising->start();

    DEBUG_PRINTLN("[BLE] Serveur démarré");
    DEBUG_PRINTF("[BLE] Nom: %s\n", device_id_get_name());
}

bool ble_is_connected() {
    return deviceConnected;
}

void ble_notify_pressure(const uint8_t *buffer, uint8_t length) {
    if (!deviceConnected || !pPressureChar) return;
    pPressureChar->setValue(buffer, length);
    pPressureChar->notify();
}

void ble_notify_imu(const uint8_t *buffer, uint8_t length) {
    if (!deviceConnected || !pIMUChar) return;
    pIMUChar->setValue(buffer, length);
    pIMUChar->notify();
}

void ble_update_battery(uint8_t level) {
    if (!deviceConnected || !pBatteryChar) return;
    pBatteryChar->setValue(&level, 1);
    pBatteryChar->notify();
}
