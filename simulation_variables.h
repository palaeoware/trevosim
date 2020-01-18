#ifndef SIMULATION_VARIABLES_H
#define SIMULATION_VARIABLES_H

#include <QString>
#include <QTextStream>

//A bunch of global variables.
extern int genome_size, taxon_number, pfield_size, species_difference, unresolvable_cutoff, mask_number, informative_characters;
extern bool batch_running, pause_flag, strip_uninformative, append, write_tree, sansomian, error_batch, unresolvable_batch, no_deleterious, calc_factor_running, factor_settings_batch, escape_pressed, work_log;
extern double strip_uninformative_factor, environment_mutation, organism_mutation;
extern QString base_01, base_02, base_03, ext_01, ext_02, ext_03, filestring_01, filestring_02, filestring_03, groups, factor_settings, settings_filename;

class simulation_variables
{
public:
    simulation_variables();
};

#endif // SIMULATION_VARIABLES_H
