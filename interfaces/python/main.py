from enum import IntEnum, unique


@unique
class SalleType(IntEnum):
    VIDE = 0
    EAU = 1
    NOURRITURE = 2
    REINE = 3


@unique
class FourmiAction(IntEnum):
    DEPLACEMENT = 0
    RAMASSE_NOURRITURE = 1
    COMMENCE_CONSTRUCTION = 2
    TERMINE_CONSTRUCTION = 3
    ATTAQUE = 4
    ATTAQUE_TUNNEL = 5
    FOURMI_PASSE = 6


@unique
class ReineAction(IntEnum):
    CREER_FOURMI = 0
    ENVOYER_FOURMI = 1
    RECUPERER_FOURMI = 2
    AMELIORE_STOCKAGE = 3
    AMELIORE_PRODUCTION = 4
    AMELIORE_ENVOI = 5
    AMELIORE_VITESSE_AMELIORATION = 6
    AMELIORE_RAMASSAGE = 7
    AMELIORE_VIE = 8
    AMELIORE_EAU = 9
    AMELIORE_DEGATS = 10
    REINE_PASSE = 11

@unique
class PheromoneType(IntEnum):
    NO_PHEROMONE = 0
    PUBLIC = 1
    PRIVE = 2


def fourmi_retour(action, arg, depose_pheromone=PheromoneType.NO_PHEROMONE, pheromone=0):
    return {
        "action": action.value,
        "arg": arg,
        "depose_pheromone": depose_pheromone,
        "pheromone": pheromone,
    }


def reine_retour(action, arg):
    return {
        "action": action.value,
        "arg": arg,
    }


def fourmi_activation(fourmi_etat, salle):
    return fourmi_retour(FourmiAction.FOURMI_PASSE, 0)


def reine_activation(fourmis, reine_etat, salle):
    return reine_retour(ReineAction.REINE_PASSE, 0)
