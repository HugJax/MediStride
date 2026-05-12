// MediStride — Rendu canvas (cartographie de pression sur un pied)

// Positions normalisées des 6 capteurs (x, y) dans le repère canvas [0..1]
// Conforme à l'ordre de config.h :
// FSR1 talon centre, FSR2 talon latéral, FSR3 arche, FSR4 1er méta, FSR5 3-4e méta, FSR6 gros orteil
const SENSOR_POSITIONS_RIGHT = [
    { x: 0.50, y: 0.86, label: 'T-C' },   // FSR1 talon centre
    { x: 0.65, y: 0.82, label: 'T-L' },   // FSR2 talon latéral
    { x: 0.55, y: 0.62, label: 'Arche' }, // FSR3
    { x: 0.35, y: 0.30, label: '1M' },    // FSR4 1er méta (médial)
    { x: 0.65, y: 0.32, label: '3-4M' },  // FSR5 3-4e méta
    { x: 0.32, y: 0.14, label: 'O1' },    // FSR6 gros orteil
];

// Symétrie axe vertical pour le pied gauche
const SENSOR_POSITIONS_LEFT = SENSOR_POSITIONS_RIGHT.map(p => ({ ...p, x: 1 - p.x }));

// Couleur en fonction de la pression normalisée [0..1]
function pressureColor(t) {
    t = Math.max(0, Math.min(1, t));
    // Bleu → vert → jaune → rouge
    const r = t < 0.5 ? Math.floor(t * 2 * 255) : 255;
    const g = t < 0.5 ? 200 : Math.floor((1 - (t - 0.5) * 2) * 200);
    const b = t < 0.3 ? Math.floor((1 - t / 0.3) * 220) : 0;
    return `rgb(${r},${g},${b})`;
}

// Tracé du contour stylisé d'un pied
function drawFootOutline(ctx, w, h, side) {
    ctx.save();
    if (side === 'left') {
        ctx.translate(w, 0);
        ctx.scale(-1, 1);
    }

    ctx.beginPath();
    // Contour très simplifié vu de dessus, talon en bas
    ctx.moveTo(w * 0.30, h * 0.95);
    ctx.bezierCurveTo(w * 0.05, h * 0.90, w * 0.10, h * 0.55, w * 0.20, h * 0.40);
    ctx.bezierCurveTo(w * 0.20, h * 0.25, w * 0.18, h * 0.10, w * 0.30, h * 0.05);
    ctx.bezierCurveTo(w * 0.45, h * -0.02, w * 0.60, h * 0.05, w * 0.65, h * 0.18);
    ctx.bezierCurveTo(w * 0.78, h * 0.20, w * 0.85, h * 0.32, w * 0.78, h * 0.45);
    ctx.bezierCurveTo(w * 0.90, h * 0.65, w * 0.85, h * 0.88, w * 0.70, h * 0.95);
    ctx.closePath();

    ctx.fillStyle = '#f0f4f8';
    ctx.strokeStyle = '#cfd8dc';
    ctx.lineWidth = 2;
    ctx.fill();
    ctx.stroke();
    ctx.restore();
}

export function drawFoot(canvas, raw, side, maxAdc = 4095) {
    const ctx = canvas.getContext('2d');
    const w = canvas.width;
    const h = canvas.height;

    ctx.clearRect(0, 0, w, h);
    drawFootOutline(ctx, w, h, side);

    const positions = side === 'left' ? SENSOR_POSITIONS_LEFT : SENSOR_POSITIONS_RIGHT;

    if (!raw) return;

    for (let i = 0; i < positions.length; i++) {
        const p = positions[i];
        const t = Math.min(1, raw[i] / maxAdc);
        const cx = p.x * w;
        const cy = p.y * h;
        const radius = 18 + t * 22;

        // Halo
        const gradient = ctx.createRadialGradient(cx, cy, 0, cx, cy, radius);
        gradient.addColorStop(0, pressureColor(t));
        gradient.addColorStop(1, 'rgba(255,255,255,0)');
        ctx.fillStyle = gradient;
        ctx.beginPath();
        ctx.arc(cx, cy, radius, 0, Math.PI * 2);
        ctx.fill();

        // Cercle central
        ctx.beginPath();
        ctx.arc(cx, cy, 8, 0, Math.PI * 2);
        ctx.fillStyle = '#fff';
        ctx.fill();
        ctx.strokeStyle = pressureColor(t);
        ctx.lineWidth = 2;
        ctx.stroke();

        // Numéro
        ctx.fillStyle = '#333';
        ctx.font = 'bold 10px sans-serif';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(String(i + 1), cx, cy);
    }
}
