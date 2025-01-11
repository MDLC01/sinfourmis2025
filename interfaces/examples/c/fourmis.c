#include "sinfourmis.h"
#include "stdio.h"

reine_retour reine_activation(fourmi_etat fourmis[], const unsigned int nb_fourmis,
                                  const reine_etat *etat, const salle *salle) {
    (void)etat;
    (void)salle;
    static int nb_sent = 0;
    if (nb_sent > 2) {
        return (reine_retour){.action = REINE_PASSE, .arg = 0};
    }
    if (nb_fourmis < 1) {
        printf("Création d'une fourmis\n");
        return (reine_retour){.action = CREER_FOURMI, .arg = 1};
    }
    for (int i = 0; i < 256; i++) {
        fourmis[0].memoire[i] = (char)i;
    }
    printf("Envoi d'une fourmi\n");
    nb_sent += 1;
    return (reine_retour){.action = ENVOYER_FOURMI, .arg = 1};
}

fourmi_retour fourmi_activation(fourmi_etat *etat, const salle *salle) {
    (void)salle;
    etat->memoire[0] += 1;
    if (etat->memoire[0] == 1) {
        printf("Fourmi: Je suis une fourmi\n");
    } else {
        printf("Fourmi: Je suis une fourmi et je sais compter\n");
    }
    return (fourmi_retour){.action = FOURMI_PASSE, .arg = 0, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
}
