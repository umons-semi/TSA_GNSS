# Selection des tableaux et graphes pour le rapport

Ce dossier contient une selection courte et academique des resultats a inserer dans le rapport.

## A inclure en priorite (obligatoire)

1. **Performance globale et validite**
- `final_table_global.csv`
- `final_table_by_snr.csv`
- `final_pd_pfa_pm_vs_snr.png`

2. **Cohérence CPU vs FPGA**
- `final_concordance.csv`
- `final_concordance_by_snr.csv`
- `final_scatter_cpu_vs_fpga.png`

3. **Gain de performance temporelle**
- `final_performance.csv`
- `final_timing.png`

4. **PFA sur PRN leurres**
- `final_pfa_leurres_global.csv`
- `final_pfa_by_snr.csv`
- `final_pfa_by_prn.csv`
- `final_pfa_leurres_vs_snr.png`
- `final_peak_ratio_inj_vs_leurres.png`

## A inclure en annexe (recommande)

- `final_domaine_validite.csv`
- `final_hard_cases.csv`
- `final_distributions.png`
- `final_heatmap_cpu_lut_angles.png`
- `final_heatmap_fpga.png`

## Ordre conseille dans le rapport

1. Protocole de validation (jeu de donnees + criteres)
2. Tableau global (`final_table_global.csv`)
3. Resultats par SNR (`final_table_by_snr.csv` + `final_pd_pfa_pm_vs_snr.png`)
4. Concordance CPU/FPGA (`final_concordance.csv` + `final_scatter_cpu_vs_fpga.png`)
5. Performance (`final_performance.csv` + `final_timing.png`)
6. PFA leurres (`final_pfa_leurres_global.csv`, `final_pfa_by_snr.csv`, graphe PFA)
7. Annexes (cas difficiles, heatmaps, distributions)

## Legendes courtes prêtes a copier

- **Figure Pd/Pfa/Pm vs SNR**: Evolution des taux de detection, fausse alarme et miss en fonction du SNR pour CPU LUT et FPGA.
- **Figure concordance CPU/FPGA**: Dispersion des `peak_ratio` entre methodes; la proximite de la diagonale confirme la coherence des sorties.
- **Figure timing**: Comparaison des temps moyens et decomposition temporelle; l'IP FPGA montre un gain majeur de latence.
- **Figure PFA leurres vs SNR**: Taux de fausse alarme sur PRN leurres selon le SNR.
- **Figure peak ratio injecte vs leurres**: Separation statistique entre PRN injecte et PRN leurres.
