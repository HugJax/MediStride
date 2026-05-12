// MediStride — Analyse de marche en temps réel
//
// Algorithmes :
//   - Symétrie G/D : ratio des forces moyennes pondéré et normalisé
//   - Cadence : détection de pics de force totale (heel strike) par fenêtre glissante

const FOOT_CONTACT_THRESHOLD_N = 50;  // Force totale minimale pour considérer un appui
const STRIKE_REFRACTORY_MS = 300;     // Temps minimum entre deux pas du même pied

export class GaitAnalyzer {
    constructor() {
        this.reset();
    }

    reset() {
        this.startTime = null;
        this.lastStrikeLeft = 0;
        this.lastStrikeRight = 0;
        this.strikesLeft = [];
        this.strikesRight = [];
        this.totalForceLeftSum = 0;
        this.totalForceRightSum = 0;
        this.sampleCountLeft = 0;
        this.sampleCountRight = 0;
        this.prevTotalLeft = 0;
        this.prevTotalRight = 0;
    }

    // sample : { side, totalForce, timestampMs }
    addSample(sample) {
        const now = Date.now();
        if (this.startTime === null) this.startTime = now;

        if (sample.side === 'left') {
            this.totalForceLeftSum += sample.totalForce;
            this.sampleCountLeft++;
            // Détection de pas : transition < seuil → ≥ seuil
            if (this.prevTotalLeft < FOOT_CONTACT_THRESHOLD_N &&
                sample.totalForce >= FOOT_CONTACT_THRESHOLD_N &&
                now - this.lastStrikeLeft > STRIKE_REFRACTORY_MS) {
                this.strikesLeft.push(now);
                this.lastStrikeLeft = now;
            }
            this.prevTotalLeft = sample.totalForce;
        } else {
            this.totalForceRightSum += sample.totalForce;
            this.sampleCountRight++;
            if (this.prevTotalRight < FOOT_CONTACT_THRESHOLD_N &&
                sample.totalForce >= FOOT_CONTACT_THRESHOLD_N &&
                now - this.lastStrikeRight > STRIKE_REFRACTORY_MS) {
                this.strikesRight.push(now);
                this.lastStrikeRight = now;
            }
            this.prevTotalRight = sample.totalForce;
        }
    }

    durationSec() {
        if (this.startTime === null) return 0;
        return (Date.now() - this.startTime) / 1000;
    }

    // Symétrie : 100 % = forces moyennes identiques G et D.
    // Calculée comme 100 × (1 - |L-R| / (L+R))
    symmetryPercent() {
        const meanL = this.sampleCountLeft ? this.totalForceLeftSum / this.sampleCountLeft : 0;
        const meanR = this.sampleCountRight ? this.totalForceRightSum / this.sampleCountRight : 0;
        const total = meanL + meanR;
        if (total < 1) return null;
        return (1 - Math.abs(meanL - meanR) / total) * 100;
    }

    // Cadence : pas par minute, fenêtre glissante 10 s.
    cadenceStepsPerMin() {
        const cutoff = Date.now() - 10000;
        const recentL = this.strikesLeft.filter(t => t >= cutoff).length;
        const recentR = this.strikesRight.filter(t => t >= cutoff).length;
        const totalSteps = recentL + recentR;
        if (totalSteps < 2) return null;
        // Extrapolation sur 60 secondes
        return Math.round(totalSteps * 6);
    }

    stepCount() {
        return this.strikesLeft.length + this.strikesRight.length;
    }
}
