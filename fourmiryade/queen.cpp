#include <map>
#include "sinfourmis.h"
#include "structures.cpp"


using namespace std;

#define min(a,b) (a)<(b)?(a):(b)

enum queen_turn_t {RECUP_TURN, RELEASE_TURN, BIRTH_TURN};
queen_turn_t turn_type = RECUP_TURN;
long turn_count = 0;




void read_memory (fourmi_etat) {}

void set_memory(); //TODO

reine_retour queen_main(fourmi_etat fourmis[], const unsigned int nb_fourmis, const reine_etat *etat, const salle *salle) {
  for (unsigned int i = 0; i<nb_fourmis; i++) {
    read_memory(fourmis[i]);
  }
  if (turn_count == 0) {
    turn_count +=1;
    return {.action = CREER_FOURMI, .arg = (int32_t)etat->nourriture/4};
  }
  else if (turn_count == 1) {
    turn_count +=1;
    for (unsigned int i=0; i<nb_fourmis; i++) {
      set_memory();
    }
  }
  else switch (turn_type) {
    case RECUP_TURN: //TODO
      turn_type = BIRTH_TURN;
      // Si une fourmi récupérée est une cartographe, appeler `handle_cartographer_from_queen(ant)` et la renvoyer directement.
      return {.action = RECUPERER_FOURMI, .arg=0};


    case BIRTH_TURN : //TODO
      turn_type = RELEASE_TURN;
      return {.action = CREER_FOURMI, .arg=0};

    case RELEASE_TURN: //TODO
      for (unsigned int i=0; i<nb_fourmis; i++) {
        set_memory();
      }
      return {.action = ENVOYER_FOURMI, .arg=0};
      turn_type = RECUP_TURN;
}
