#include <cstdint>
#include <vector>
#include "sinfourmis.h"
#include "utils.cpp"
#include "memory.cpp"
#include "cartographer.cpp"
#include "workers.cpp"
#include "explorer.cpp"

using namespace std;

enum queen_turn_t {RECUP_TURN, RELEASE_TURN, BIRTH_TURN};
queen_turn_t turn_type = BIRTH_TURN;

int nb_cartographer = 0;
int nb_explorer = 0;

vector<int*> food_paths = {}; 
vector<int> food_paths_len = {};

uint32_t ancien_compte_nourriture = 0;
uint32_t nourriture_a_depenser = 0;


reine_retour queen_main(fourmi_etat fourmis[], const unsigned int nb_fourmis, const reine_etat *etat, const salle *salle) {
  if (turn_count == 0) {
    nb_explorer = (etat->nourriture/4);
  }
  if (turn_count == 100) {
    nb_cartographer = 4;
  }
  turn_count +=1;
  nourriture_a_depenser += (etat->nourriture - ancien_compte_nourriture) / 2;
  ancien_compte_nourriture = etat->nourriture;
  for (unsigned int i = 0; i<nb_fourmis; i++) {
    rw mem = create_rw(fourmis[i].memoire);
    type_fourmi fourmi = read_type(&mem);
    switch (fourmi) {
      case EXPLORER:{
        handle_explorer_from_queen(&(fourmis[i]), food_paths, food_paths_len);
        nb_explorer ++;
        break;
      }
      case CARTOGRAPHER : {
        handle_cartographer_from_queen(&(fourmis[i]));
        nb_cartographer++;
        break;
      }
      case WORKER:{
        handle_worker_from_queen(&(fourmis[i]));
        break;
      }
      case VARIANT_COUNT: {
        assert(false);
      }
    }
    for  (int j=0; j<256; j++){
      fourmis[i].memoire[j] = 0;
    }
  }
  switch (turn_type) {
    case RECUP_TURN:{
      turn_type = BIRTH_TURN;
      return {.action = RECUPERER_FOURMI, .arg=0};
    }
    case BIRTH_TURN : {
      turn_type = RELEASE_TURN;
      int diff = etat->max_stockage - nb_fourmis;
      if (2*diff < (int)nourriture_a_depenser) {
        nourriture_a_depenser = nourriture_a_depenser - 2*diff;
        return {.action = CREER_FOURMI, .arg = (int32_t)etat->max_stockage};
        }
      else {
        nourriture_a_depenser = 0;
        return {.action = CREER_FOURMI, .arg = (int32_t)nourriture_a_depenser/2};
      }
    }
    case RELEASE_TURN: {
      for (unsigned int i=0; i<nb_fourmis; i++) {
        if (nb_cartographer != 0) {
          nb_cartographer --;
          initialize_cartographer(&(fourmis[i]), rand()%4);
        }
        else if (nb_explorer != 0) {
          nb_explorer --;
          initialize_explorer(&(fourmis[i]), etat->max_eau);
        }
        else {
          initialize_worker(&(fourmis[i]), food_paths, food_paths_len);
        }
      }
      return {.action = ENVOYER_FOURMI, .arg=0};
      turn_type = RECUP_TURN;
    }
  }
}
