#ifndef  FILE_EXPLORER
#define FILE_EXPLORER

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
    EXPLORER_MOVED,
    EXPLORER_BUILT_1,
    EXPLORER_BUILT_2,
    EXPLORER_BUILT_3,
    EXPLORER_ENDED_ACTION,
    NUM_EXPLORER_STATES
} explorer_state;
#define EXPLORER_STATE_SIZE ceil_log2(NUM_EXPLORER_STATES)


// memory of a explorer : state, forward, position, path_length, mem
#define EXPLORER_HEADER_END octet(7)
#define exp_acces_direction(pos) EXPLORER_HEADER_END + octet(pos)

void initialize_explorer(fourmi_etat *etat, int max_water){
  set_number(etat->memoire, octet(0), octet(1), EXPLORER); // type de fourmi
  set_number(etat->memoire, octet(1), octet(2)-1, EXPLORER_ENDED_ACTION); // state
  set_number(etat->memoire, octet(2)-1, octet(2), 1); //forward
  set_number(etat->memoire, octet(3), octet(4), 0); //position
  set_number(etat->memoire, octet(4), octet(5), 0); //result
  set_number(etat->memoire, octet(5), octet(6), max_water); // max_water
  set_number(etat->memoire, octet(6), octet(7), 0); // path_len
}

void handle_explorer_from_queen (fourmi_etat* etat, vector<int*> food_paths, vector<int> food_paths_len) {
  if (get_number(etat->memoire, octet(4), octet(5))){ //result
    int path_len = get_number(etat->memoire, octet(6), octet(7)); //path_len
    int* path = (int*) malloc(path_len*sizeof(int));
    food_paths.push_back(path);
    food_paths_len.push_back(path_len+1);
  }
}



fourmi_retour explorer_activation(fourmi_etat *etat, const salle *salle) {

    explorer_state state = (explorer_state) get_number(etat->memoire, octet(1), octet(2)-1);

    bool forward = (bool) get_number(etat->memoire, octet(2)-1, octet(2));
    int position = (int) get_number(etat->memoire, octet(3), octet(4));
    int max_water = (int) get_number(etat->memoire, octet(5), octet(6));
    switch (state) {
      case EXPLORER_MOVED: {
        if (etat->eau <= max_water/2+5) {
          forward = 0;
        }

        if (forward == 0 && position == 0) {
          return {
            .action=FOURMI_PASSE,
            .arg=0,
            .depose_pheromone=NO_PHEROMONE,
            .pheromone=0
          };
        }
        else if (salle->type == NOURRITURE && salle->pheromone == 0) {
          position +=1;
          set_number(etat->memoire, octet(3), octet(4), position); // position
          set_number(etat->memoire, octet(1), octet(2)-1, EXPLORER_ENDED_ACTION); //state
          set_number(etat->memoire, octet(2)-1, octet(2), 0); //forward
          set_number(etat->memoire, octet(4), octet(5), 1); // result = success
          set_number(etat->memoire, octet(6), octet(7), position); //path_len
          return {
            .action = RAMASSE_NOURRITURE,
            .arg = 0,
            .depose_pheromone = PRIVE,
            .pheromone = EXPLORER_FOOD
          };
        } else if (etat->result == -2) {
          int direction = (int)get_number(etat->memoire, EXPLORER_HEADER_END + octet(position), EXPLORER_HEADER_END + octet(position)+1);
          set_number(etat->memoire, octet(1), octet(2)-1, EXPLORER_BUILT_1);
          return {
            .action=COMMENCE_CONSTRUCTION,
            .arg=direction,
            .depose_pheromone=NO_PHEROMONE,
            .pheromone=0
          };
        } else {
          assert(etat->result != -1);
          int direction;
          if (forward) {
            position +=1;
            direction = rand()%salle->degre;
          }
          else {
            position -=1;
            direction = (int)get_number(etat->memoire, exp_acces_direction(position), exp_acces_direction(position+1));
          }
          set_number(etat->memoire, octet(3), octet(4), position);
          set_number(etat->memoire, exp_acces_direction(position), exp_acces_direction(position+1), etat->result);
          set_number(etat->memoire, octet(1), octet(2)-1, EXPLORER_MOVED);
          return {
            .action=DEPLACEMENT,
            .arg=direction,
            .depose_pheromone=NO_PHEROMONE,
            .pheromone=0
          };
        }
      }
      case  EXPLORER_BUILT_1: {
        set_number(etat->memoire, octet(1), octet(2)-1, EXPLORER_BUILT_2);
        return {
          .action=FOURMI_PASSE,
          .arg=0,
          .depose_pheromone = NO_PHEROMONE,
          .pheromone = 0
        };
      }
      case EXPLORER_BUILT_2: {
        set_number(etat->memoire, octet(1), octet(2)-1, EXPLORER_BUILT_2);
        return {
          .action=FOURMI_PASSE,
          .arg=0,
          .depose_pheromone = NO_PHEROMONE,
          .pheromone = 0
        };
      }
      case EXPLORER_BUILT_3: {
        set_number(etat->memoire, octet(1), octet(2)-1, EXPLORER_ENDED_ACTION);
        return {
          .action=TERMINE_CONSTRUCTION,
          .arg=0,
          .depose_pheromone=NO_PHEROMONE,
          .pheromone=0
        };
      }
      case EXPLORER_ENDED_ACTION: {
        int direction = (int)get_number(etat->memoire, exp_acces_direction(position), exp_acces_direction(position+1));
        set_number(etat->memoire, octet(1), octet(2)-1, EXPLORER_MOVED);
        return {
          .action=DEPLACEMENT,
          .arg=direction,
          .depose_pheromone=NO_PHEROMONE,
          .pheromone=0
        };
      }
      default: {
        fprintf(stderr, "--%d\n", state);
        for (int i=0; i<30; i++) {
            if (i%4 == 0) {
                printf(" ");
            }
            printf("%d", get_bit(etat->memoire, i));
        }
        printf("\n");
        assert(false);
      }
    }
}

#endif
