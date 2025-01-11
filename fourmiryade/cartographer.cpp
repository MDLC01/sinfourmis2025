#include "sinfourmis.h"
#include "structures.cpp"
#include "memory.cpp"
#include <stdlib.h>

#define PATH_NODE_INFO_PREV_SIZE 4
#define PATH_NODE_INFO_COST_SIZE 4
#define PATH_NODE_INFO_NEXT_SIZE 4
#define PATH_NODE_INFO_SIZE (1 + 1 + PATH_NODE_INFO_PREV_SIZE + PATH_NODE_INFO_COST_SIZE + PATH_NODE_INFO_NEXT_SIZE)

typedef enum {
    CARTOGRAPHER_STARTS,
    CARTOGRAPHER_TRIED_MOVING,
    CARTOGRAPHER_DIGGED_1,
    CARTOGRAPHER_DIGGED_2,
    CARTOGRAPHER_DIGGED_3,
    NUM_CARTOGRAPHER_STATES,
} cartographer_state;

#define CARTOGRAPHER_STATE_SIZE ceil_log2(NUM_CARTOGRAPHER_STATES)

fourmi_retour cartographer_activation(fourmi_etat *etat, rw *rw, const salle *salle) {
    // First info is water amount.
    int previous_water = override_number(rw, 4, etat->eau);

    // State.
    auto rw_at_state = clone_rw(rw);
    cartographer_state state = (cartographer_state) read_number(rw, CARTOGRAPHER_STATE_SIZE);

    // Max encodable path length: 256.
    int path_length = read_number(rw, 8);
    // Initial choice + rest of the path.
    int path_size = PATH_NODE_INFO_NEXT_SIZE + path_length * PATH_NODE_INFO_SIZE;

    switch (state) {
        CARTOGRAPHER_STARTS: {
            choice c = rand() % salle->degre;
            write_number(rw, PATH_NODE_INFO_NEXT_SIZE, c);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        CARTOGRAPHER_TRIED_MOVING: {
            rw->offset += path_size;
            if (etat->result < 0) {
                // Unable to move there.
                rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
                choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
                write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_1);
                return {.action = ATTAQUE_TUNNEL, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
            }
            write_bool(rw, salle->nourriture > 0);
            write_bool(rw, salle->type == EAU);
            write_number(rw, PATH_NODE_INFO_PREV_SIZE, etat->result);
            write_number(rw, PATH_NODE_INFO_COST_SIZE, previous_water - etat->eau);
            choice c = rand() % salle->degre;
            write_number(rw, PATH_NODE_INFO_NEXT_SIZE, c);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        CARTOGRAPHER_DIGGED_1: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_2);
            return {.action = ATTAQUE_TUNNEL, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }
        CARTOGRAPHER_DIGGED_2: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_3);
            return {.action = ATTAQUE_TUNNEL, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }
        CARTOGRAPHER_DIGGED_3: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        default: {
            assert(false);
        }
    }
}
