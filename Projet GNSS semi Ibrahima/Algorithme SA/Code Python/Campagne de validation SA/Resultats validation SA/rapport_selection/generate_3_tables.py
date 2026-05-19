from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt


BASE = Path(__file__).resolve().parent
SRC = BASE.parent / "merged_cpu_fpga"
OUT = BASE


def _format_number(value):
    if isinstance(value, float):
        return f"{value:,.2f}".replace(",", "X").replace(".", ",").replace("X", " ")
    return str(value)


def render_table(df: pd.DataFrame, title: str, filename: str):
    fig_h = max(2.2, 0.7 + 0.45 * (len(df) + 1))
    fig_w = max(9.0, 1.5 * len(df.columns))
    fig, ax = plt.subplots(figsize=(fig_w, fig_h), dpi=180)
    ax.axis("off")

    text_df = df.copy()
    for col in text_df.columns:
        text_df[col] = text_df[col].map(_format_number)

    table = ax.table(
        cellText=text_df.values,
        colLabels=text_df.columns,
        loc="center",
        cellLoc="center",
    )
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1, 1.5)

    for (r, c), cell in table.get_celld().items():
        cell.set_edgecolor("#2f2f2f")
        if r == 0:
            cell.set_facecolor("#1f4e78")
            cell.set_text_props(color="white", weight="bold")
        else:
            cell.set_facecolor("#f7fbff" if r % 2 else "#eaf2fb")

    ax.set_title(title, fontsize=13, fontweight="bold", pad=16)
    fig.tight_layout()
    fig.savefig(OUT / filename, bbox_inches="tight")
    plt.close(fig)


def build_tables():
    global_df = pd.read_csv(SRC / "merged_table_global.csv")
    agreement_df = pd.read_csv(SRC / "merged_table_agreement.csv")
    perf_df = pd.read_csv(SRC / "merged_table_cpu_fpga.csv")

    table1 = global_df[[
        "method_label",
        "n_signals",
        "strict_success_pct",
        "Pd_pct",
        "Pfa_pct",
        "doppler_mae_hz",
        "phase_mae_chip",
    ]].rename(columns={
        "method_label": "Methode",
        "n_signals": "Signaux",
        "strict_success_pct": "Succes strict (%)",
        "Pd_pct": "Pd (%)",
        "Pfa_pct": "Pfa (%)",
        "doppler_mae_hz": "Doppler MAE (Hz)",
        "phase_mae_chip": "Phase MAE (chip)",
    })

    row = agreement_df.iloc[0]
    table2 = pd.DataFrame([{
        "Fichiers communs": int(row["n_common"]),
        "Accord decision (%)": float(row["decision_agree_pct"]),
        "Accord PRN (%)": float(row["prn_agree_pct"]),
        "Accord Doppler (%)": float(row["doppler_agree_pct"]),
        "Accord Phase (%)": float(row["phase_agree_pct"]),
        "Delta peak ratio moyen": float(row["peak_ratio_delta_mean"]),
    }])

    table3 = perf_df[[
        "method_label",
        "time_mean_ms",
        "time_median_ms",
        "time_p95_ms",
        "speedup_vs_cpu",
    ]].rename(columns={
        "method_label": "Methode",
        "time_mean_ms": "Temps moyen (ms)",
        "time_median_ms": "Temps median (ms)",
        "time_p95_ms": "Temps P95 (ms)",
        "speedup_vs_cpu": "Speedup vs CPU",
    })

    render_table(table1, "Tableau 1 - Validation globale CPU vs FPGA", "tableau_1_validation_globale.png")
    render_table(table2, "Tableau 2 - Concordance CPU vs FPGA", "tableau_2_concordance.png")
    render_table(table3, "Tableau 3 - Analyse temporelle", "tableau_3_temps_speedup.png")


if __name__ == "__main__":
    build_tables()
    print(f"Tables generees dans: {OUT}")
