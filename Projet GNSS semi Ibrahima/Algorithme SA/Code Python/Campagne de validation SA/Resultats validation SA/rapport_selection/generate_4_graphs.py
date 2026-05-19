from pathlib import Path
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


BASE = Path(__file__).resolve().parent
SRC = BASE.parent / "merged_cpu_fpga"
OUT = BASE


def _style():
    plt.rcParams["figure.dpi"] = 180
    plt.rcParams["font.size"] = 10
    plt.rcParams["axes.grid"] = True
    plt.rcParams["grid.alpha"] = 0.25
    plt.rcParams["axes.spines.top"] = False
    plt.rcParams["axes.spines.right"] = False


def graph_pd_pfa_pm_vs_snr(snr_df: pd.DataFrame):
    fig, ax = plt.subplots(figsize=(8.8, 4.8))
    colors = {"cpu_lut_angles": "#1f77b4", "fpga": "#ff7f0e"}

    for method, g in snr_df.groupby("method"):
        g = g.sort_values("snr_db")
        label = g["method_label"].iloc[0] if "method_label" in g.columns else method
        c = colors.get(method, None)
        ax.plot(g["snr_db"], g["Pd_pct"], marker="o", linewidth=2, color=c, label=f"{label} - Pd")
        ax.plot(g["snr_db"], g["Pfa_pct"], marker="x", linestyle="--", linewidth=1.6, color=c, alpha=0.9, label=f"{label} - Pfa")
        if "Pm_pct" in g.columns:
            ax.plot(g["snr_db"], g["Pm_pct"], marker="s", linestyle=":", linewidth=1.5, color=c, alpha=0.9, label=f"{label} - Pm")

    ax.set_title("Performance de detection vs SNR")
    ax.set_xlabel("SNR (dB)")
    ax.set_ylabel("Taux (%)")
    ax.set_ylim(-2, 102)
    ax.legend(fontsize=8, ncol=2, frameon=False)
    fig.tight_layout()
    fig.savefig(OUT / "graphe_1_pd_pfa_pm_vs_snr.png", bbox_inches="tight")
    plt.close(fig)


def graph_scatter_cpu_fpga(winner_df: pd.DataFrame):
    ok = winner_df[winner_df["status"] == "ok"].copy()
    piv = ok.pivot(index="file", columns="method", values="peak_ratio").dropna()
    if "cpu_lut_angles" not in piv.columns or "fpga" not in piv.columns:
        return

    fig, ax = plt.subplots(figsize=(6.8, 6.2))
    x = piv["cpu_lut_angles"].values
    y = piv["fpga"].values
    ax.scatter(x, y, s=28, alpha=0.75, edgecolors="black", linewidths=0.25, color="#2ca02c")
    mn, mx = min(x.min(), y.min()), max(x.max(), y.max())
    ax.plot([mn, mx], [mn, mx], "r--", linewidth=1.4, label="y = x")
    ax.set_title("Concordance CPU vs FPGA (peak ratio)")
    ax.set_xlabel("CPU LUT - peak ratio")
    ax.set_ylabel("FPGA - peak ratio")
    ax.legend(frameon=False)
    fig.tight_layout()
    fig.savefig(OUT / "graphe_2_scatter_cpu_vs_fpga.png", bbox_inches="tight")
    plt.close(fig)


def graph_timing(perf_df: pd.DataFrame):
    df = perf_df.copy()
    labels = df["method_label"] if "method_label" in df.columns else df["method"]
    vals = df["time_mean_ms"].astype(float)
    colors = ["#1f77b4", "#ff7f0e"][: len(df)]

    fig, ax = plt.subplots(figsize=(7.2, 4.6))
    bars = ax.bar(labels, vals, color=colors)
    for b, v in zip(bars, vals):
        ax.text(b.get_x() + b.get_width() / 2, b.get_height() * 1.01, f"{v:,.0f} ms".replace(",", " "), ha="center", va="bottom", fontsize=9)
    ax.set_title("Comparaison des temps moyens")
    ax.set_ylabel("Temps moyen (ms)")
    fig.tight_layout()
    fig.savefig(OUT / "graphe_3_timing_comparatif.png", bbox_inches="tight")
    plt.close(fig)


def graph_pfa_leurres_vs_snr(pfa_snr_df: pd.DataFrame):
    fig, ax = plt.subplots(figsize=(8.2, 4.6))
    colors = {"cpu_lut_angles": "#1f77b4", "fpga": "#ff7f0e"}
    for method, g in pfa_snr_df.groupby("method"):
        g = g.sort_values("snr_db")
        label = g["label"].iloc[0] if "label" in g.columns else method
        ax.plot(g["snr_db"], g["Pfa_pct"], marker="o", linewidth=2, color=colors.get(method, None), label=label)
    ax.set_title("PFA sur PRN leurres vs SNR")
    ax.set_xlabel("SNR (dB)")
    ax.set_ylabel("PFA (%)")
    ymax = max(5.0, float(pfa_snr_df["Pfa_pct"].max()) * 1.25 + 0.5)
    ax.set_ylim(0, ymax)
    ax.legend(frameon=False)
    fig.tight_layout()
    fig.savefig(OUT / "graphe_4_pfa_leurres_vs_snr.png", bbox_inches="tight")
    plt.close(fig)


def main():
    _style()
    snr_df = pd.read_csv(SRC / "merged_table_by_snr.csv")
    winner_df = pd.read_csv(SRC / "merged_winner_results.csv")
    perf_df = pd.read_csv(SRC / "merged_table_cpu_fpga.csv")

    # PFA par SNR reconstruit depuis winner (compatible merged)
    ok = winner_df[winner_df["status"] == "ok"].copy()
    pfa_snr = (
        ok.groupby(["method", "snr_db"], as_index=False)
        .agg(Pfa_pct=("pfa_flag", lambda s: 100.0 * np.mean(s)))
        .sort_values(["snr_db", "method"])
        .reset_index(drop=True)
    )
    label_map = {"cpu_lut_angles": "CPU LUT angles", "fpga": "FPGA IP"}
    pfa_snr["label"] = pfa_snr["method"].map(label_map).fillna(pfa_snr["method"])

    graph_pd_pfa_pm_vs_snr(snr_df)
    graph_scatter_cpu_fpga(winner_df)
    graph_timing(perf_df)
    graph_pfa_leurres_vs_snr(pfa_snr)

    print(f"Graphes generes dans: {OUT}")


if __name__ == "__main__":
    main()
