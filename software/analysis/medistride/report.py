"""Affichage texte et graphiques pour une session."""

from __future__ import annotations

from pathlib import Path

from .session import Session, FSR_COLUMNS_N
from .metrics import (
    cadence_steps_per_minute,
    detect_heel_strikes,
    symmetry_percent,
    zone_distribution,
)


def print_summary(session: Session) -> None:
    duration = session.duration_sec()
    n_left = len(detect_heel_strikes(session.left()))
    n_right = len(detect_heel_strikes(session.right()))
    sym = symmetry_percent(session)
    cad = cadence_steps_per_minute(session)

    print("=" * 60)
    print(f"  Session MediStride — patient {session.patient_id}")
    print(f"  Début : {session.start_iso}")
    print("=" * 60)
    print(f"  Durée                : {duration:.1f} s")
    print(f"  Échantillons         : {len(session.samples)} (G={len(session.left())} D={len(session.right())})")
    print(f"  Pas détectés         : G={n_left}  D={n_right}  total={n_left + n_right}")
    print(f"  Symétrie G/D         : {sym:.1f} %" if sym is not None else "  Symétrie G/D         : N/A")
    print(f"  Cadence              : {cad:.0f} pas/min" if cad is not None else "  Cadence              : N/A")
    print()

    for side in ("left", "right"):
        label = "gauche" if side == "left" else "droit"
        zones = zone_distribution(session, side)
        print(f"  Répartition pied {label} :")
        for zone, pct in zones.items():
            print(f"    {zone:<14s} : {pct:5.1f} %")
        print()


def save_plots(session: Session, out_dir: Path) -> None:
    """Sauvegarde les graphiques principaux à côté du CSV."""
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("matplotlib non installé — graphiques sautés")
        return

    out_dir.mkdir(exist_ok=True)
    base = out_dir / f"medistride-{session.patient_id}"

    # 1) Force totale au cours du temps
    fig, ax = plt.subplots(figsize=(10, 4))
    for side, color in (("left", "tab:green"), ("right", "tab:red")):
        df = session.samples[session.samples["side"] == side]
        if df.empty:
            continue
        t0 = df["timestamp_ms_device"].iloc[0]
        ax.plot(
            (df["timestamp_ms_device"] - t0) / 1000.0,
            df["total_force_n"],
            color=color,
            label=f"Pied {'gauche' if side == 'left' else 'droit'}",
            linewidth=1.0,
        )
    ax.set_xlabel("Temps (s)")
    ax.set_ylabel("Force totale (N)")
    ax.set_title(f"Force totale — patient {session.patient_id}")
    ax.grid(True, alpha=0.3)
    ax.legend()
    fig.tight_layout()
    fig.savefig(f"{base}-force-totale.png", dpi=120)
    plt.close(fig)

    # 2) Répartition moyenne par capteur (bar chart)
    fig, axes = plt.subplots(1, 2, figsize=(10, 4), sharey=True)
    for ax, side in zip(axes, ("left", "right")):
        df = session.samples[session.samples["side"] == side]
        if df.empty:
            ax.text(0.5, 0.5, "Aucune donnée", ha="center", va="center")
            ax.set_title("Pied " + ("gauche" if side == "left" else "droit"))
            continue
        means = df[FSR_COLUMNS_N].mean()
        ax.bar(range(1, 7), means, color="tab:green" if side == "left" else "tab:red")
        ax.set_xticks(range(1, 7))
        ax.set_xlabel("Capteur")
        ax.set_title(f"Pied {'gauche' if side == 'left' else 'droit'}")
    axes[0].set_ylabel("Force moyenne (N)")
    fig.suptitle(f"Force moyenne par capteur — patient {session.patient_id}")
    fig.tight_layout()
    fig.savefig(f"{base}-repartition.png", dpi=120)
    plt.close(fig)
