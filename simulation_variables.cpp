#include "simulation_variables.h"

//Global(ish) variables, initialised in mainwindow constructor, declared here to allow global scope
int genome_size;
int taxon_number;
int pfield_size;
int species_difference;
int mask_number;
int unresolvable_cutoff;
int informative_characters;
double strip_uninformative_factor;
double environment_mutation;
double organism_mutation;
bool escape_pressed;
bool batch_running;
bool error_batch;
bool unresolvable_batch;
bool pause_flag;
bool strip_uninformative;
bool append;
bool write_tree;
bool sansomian;
bool no_deleterious;
bool species_curve;
bool calc_factor_running;
bool factor_settings_batch;
bool work_log;
QString base_01, base_02, base_03, ext_01, ext_02, ext_03, filestring_01, filestring_02, filestring_03, groups, factor_settings, settings_filename;
QTextStream sanity_out;



