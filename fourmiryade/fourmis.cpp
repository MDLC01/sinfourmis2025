#include "sinfourmis.h"
#include "memory.cpp"
#include "cartographer.cpp"
#include "explorer.cpp"
#include "workers.cpp"
#include "queen.cpp"
#include <iostream>


reine_retour cpp_reine_activation(fourmi_etat fourmis[], const unsigned int nb_fourmis,
                                  const reine_etat *etat, const salle *salle) {
    return queen_main(fourmis, nb_fourmis, etat, salle);
}

fourmi_retour cpp_fourmi_activation(fourmi_etat *etat, const salle *salle) {
    rw rw = create_rw(etat->memoire);
    type_fourmi type = read_type(&rw);

    switch (read_type(&rw)) {
        case EXPLORER: {
            return explorer_activation(etat, salle);
        }
        case WORKER: {
            return worker_activation(etat, salle);
            assert(false);
        }
        case CARTOGRAPHER: {
            return cartographer_activation(etat, &rw, salle);
        }
        case VARIANT_COUNT: {
            assert(false);
        }
    }

    assert(false);
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
