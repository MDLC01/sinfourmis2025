#include "sinfourmis.h"
#include <iostream>

reine_retour cpp_reine_activation(fourmi_etat fourmis[], const unsigned int nb_fourmis,
                                  const reine_etat *etat, const salle *salle) {

    std::cout << "Reine" << std::endl;
    if (etat->nourriture < 10) {
        return {.action = REINE_PASSE, .arg = 0};
    }
    if (nb_fourmis > 0) {
        for (uint i = 0; i < nb_fourmis; i++) {
            for (uint j = 0; j < 256; j++) {
                fourmis[i].memoire[j] = 0;
            }
        }
        std::cout << "Initilisation mémoire" << std::endl;
        int e = etat->max_envoi;
        return {.action = ENVOYER_FOURMI, .arg = e};
    }

    static int parite_action = 0;

    if (parite_action == 0) {
        parite_action = 1;
        int e = etat->max_envoi;
        return {.action = CREER_FOURMI, .arg = e};
    }

    if (parite_action == 1) {
        parite_action = 2;
        if (etat->duree_amelioration > 1) {
            return {.action = AMELIORE_VITESSE_AMELIORATION, .arg = 1};
        }
    }

    if (parite_action == 2) {
        parite_action = 3;
        if (etat->max_stockage < etat->max_production) {
            return {.action = AMELIORE_STOCKAGE, .arg = 5};
        }
        if (etat->max_production < etat->max_envoi) {
            return {.action = AMELIORE_PRODUCTION, .arg = 5};
        }
        return {.action = AMELIORE_ENVOI, .arg = 5};
    }

    if (parite_action == 3) {
        parite_action = 4;
        return {.action = AMELIORE_RAMASSAGE, .arg = 1};
    }

    if (parite_action == 4) {
        parite_action = 0;
        if ((etat->nourriture / 10) % 2 == 0) {
            return {.action = AMELIORE_EAU, .arg = 1};
        }
        return {.action = AMELIORE_VIE, .arg = 1};
    }

    return {.action = REINE_PASSE, .arg = 0};
}

fourmi_retour cpp_fourmi_activation(fourmi_etat *etat, const salle *salle) {
    std::cout << "Fourmi" << std::endl;
    uint coup_precedent = 254;
    // indice de la case dans la memoire qui stocke le coup precedent
    // 1 pour un deplacement
    // 2 pour un ramasse_nouriture
    // 3 pour un Commence_construction

    uint distance_reine = 255;
    // indice de la case dans la memoire qui stocke la distance à la reine

    std::cout << "distance_reine = " << (int)etat->memoire[distance_reine] << std::endl;

    if (salle->type == NOURRITURE && etat->memoire[coup_precedent] == 1) {
        if (etat->result > 0) {
            etat->memoire[(int)etat->memoire[distance_reine]] = etat->result;
            etat->memoire[distance_reine]++;
        }

        etat->memoire[coup_precedent] = 2;
        return {.action = RAMASSE_NOURRITURE,
                .arg = 1,
                .depose_pheromone = NO_PHEROMONE,
                .pheromone = 0};
    }

    if (etat->nourriture > 0) {
        std::cout << "nourriture = " << etat->nourriture << std::endl;
        std::cout << "prec_mouv = " << (int)etat->memoire[coup_precedent] << std::endl;
        if (etat->memoire[coup_precedent] == 1) {
            std::cout << "result = " << etat->result << std::endl;
            if (etat->result < 0) {
                int chemin = etat->memoire[(int)etat->memoire[distance_reine]];
                return {.action = COMMENCE_CONSTRUCTION,
                        .arg = chemin,
                        .depose_pheromone = NO_PHEROMONE,
                        .pheromone = 0};
            }
            int chemin = etat->memoire[(int)etat->memoire[--distance_reine]];
            return {.action = DEPLACEMENT,
                    .arg = chemin,
                    .depose_pheromone = NO_PHEROMONE,
                    .pheromone = 0};
        }
        int chemin = etat->memoire[(int)etat->memoire[--distance_reine]];
        std::cout << "chemin = " << chemin << std::endl;
        return {
            .action = DEPLACEMENT, .arg = chemin, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
    }

    // phase d'exploration
    if (etat->memoire[coup_precedent] == 1) {
        if (etat->result > 0) {
            etat->memoire[(int)etat->memoire[distance_reine]] = etat->result;
            etat->memoire[distance_reine]++;
        }
    }

    int chemin = rand() % salle->degre;

    if (etat->memoire[distance_reine] == 0) {
        if (etat->memoire[coup_precedent] == 1 && etat->result > 0) {
            return {.action = DEPLACEMENT,
                    .arg = etat->result,
                    .depose_pheromone = NO_PHEROMONE,
                    .pheromone = 0};
        }
        etat->memoire[coup_precedent] = 1;
        return {
            .action = DEPLACEMENT, .arg = chemin, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
    }

    etat->memoire[coup_precedent] = 1;
    return {.action = DEPLACEMENT, .arg = chemin, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
}

extern "C" {
reine_retour reine_activation(fourmi_etat fourmis[], const unsigned int nb_fourmis,
                              const reine_etat *etat, const salle *salle) {
    return cpp_reine_activation(fourmis, nb_fourmis, etat, salle);
}

fourmi_retour fourmi_activation(fourmi_etat *etat, const salle *salle) {
    return cpp_fourmi_activation(etat, salle);
}
}