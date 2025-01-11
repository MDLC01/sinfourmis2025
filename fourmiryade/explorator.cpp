#include "sinfourmis.h"

fourmi_retour explorator_activation(fourmi_etat *etat, const salle *salle) {
    // I
    // On suppose la mémoire initialisée

    // decrémenter limite eau / mémoire ?

    if get_memoire() {
        // Trajet vers src départ
        // Suivre chemin ?
        return {.action = DEPLACEMENT, .arg = XXX, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
    }

    if (get_memoire(trajet) == 0) {
        if (!EXPLOREE && (salle->salle_type == EAU || (salle->salle_type == NOURRITURE && salle->nourriture > 0))) {
            pheromone();
            set_memoire(trajet, 1);
            return {.action = DEPLACEMENT, .arg = XXX, .depose_pheromone = PRIVE, .pheromone = 1};
        }

        if (LIMITE) {
            set_memoire(trajet, 1);
            return {.action = FOURMI_PASSE, .arg = 0, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        if () {

        }

        // déplacement-construction (edge)
        return {.action = FOURMI_PASSE, .arg = 0, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};

    }

    else {
        // Trajet vers src départ
        // Suivre chemin retour ?
    }
}
