#include "ble_server.h"

static NimBLEServer *pServer = nullptr;
static NimBLECharacteristic *pPressureChar = nullptr;
static NimBLECharacteristic *pIMUChar = nullptr;
static NimBLECharacteristic *pBatteryChar = nullptr;
static bool deviceConnected = false;

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override {
        deviceConnected = true;
        DEBUG_PRINTLN("[BLE] Client connecté");

        // Optimiser les paramètres de connexion pour le temps réel
        pServer->updateConnParams(
            connInfo.getConnHandle(),
            BLE_CONN_MIN_INTERVAL,
            BLE_CONN_MAX_INTERVAL,
            0,    // latence
            400   // timeout supervision (ms)
        );
    }

    void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override {
        deviceConnected = false;
        DEBUG_PRINTF("[BLE] Client déconnecté (raison: %d)\n", reason);

        // Redémarrer l'advertising
        NimBLEDevice::startAdvertising();
        DEBUG_PRINTLN("[BLE] Advertising redémarré");
    }
};

void ble_init() {
    NimBLEDevice::init(DEVICE_NAME);
    NimBLEDevice::setPower(ESP_PWR_LVL_P6);  // +6 dBm

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Service MediStride
    NimBLEService *pService = pServer->createService(SERVICE_UUID);

    // Caractéristique Pression (6x uint16 + timestamp)
    pPressureChar = pService->createCharacteristic(
        PRESSURE_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    // Caractéristique IMU (optionnel)
    pIMUChar = pService->createCharacteristic(
        IMU_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    // Caractéristique Batterie
    pBatteryChar = pService->createCharacteristic(
        BATTERY_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pService->start();

    // Configuration de l'advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    DEBUG_PRINTLN("[BLE] Serveur démarré");
    DEBUG_PRINTF("[BLE] Nom: %s\n", DEVICE_NAME);
}

bool ble_is_connected() {
    return deviceConnected;
}

void ble_notify_pressure(const uint8_t *buffer, uint8_t length) {
    if (!deviceConnected) return;

    pPressureChar->setValue(buffer, length);
    pPressureChar->notify();
}

void ble_notify_imu(const uint8_t *buffer, uint8_t length) {
    if (!deviceConnected) return;

    pIMUChar->setValue(buffer, length);
    pIMUChar->notify();
}

void ble_update_battery(uint8_t level) {
    if (!deviceConnected) return;

    pBatteryChar->setValue(&level, 1);
    pBatteryChar->notify();
}