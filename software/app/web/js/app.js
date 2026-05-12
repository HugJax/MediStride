// MediStride — Logique principale de l'application

import { MediStrideDevice, adcToForce } from './ble.js';
import { drawFoot } from './viz.js';
import { GaitAnalyzer } from './analysis.js';
import { buildCsv, downloadCsv } from './export.js';

const state = {
    devices: { left: null, right: null },
    latest: { left: null, right: null },
    analyzer: new GaitAnalyzer(),
    recording: false,
    sessionStartIso: null,
    samples: [],
};

// Vérifie le support Web Bluetooth au chargement
function checkBleSupport() {
    const warning = document.getElementById('ble-warning');
    if (!navigator.bluetooth) {
        warning.innerHTML = '⚠️ Votre navigateur ne supporte pas Web Bluetooth. ' +
            'Utilisez <strong>Chrome</strong> ou <strong>Edge</strong> sur ordinateur ou Android. ' +
            'Web Bluetooth n\'est pas disponible sur iOS.';
        document.getElementById('btn-connect-left').disabled = true;
        document.getElementById('btn-connect-right').disabled = true;
        return false;
    }
    return true;
}

async function connectSide(side) {
    const dev = new MediStrideDevice(side);
    state.devices[side] = dev;

    dev.addEventListener('pressure', (e) => onPressure(e.detail));
    dev.addEventListener('battery', (e) => onBattery(e.detail));
    dev.addEventListener('disconnected', () => onDisconnect(side));

    try {
        await dev.connect();
        document.getElementById(`status-${side}`).classList.add('connected');
        updateView();
    } catch (err) {
        console.error(err);
        alert(`Échec de la connexion du pied ${side === 'left' ? 'gauche' : 'droit'} :\n${err.message}`);
        state.devices[side] = null;
    }
}

function onDisconnect(side) {
    document.getElementById(`status-${side}`).classList.remove('connected');
    state.devices[side] = null;
    updateView();
}

function onPressure({ raw, timestampMs, side }) {
    const forces = raw.map(adcToForce);
    const totalForce = forces.reduce((a, b) => a + b, 0);
    state.latest[side] = { raw, forces, totalForce, timestampMs };

    if (state.recording) {
        state.samples.push({
            side, raw, forces, totalForce, timestampMs,
            receivedAt: new Date().toISOString()
        });
        state.analyzer.addSample({ side, totalForce, timestampMs });
    }

    redraw(side);
}

function onBattery({ level, side }) {
    const el = document.getElementById(`metric-battery-${side}`);
    if (el) el.textContent = `${level} %`;
}

function redraw(side) {
    const canvas = document.getElementById(`canvas-${side}`);
    const data = state.latest[side];
    if (canvas && data) {
        drawFoot(canvas, data.raw, side);
        document.getElementById(`total-${side}`).textContent = Math.round(data.totalForce);
    }
}

function updateView() {
    const anyConnected = state.devices.left || state.devices.right;
    document.getElementById('view-disconnected').classList.toggle('active', !anyConnected);
    document.getElementById('view-session').classList.toggle('active', !!anyConnected);
}

function startSession() {
    state.analyzer.reset();
    state.samples = [];
    state.recording = true;
    state.sessionStartIso = new Date().toISOString();
    document.getElementById('btn-start').disabled = true;
    document.getElementById('btn-stop').disabled = false;
    document.getElementById('btn-export').disabled = true;
}

function stopSession() {
    state.recording = false;
    document.getElementById('btn-start').disabled = false;
    document.getElementById('btn-stop').disabled = true;
    document.getElementById('btn-export').disabled = state.samples.length === 0;
}

function exportSession() {
    const patientId = document.getElementById('patient-id').value.trim() || 'anonyme';
    const csv = buildCsv(state.samples, patientId, state.sessionStartIso);
    const dateTag = state.sessionStartIso.replace(/[:.]/g, '-').slice(0, 19);
    downloadCsv(csv, `medistride-${patientId}-${dateTag}.csv`);
}

function refreshMetrics() {
    if (!state.recording) return;
    const dur = state.analyzer.durationSec();
    const mm = String(Math.floor(dur / 60)).padStart(2, '0');
    const ss = String(Math.floor(dur % 60)).padStart(2, '0');
    document.getElementById('metric-duration').textContent = `${mm}:${ss}`;

    const sym = state.analyzer.symmetryPercent();
    document.getElementById('metric-symmetry').textContent = sym === null ? '—' : `${sym.toFixed(0)} %`;

    const cad = state.analyzer.cadenceStepsPerMin();
    document.getElementById('metric-cadence').textContent = cad === null ? '—' : cad;

    document.getElementById('metric-samples').textContent = state.samples.length;
}

// Pré-tracé du contour
function initCanvases() {
    drawFoot(document.getElementById('canvas-left'), null, 'left');
    drawFoot(document.getElementById('canvas-right'), null, 'right');
}

document.addEventListener('DOMContentLoaded', () => {
    checkBleSupport();
    initCanvases();
    updateView();

    document.getElementById('btn-connect-left').addEventListener('click', () => connectSide('left'));
    document.getElementById('btn-connect-right').addEventListener('click', () => connectSide('right'));
    document.getElementById('btn-start').addEventListener('click', startSession);
    document.getElementById('btn-stop').addEventListener('click', stopSession);
    document.getElementById('btn-export').addEventListener('click', exportSession);

    setInterval(refreshMetrics, 200);

    if ('serviceWorker' in navigator) {
        navigator.serviceWorker.register('service-worker.js').catch(err => {
            console.warn('Service worker non enregistré:', err);
        });
    }
});
