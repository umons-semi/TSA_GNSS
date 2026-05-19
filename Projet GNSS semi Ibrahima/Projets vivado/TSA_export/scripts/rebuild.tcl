################################################################################
# rebuild.tcl - Script de reconstruction du projet Vivado
#
# Usage dans la Tcl Console Vivado :
#   set export_root "/chemin/absolu/vers/dossier/export"
#   source "$export_root/scripts/rebuild.tcl"
#
# Ce script est AUTONOME : il ne nécessite pas le projet original.
################################################################################

if {![info exists export_root]} {
    set script_dir [file normalize [file dirname [info script]]]
    set export_root [file normalize [file join $script_dir ".."]]
}
set export_root [string map {\\ /} $export_root]
puts "\[REBUILD\] export_root = $export_root"

proc luniq_rebuild {lst} {
    set seen {}; set out {}
    foreach e $lst { if {[lsearch -exact $seen $e] == -1} { lappend seen $e; lappend out $e } }
    return $out
}
proc safe_update_ip_catalog_rb {} {
    if {[catch {update_ip_catalog -rebuild_user_ip} _e]} { catch {update_ip_catalog} }
}

# IP repos custom exportés (chemins relatifs à export_root)
set custom_ip_repos [list]
lappend custom_ip_repos [string map {\\ /} [file normalize [file join $export_root "ip_repo/ip"]]]

puts "\[REBUILD\] [1/4] Configuration des IP repositories..."
set _er [list]
catch { set _er [get_property IP_REPO_PATHS [current_project]] }
set _mr [luniq_rebuild [concat $_er $custom_ip_repos]]
if {[catch {current_project}] == 0} {
    set_property IP_REPO_PATHS $_mr [current_project]
    safe_update_ip_catalog_rb
}

puts "\[REBUILD\] [2/4] Reconstruction du projet..."
set origin_dir $export_root
source [file join $export_root "scripts" "_raw_project.tcl"]

puts "\[REBUILD\] [3/4] Reconfiguration IP repos post-création..."
set _er2 [list]
catch { set _er2 [get_property IP_REPO_PATHS [current_project]] }
set _nu 0
foreach _r $custom_ip_repos { if {[lsearch -exact $_er2 $_r] == -1} { set _nu 1; break } }
if {$_nu} {
    set_property IP_REPO_PATHS [luniq_rebuild [concat $_er2 $custom_ip_repos]] [current_project]
    safe_update_ip_catalog_rb
    puts "\[REBUILD\] IP_REPO_PATHS mis à jour."
} else {
    puts "\[REBUILD\] IP_REPO_PATHS déjà à jour."
}

puts "\[REBUILD\] [4/4] Rechargement du Block Design..."
set _bdf [get_files -filter {FILE_TYPE == "Block Designs"}]
if {[llength $_bdf] > 0} {
    open_bd_design [lindex $_bdf 0]
    safe_update_ip_catalog_rb
    set _stale [get_ips -filter {UPGRADE_VERSIONS != ""}]
    if {[llength $_stale] > 0} { catch {upgrade_ip $_stale} }
    if {[catch {validate_bd_design} _ve]} {
        puts "\[REBUILD\] AVERTISSEMENT validate : $_ve"
    } else {
        puts "\[REBUILD\] Block Design valide."
    }
    save_bd_design
}

puts "\[REBUILD\] ============================================="
puts "\[REBUILD\] Reconstruction terminée avec succès."
catch { puts "\[REBUILD\] Projet : [get_property NAME [current_project]]" }
catch { puts "\[REBUILD\] Dossier: [get_property DIRECTORY [current_project]]" }
puts "\[REBUILD\] ============================================="
