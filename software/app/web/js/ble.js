// MediStride — Gestion BLE (Web Bluetooth API)
//
// UUIDs définis dans firmware/src/config.h
export const SERVICE_UUID       = '4d454449-5354-5249-4445-000000000001';
export const PRESSURE_CHAR_UUID = '4d454449-5354-5249-4445-000000000002';
export const IMU_CHAR_UUID      = '4d454449-5354-5249-4445-000000000003';
export const BATTERY_CHAR_UUID  = '4d454449-5354-5249-4445-000000000004';

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
            // Lecture initiale
            const batValue = await this.batteryChar.readValue();
            this.dispatchEvent(new CustomEvent('battery', { detail: { level: batValue.getUint8(0), side: this.side } }));
        } catch (err) {
            console.warn(`[${this.side}] Caractéristique batterie indisponible`, err);
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
}
