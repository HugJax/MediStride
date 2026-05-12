"""Point d'entrée : `python -m medistride <session.csv>`."""

import argparse
import sys
from pathlib import Path

from .session import Session
from .report import print_summary, save_plots


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        prog="medistride",
        description="Analyse d'une session MediStride exportée en CSV",
    )
    parser.add_argument("csv", type=Path, help="Fichier CSV exporté par l'application web")
    parser.add_argument(
        "--no-plots", action="store_true", help="Ne pas générer les graphiques PNG"
    )
    args = parser.parse_args(argv)

    if not args.csv.is_file():
        print(f"Fichier introuvable : {args.csv}", file=sys.stderr)
        return 1

    session = Session.from_csv(args.csv)
    print_summary(session)

    if not args.no_plots:
        out_dir = args.csv.parent
        save_plots(session, out_dir)
        print(f"\nGraphiques sauvegardés dans {out_dir}/")

    return 0


if __name__ == "__main__":
    sys.exit(main())
