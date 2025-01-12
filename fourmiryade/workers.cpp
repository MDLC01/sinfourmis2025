#ifndef FILE_WORKERS 
#define FILE_WORKERS

#include "sinfourmis.h"
#include "memory.cpp"
#include "utils.cpp"
#include <vector>

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
#define WORKER_HEADER_END octet(5)
#define work_acces_direction(pos) WORKER_HEADER_END + octet(pos)

void initialize_worker(fourmi_etat *etat, vector<int*>food_paths, vector<int>food_paths_len){
  set_number(etat->memoire, octet(0), octet(1), WORKER);
  set_number(etat->memoire, octet(1), octet(2)-1, WORKER_ENDED_ACTION);
  set_number(etat->memoire, octet(2)-1, octet(2), 1);
  set_number(etat->memoire, octet(3), octet(4), 0);
  int id_path = rand()%food_paths.size();
  set_number(etat->memoire, octet(4), octet(5), food_paths_len[id_path]);
  for (int i=0; i<food_paths_len[id_path]; i++) {
    set_number(etat->memoire, work_acces_direction(i), work_acces_direction(i+1), food_paths[id_path][i]-1);
  }
}

void handle_worker_from_queen (fourmi_etat* etat) {
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
          set_number(etat->memoire, octet(2)-1, octet(2), 0);
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
          int direction = (int)get_number(etat->memoire, WORKER_HEADER_END + octet(position), WORKER_HEADER_END + octet(position)+1);
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
          int direction = (int)get_number(etat->memoire, work_acces_direction(position), work_acces_direction(position+1));
          set_number(etat->memoire, work_acces_direction(position), work_acces_direction(position+1), etat->result);
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
        int direction = (int)get_number(etat->memoire, work_acces_direction(position), work_acces_direction(position+1));
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
#endif