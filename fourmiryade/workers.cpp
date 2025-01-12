#include "sinfourmis.h"
#include "structures.cpp"
#include "memory.cpp"
#include "structures.cpp"

#include <cassert>
#include <stdlib.h>
using namespace std;

#define PATH_NODE_SIZE 8
#define PATH_POSITION_SIZE 8
#define octet(x) (8*(x))
/**
 * le fighter doit récupérer un chemin correspondant à une salle avec de l'eau vérifier qu'il n'y a pas de fighter dessus
 * et s'y diriger : c'est la reine qui lui donne le chemin !
 * cette fonction n'est donc utilisée que par la reine
 */




typedef enum {
    WORKER_MOVED,
    WORKER_BUILT_1,
    WORKER_BUILT_2,
    WORKER_BUILT_3,
    WORKER_ENDED_ACTION,
    NUM_WORKER_STATES
} worker_state;
#define WORKER_STATE_SIZE ceil_log2(NUM_WORKER_STATES)


// memory of a worker : state, forward, position, path_length, mem
#define HEADER_END octet(5)
#define acces_direction(pos) HEADER_END + octet(pos)

void initialize_worker(fourmi_etat *etat, int* path, int path_length){
  set_number(etat->memoire, octet(0), octet(1), WORKER);
  set_number(etat->memoire, octet(1), octet(2)-1, WORKER_ENDED_ACTION);
  set_bit(etat->memoire, octet(2)-1, 1);
  set_number(etat->memoire, octet(3), octet(4), 0);
  set_number(etat->memoire, octet(4), octet(5), path_length);
  for (int i=0; i<path_length; i++) {
    set_number(etat->memoire, acces_direction(i), acces_direction(i+1), path[i]);
  }
}



fourmi_retour worker_activation(fourmi_etat *etat, const salle *salle) {
    // State.
    
    worker_state state = (worker_state) get_number(etat->memoire, octet(1), octet(2)-1);

    bool forward = (bool) get_number(etat->memoire, octet(2)-1, octet(2));
    int position = (int) get_number(etat->memoire, octet(3), octet(4));
    int path_length = (int) get_number(etat->memoire, octet(4), octet(5));
    switch (state) {
      case WORKER_MOVED: {
        if (position == path_length && forward) {
          assert(salle->type == NOURRITURE);
          set_number(etat->memoire, octet(1), octet(2)-1, WORKER_ENDED_ACTION);
          set_bit(etat->memoire, octet(2)-1, 0);
          return {
            .action = RAMASSE_NOURRITURE,
            .arg = 0,
            .depose_pheromone = NO_PHEROMONE,
            .pheromone = 0
          };
        }
        else if (position == 0 && !forward) {
          return {
            .action=FOURMI_PASSE,
            .arg=0,
            .depose_pheromone=NO_PHEROMONE,
            .pheromone=0
          };
        }
        else if (etat->result == -2) {
          int direction = (int)get_number(etat->memoire, HEADER_END + octet(position), HEADER_END + octet(position)+1);
          set_number(etat->memoire, octet(1), octet(2)-1, WORKER_BUILT_1);
          return {
            .action=COMMENCE_CONSTRUCTION,
            .arg=direction,
            .depose_pheromone=NO_PHEROMONE,
            .pheromone=0
          };
        } else {
          assert(etat->result != -1);
          if (forward) {
            position +=1;
          }
          else {
            position -=1;
          }
          set_number(etat->memoire, octet(3), octet(4), position);
          int direction = (int)get_number(etat->memoire, acces_direction(position), acces_direction(position+1));
          set_number(etat->memoire, acces_direction(position), acces_direction(position+1), etat->result);
          set_number(etat->memoire, octet(1), octet(2)-1, WORKER_MOVED);
          return {
            .action=DEPLACEMENT,
            .arg=direction, 
            .depose_pheromone=NO_PHEROMONE, 
            .pheromone=0
          };
        }
      }
      case  WORKER_BUILT_1: {
        set_number(etat->memoire, octet(1), octet(2)-1, WORKER_BUILT_2);
        return {
          .action=FOURMI_PASSE,
          .arg=0,
          .depose_pheromone = NO_PHEROMONE,
          .pheromone = 0
        };
      }
      case WORKER_BUILT_2: {
        set_number(etat->memoire, octet(1), octet(2)-1, WORKER_BUILT_2);
        return {
          .action=FOURMI_PASSE,
          .arg=0,
          .depose_pheromone = NO_PHEROMONE,
          .pheromone = 0
        };
      }
      case WORKER_BUILT_3: {
        set_number(etat->memoire, octet(1), octet(2)-1, WORKER_ENDED_ACTION);
        return {
          .action=TERMINE_CONSTRUCTION,
          .arg=0,
          .depose_pheromone=NO_PHEROMONE,
          .pheromone=0
        };
      }
      case WORKER_ENDED_ACTION: {
        int direction = (int)get_number(etat->memoire, acces_direction(position), acces_direction(position+1));
        set_number(etat->memoire, octet(1), octet(2)-1, WORKER_MOVED);
        return {
          .action=DEPLACEMENT,
          .arg=direction, 
          .depose_pheromone=NO_PHEROMONE, 
          .pheromone=0
        };
      }
      default:
        assert(false);
    }
}
