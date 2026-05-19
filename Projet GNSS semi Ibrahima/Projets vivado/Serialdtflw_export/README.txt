========================================================
 Export Vivado - Projet : Serialdtflw
 Date    : 2026-04-30 13:04:47
 Part    : xc7z020clg400-1
========================================================

STRUCTURE
---------
  src/             -> Sources HDL standalone (Verilog / VHDL)
  constraints/     -> Contraintes utilisateur (XDC hors IP generees)
  ip_repo/         -> IP custom et HLS exportees
  scripts/
    _raw_project.tcl  <- généré par write_project_tcl
    rebuild.tcl       <- script autonome principal

RECONSTRUCTION
--------------
1. Ouvrez Vivado (sans projet)
2. Dans la Tcl Console :
     set export_root {/chemin/absolu/vers/ce/dossier}
     source "$export_root/scripts/rebuild.tcl"

EMPLACEMENTS
------------
  Local (original) : C:/Users/LABO01/PYNQ/Ibrahima_1/Vivado/SerialAcq/Serialdtflw/export
  Cible  (copie)   : C:/Users/LABO01/Desktop/Algorithmes d'acquisition GNSS/Projets vivado/Serialdtflw_export

IP REPOSITORIES CUSTOM
----------------------
  ip_repo/ip
    <- C:/Users/LABO01/PYNQ/Ibrahima_1/Vitis/acqDMA/acq_serial_dtflow/acquisition_serial/hls/impl/ip
