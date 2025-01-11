#include <cstdint>
#include <map>
#include "sinfourmis.h"
#include "structures.cpp"
#include "memory.cpp"



using namespace std;

#define min(a,b) (a)<(b)?(a):(b)

enum queen_turn_t {RECUP_TURN, RELEASE_TURN, BIRTH_TURN};
queen_turn_t turn_type = BIRTH_TURN;
long turn_count = 0;

int nb_cartographer = 0;
int nb_explorer = 0;

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
        // TODO : read explorer memory
        nb_explorer ++;
        break;
      }
      case CARTOGRAPHER : {
        // TODO : read cartographer memory
        nb_cartographer++;
        break;
      }
      case WORKER:{
        // TODO : read worker memory
        break;
      }
      default: {}
    }
    for  (int j=0; j<256; j++){
      fourmis[i].memoire[j] = 0;
    }
  }
  switch (turn_type) {
    case RECUP_TURN: //TODO
      turn_type = BIRTH_TURN;
      // Si une fourmi récupérée est une cartographe, appeler `handle_cartographer_from_queen(ant)` et la renvoyer directement.
      return {.action = RECUPERER_FOURMI, .arg=0};


    case BIRTH_TURN : //TODO
      turn_type = RELEASE_TURN;
      return {.action = CREER_FOURMI, .arg=0};

    case RELEASE_TURN: //TODO
      for (unsigned int i=0; i<nb_fourmis; i++) {
        if (nb_cartographer != 0) {
          nb_cartographer --;
          // TODO : set cartographer memory
        }
        else if (nb_explorer != 0) {
          nb_explorer --;
          // TODO : set explorer memory
        }
        else {
          // TODO : set worker memory
        }
      }
      return {.action = ENVOYER_FOURMI, .arg=0};
      turn_type = RECUP_TURN;
  }
}