#include "sinfourmis.h"

reine_retour reine_activation(fourmi_etat fourmis[], const unsigned int nb_fourmis,
                                  const reine_etat *etat, const salle *salle) {
    return {.action = REINE_PASSE, .arg = 0};
}

fourmi_retour fourmi_activation(fourmi_etat *etat, const salle *salle) {
    return {.action = FOURMI_PASSE, .arg = 0, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
}