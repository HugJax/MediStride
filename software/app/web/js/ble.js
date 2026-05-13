// MediStride — Gestion BLE (Web Bluetooth API)
//
// UUIDs définis dans firmware/src/config.h
export const SERVICE_UUID       = '4d454449-5354-5249-4445-000000000001';
export const PRESSURE_CHAR_UUID = '4d454449-5354-5249-4445-000000000002';
export const IMU_CHAR_UUID      = '4d454449-5354-5249-4445-000000000003';
export const BATTERY_CHAR_UUID  = '4d454449-5354-5249-4445-000000000004';
export const CONFIG_CHAR_UUID   = '4d454449-5354-5249-4445-000000000005';

// Codes de commandes pour la caractéristique CONFIG. Doivent rester en phase
// avec l'énumération ConfigCmd côté firmware (firmware/src/ble_server.cpp).
export const CMD_SYNC_TIME    = 0x01;
export const CMD_SET_CAL      = 0x02;
export const CMD_SAVE_CAL     = 0x03;
export const CMD_RESET_CAL    = 0x04;
export const CMD_FORCE_SIDE_L = 0x05;
export const CMD_FORCE_SIDE_R = 0x06;
export const CMD_CLEAR_FORCE  = 0x07;

const FSR_COUNT = 6;

// Décodage du paquet pression : 6× uint16 little-endian + uint32 timestamp = 16 octets
export function decodePressure(dataView) {
    if (dataView.byteLength < FSR_COUNT * 2 + 4) return null;
    const raw = new Array(FSR_COUNT);
    for (let i = 0; i < FSR_COUNT; i++) {
        raw[i] = dataView.getUint16(i * 2, true);
    }
    const timestampMs = dataView.getUint32(FSR_COUNT * 2, true);
    return { raw, timestampMs };
}

// Conversion ADC → Newtons (approximation, voir sensors.cpp)
// Reproduit la formule firmware pour une visualisation cohérente.
const ADC_MAX = 4095;
const PULLDOWN_OHM = 10000;
const FSR_MAX_FORCE_N = 445;
const G_MAX = 1 / 1000;

export function adcToForce(adc) {
    if (adc === 0) return 0;
    const vout = (adc / ADC_MAX) * 3.3;
    if (vout >= 3.29) return FSR_MAX_FORCE_N;
    const rFsr = PULLDOWN_OHM * (3.3 / vout - 1);
    const force = FSR_MAX_FORCE_N * ((1 / rFsr) / G_MAX);
    return Math.max(0, Math.min(FSR_MAX_FORCE_N, force));
}

export class MediStrideDevice extends EventTarget {
    constructor(side) {
        super();
        this.side = side; // 'left' ou 'right'
        this.device = null;
        this.server = null;
        this.pressureChar = null;
        this.batteryChar = null;
        this.configChar = null;
        this.connected = false;
    }

    async connect() {
        if (!navigator.bluetooth) {
            throw new Error('Web Bluetooth non supporté par ce navigateur. Utilisez Chrome ou Edge.');
        }

        const namePrefix = this.side === 'left' ? 'MediStride-L' : 'MediStride-R';

        this.device = await navigator.bluetooth.requestDevice({
            filters: [{ namePrefix }],
            optionalServices: [SERVICE_UUID]
        });

        this.device.addEventListener('gattserverdisconnected', () => this._onDisconnect());

        this.server = await this.device.gatt.connect();
        const service = await this.server.getPrimaryService(SERVICE_UUID);

        this.pressureChar = await service.getCharacteristic(PRESSURE_CHAR_UUID);
        this.pressureChar.addEventListener('characteristicvaluechanged', (e) => {
            const data = decodePressure(e.target.value);
            if (data) {
                this.dispatchEvent(new CustomEvent('pressure', { detail: { ...data, side: this.side } }));
            }
        });
        await this.pressureChar.startNotifications();

        try {
            this.batteryChar = await service.getCharacteristic(BATTERY_CHAR_UUID);
            this.batteryChar.addEventListener('characteristicvaluechanged', (e) => {
                const level = e.target.value.getUint8(0);
                this.dispatchEvent(new CustomEvent('battery', { detail: { level, side: this.side } }));
            });
            await this.batteryChar.startNotifications();
            const batValue = await this.batteryChar.readValue();
            this.dispatchEvent(new CustomEvent('battery', { detail: { level: batValue.getUint8(0), side: this.side } }));
        } catch (err) {
            console.warn(`[${this.side}] Caractéristique batterie indisponible`, err);
        }

        try {
            this.configChar = await service.getCharacteristic(CONFIG_CHAR_UUID);
        } catch (err) {
            console.warn(`[${this.side}] Caractéristique CONFIG indisponible (firmware ancien)`, err);
        }

        this.connected = true;
        this.dispatchEvent(new CustomEvent('connected'));
    }

    _onDisconnect() {
        this.connected = false;
        this.dispatchEvent(new CustomEvent('disconnected'));
    }

    disconnect() {
        if (this.device && this.device.gatt.connected) {
            this.device.gatt.disconnect();
        }
    }

    // --- Caractéristique CONFIG ---

    async _writeConfig(cmd, payload = new Uint8Array(0)) {
        if (!this.configChar) {
            throw new Error('Caractéristique CONFIG indisponible — mettre à jour le firmware.');
        }
        const buffer = new Uint8Array(1 + payload.byteLength);
        buffer[0] = cmd;
        buffer.set(payload, 1);
        await this.configChar.writeValue(buffer);
    }

    // Aligne l'horloge interne du module sur `referenceMs` (millisecondes
    // depuis epoch ou tout autre référence commune). Indispensable pour
    // comparer les timestamps des deux pieds.
    async syncTime(referenceMs = Date.now()) {
        const payload = new Uint8Array(4);
        new DataView(payload.buffer).setUint32(0, referenceMs >>> 0, true);
        await this._writeConfig(CMD_SYNC_TIME, payload);
    }

    // Définit la calibration d'un capteur (force_n = slope*adc + intercept).
    // `sensorIndex` est 0-based (0..5).
    async setCalibration(sensorIndex, slope, intercept) {
        const payload = new Uint8Array(9);
        const view = new DataView(payload.buffer);
        view.setUint8(0, sensorIndex);
        view.setFloat32(1, slope, true);
        view.setFloat32(5, intercept, true);
        await this._writeConfig(CMD_SET_CAL, payload);
    }

    async saveCalibration() { return this._writeConfig(CMD_SAVE_CAL); }
    async resetCalibration() { return this._writeConfig(CMD_RESET_CAL); }

    async forceSide(side) {
        return this._writeConfig(side === 'left' ? CMD_FORCE_SIDE_L : CMD_FORCE_SIDE_R);
    }
    async clearForcedSide() { return this._writeConfig(CMD_CLEAR_FORCE); }
}
