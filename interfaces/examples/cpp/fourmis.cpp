#include "sinfourmis.h"
#include <iostream>

reine_retour cpp_reine_activation(fourmi_etat fourmis[], const unsigned int nb_fourmis,
                                  const reine_etat *etat, const salle *salle) {
	(void)etat;
	(void)salle;
    static int nb_sent = 0;
	if (nb_sent > 2) {
		return {.action = REINE_PASSE, .arg = 0};
	}
	if (nb_fourmis < 1) {
		std::cout << "Création d'une fourmis" << std::endl;
		return {.action = CREER_FOURMI, .arg = 1};
	}
	for (int i = 0; i < 256; i++) {
		fourmis[0].memoire[i] = i;
	}
	std::cout << "Envoi d'une fourmi" << std::endl;
	nb_sent += 1;
	return {.action = ENVOYER_FOURMI, .arg = 1};
}

fourmi_retour cpp_fourmi_activation(fourmi_etat *etat, const salle *salle) {
	(void)salle;
	etat->memoire[0] += 1;
	if (etat->memoire[0] == 1) {
		std::cout << "Fourmi: Je suis une fourmi" << std::endl;
	} else {
		std::cout << "Fourmi: Je suis une fourmi et je sais compter" << std::endl;
	}
	return {.action = FOURMI_PASSE, .arg = 0, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
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