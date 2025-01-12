#include "memory.cpp"
#include "utils.cpp"
#include "sinfourmis.h"

#define PHEROMONE_EXPLORED 0b00010000

typedef enum {
    EXPLORER_FOLLOWS_INITIAL_PATH,
    EXPLORER_EXPLORES,
    EXPLORER_BACKS_UP,
    NUM_EXPLORER_PHASES,
} explorer_phase;
#define EXPLORER_PHASE_SIZE ceil_log2(NUM_EXPLORER_PHASES)

typedef enum {
    SUE_TRIED_MOVING,
    SUE_DIGGED_1,
    SUE_DIGGED_2,
    SUE_DIGGED_3,
    SUE_DIGGED_4,
    NUM_SUE_STATES,
} setting_up_explorer_state;
#define SUE_STATE_SIZE ceil_log2(NUM_SUE_STATES)

#define SUE_PATH_OFFSET_SIZE 10
#define SUE_LAST_DEGREE_SIZE 4

typedef enum {
    EXPLORER_STARTS,
    EXPLORER_TRIED_MOVING,
    EXPLORER_DIGGED_1,
    EXPLORER_DIGGED_2,
    EXPLORER_DIGGED_3,
    NUM_EXPLORER_STATES,
} explorer_state;
#define EXPLORER_STATE_SIZE ceil_log2(NUM_EXPLORER_STATES)

fourmi_retour explorer_activation(fourmi_etat *etat, rw *rw, const salle *salle) {
    auto rw_at_phase = clone_rw(rw);
    explorer_phase phase = (explorer_phase) read_number(rw, EXPLORER_PHASE_SIZE);
    switch (phase) {
        case EXPLORER_FOLLOWS_INITIAL_PATH: {
            auto rw_at_state = clone_rw(rw);
            setting_up_explorer_state state = (setting_up_explorer_state) read_number(rw, SUE_STATE_SIZE);
            switch (state) {
                case SUE_TRIED_MOVING: {
                    // TODO: Change phase when the path ends.
                    auto rw_at_last_degree = clone_rw(rw);
                    int last_degree = read_number(rw, SUE_LAST_DEGREE_SIZE);
                    int path_offset = read_number(rw, SUE_PATH_OFFSET_SIZE);
                    rw->offset += path_offset;
                    if (etat->result < 0) {
                        // Unable to move.
                        int choice_size = ceil_log2(salle->degre);
                        choice c = read_number(rw, choice_size);
                        write_number(&rw_at_state, SUE_STATE_SIZE, (unsigned long long) SUE_DIGGED_1);
                        return {.action = ATTAQUE_TUNNEL, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
                    }
                    rw->offset += ceil_log2(last_degree);
                    write_number(&rw_at_last_degree, SUE_LAST_DEGREE_SIZE, salle->degre);
                    write_number(&rw_at_last_degree, SUE_PATH_OFFSET_SIZE, rw->offset);
                    int choice_size = ceil_log2(salle->degre);
                    choice c = read_number(rw, choice_size);
                    return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
                }
                case SUE_DIGGED_1: {
                    auto rw_at_last_degree = clone_rw(rw);
                    int last_degree = read_number(rw, SUE_LAST_DEGREE_SIZE);
                    int path_offset = read_number(rw, SUE_PATH_OFFSET_SIZE);
                    rw->offset += path_offset;
                    int choice_size = ceil_log2(salle->degre);
                    choice c = read_number(rw, choice_size);
                    write_number(&rw_at_state, SUE_STATE_SIZE, (unsigned long long) SUE_DIGGED_2);
                    return {.action = FOURMI_PASSE, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
                }
                case SUE_DIGGED_2: {
                    auto rw_at_last_degree = clone_rw(rw);
                    int last_degree = read_number(rw, SUE_LAST_DEGREE_SIZE);
                    int path_offset = read_number(rw, SUE_PATH_OFFSET_SIZE);
                    rw->offset += path_offset;
                    int choice_size = ceil_log2(salle->degre);
                    choice c = read_number(rw, choice_size);
                    write_number(&rw_at_state, SUE_STATE_SIZE, (unsigned long long) SUE_DIGGED_3);
                    return {.action = FOURMI_PASSE, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
                }
                case SUE_DIGGED_3: {
                    auto rw_at_last_degree = clone_rw(rw);
                    int last_degree = read_number(rw, SUE_LAST_DEGREE_SIZE);
                    int path_offset = read_number(rw, SUE_PATH_OFFSET_SIZE);
                    rw->offset += path_offset;
                    int choice_size = ceil_log2(salle->degre);
                    choice c = read_number(rw, choice_size);
                    write_number(&rw_at_state, SUE_STATE_SIZE, (unsigned long long) SUE_DIGGED_4);
                    return {.action = TERMINE_CONSTRUCTION, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
                }
                case SUE_DIGGED_4: {
                    auto rw_at_last_degree = clone_rw(rw);
                    int last_degree = read_number(rw, SUE_LAST_DEGREE_SIZE);
                    int path_offset = read_number(rw, SUE_PATH_OFFSET_SIZE);
                    rw->offset += path_offset;
                    int choice_size = ceil_log2(salle->degre);
                    choice c = read_number(rw, choice_size);
                    write_number(&rw_at_state, SUE_STATE_SIZE, (unsigned long long) SUE_TRIED_MOVING);
                    return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
                }
                case NUM_SUE_STATES: {
                    assert(false);
                }
            }
            break;
        }


//   si trajet vers source de départ :
//     suivre le chemin
//   si trajet aller :
//     si case de nourriture ou d'eau non-explorée :
//       la marquer comme explorée
//       collecter de la nourriture
//       passer au trajet retour
//     sinon si limite d'eau ou de mémoire :
//       passer au trajet retour
//     sinon :
//       si précédente action était un déplacement réussi :
//         noter le résultat du déplacement précédent (dans trajet retour) + modifier la position dans le chemin
//         choisir une direction aléatoire
//         la noter dans le chemin aller
//       déplacement-construction (edge)
//   si trajet retour :
//     suivre le chemin retour

        case EXPLORER_EXPLORES: {
            // TODO: Continuer par ici...
            if ((salle->nourriture > 0 || salle->type == EAU) && (salle->pheromone & PHEROMONE_EXPLORED) != 0) {
                write_number(&rw_at_phase, EXPLORER_PHASE_SIZE, EXPLORER_BACKS_UP);
                return {.action = RAMASSE_NOURRITURE, .arg = 0, .depose_pheromone = PUBLIC, .pheromone = PHEROMONE_EXPLORED};
            }
        }

        case EXPLORER_BACKS_UP: {
            return;
        }

        case NUM_EXPLORER_PHASES: {
            assert(false);
        }
    }
    return {};
}
