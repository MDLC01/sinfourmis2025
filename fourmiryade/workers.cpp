#include "sinfourmis.h"
#include "cartographer.cpp"

typedef enum {
    ANT_START,
    ANT_TRIED_MOVING,
    ANT_DIGGED_1,
    ANT_DIGGED_2,
    ANT_DIGGED_3,
    NUM_ANT_STATE,
} ant_state;

/**
 * le fighter doit récupérer un chemin correspondant à une salle avec de l'eau vérifier qu'il n'y a pas de fighter dessus
 * et s'y diriger : c'est la reine qui lui donne le chemin !
 * cette fonction n'est donc utilisée que par la reine
 */
fourmi_etat *program_worker(fourmi_etat *etat){
    for(int i=0; i<MAX_NODES;i++){
        if(node_infos[i].food || node_infos[i].water){
            path path = shortest_path(BASE_NODE, i);
            uint8_t length = path.size();
            
            etat->memoire[0] = length;
            
            for(int j=1;j<length;j++) etat->memoire[j] = path[i];
            return etat;
        }
    }

    return etat;
}

/**
 * Dans un fighter la mémoire est organisée ainsi : 
 */

fourmi_retour move_or_build(fourmi_etat *etat){
    

    switch (state) {
        ANT_START: {
            choice c = rand() % salle->degre;
            write_number(rw, PATH_NODE_INFO_NEXT_SIZE, c);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) ANT_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        ANT_TRIED_MOVING: {
            rw->offset += path_size;
            if (etat->result < 0) {
                // Unable to move there.
                rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
                choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
                write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) ANT_DIGGED_1);
                return {.action = ATTAQUE_TUNNEL, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
            }
            write_bool(rw, salle->nourriture > 0);
            write_bool(rw, salle->type == EAU);
            write_number(rw, PATH_NODE_INFO_PREV_SIZE, etat->result);
            write_number(rw, PATH_NODE_INFO_COST_SIZE, previous_water - etat->eau);
            choice c = rand() % salle->degre;
            write_number(rw, PATH_NODE_INFO_NEXT_SIZE, c);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) ANT_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        ANT_DIGGED_1: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) ANT_DIGGED_2);
            return {.action = ATTAQUE_TUNNEL, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }
        ANT_DIGGED_2: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) ANT_DIGGED_3);
            return {.action = ATTAQUE_TUNNEL, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }
        ANT_DIGGED_3: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) ANT_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        default: {
            assert(false);

        }
    }
}