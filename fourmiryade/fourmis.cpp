#include "sinfourmis.h"
#include "memory.cpp"
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

fourmi_retour exploratrice(fourmi_etat *etat, const salle *salle) {
	// I
	// On suppose la mémoire initialisée

	// decrémenter limite eau / mémoire ?
	
	if get_memoire() {
		// Trajet vers src départ
		// Suivre chemin ?
		return {.action = DEPLACEMENT, .arg = XXX, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
	}

	if (get_memoire(trajet) == 0) {
		if (!EXPLOREE && (salle->salle_type == EAU || (salle->salle_type == NOURRITURE && salle->nourriture > 0))) {
			pheromone();
			set_memoire(trajet, 1);
			return {.action = DEPLACEMENT, .arg = XXX, .depose_pheromone = PRIVE, .pheromone = 1};
		}

		if (LIMITE) {
			set_memoire(trajet, 1);
			return {.action = FOURMI_PASSE, .arg = 0, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
		}

		if () {

		}
		
		// déplacement-construction (edge)
		return {.action = FOURMI_PASSE, .arg = 0, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};

	}

	else {
		// Trajet vers src départ
		// Suivre chemin retour ?
	}
}

fourmi_retour cpp_fourmi_activation(fourmi_etat *etat, const salle *salle) {
	if (get_type(self) == EXPLORATRICE) {
		return exploratrice(etat, salle);
	}

	if (get_type(self) == OUVRIERE) {
		return ouvriere(etat, salle);
	}

	if (get_type(self) == CARTOGRAPHE) {
		return cartographe(self, salle);
	}
}

/*
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
*/

extern "C" {
reine_retour reine_activation(fourmi_etat fourmis[], const unsigned int nb_fourmis,
                              const reine_etat *etat, const salle *salle) {
    return cpp_reine_activation(fourmis, nb_fourmis, etat, salle);
}

fourmi_retour fourmi_activation(fourmi_etat *etat, const salle *salle) {
    return cpp_fourmi_activation(etat, salle);
}
}
