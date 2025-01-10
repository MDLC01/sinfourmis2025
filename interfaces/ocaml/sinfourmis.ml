type salle_type =
  | VIDE
  | EAU
  | NOURRITURE
  | REINE

type fourmi_action =
  | DEPLACEMENT
  | RAMASSE_NOURRITURE
  | COMMENCE_CONSTRUCTION
  | TERMINE_CONSTRUCTION
  | ATTAQUE
  | ATTAQUE_TUNNEL
  | FOURMI_PASSE

type reine_action =
  | CREER_FOURMI
  | ENVOYER_FOURMI
  | RECUPERER_FOURMI
  | AMELIORE_STOCKAGE
  | AMELIORE_PRODUCTION
  | AMELIORE_ENVOI
  | AMELIORE_VITESSE_AMELIORATION
  | AMELIORE_RAMASSAGE
  | AMELIORE_VIE
  | AMELIORE_EAU
  | AMELIORE_DEGATS
  | REINE_PASSE

type salle = {
  salle_type : salle_type;
  pheromone : int;
  degre : int;
  compteurs_fourmis : (int * int) list
}

type fourmi_etat = {
  vie : int;
  memoire : char array;
  result : int;
  eau : int;
  nourriture : int
}

type fourmi_retour = {
  action : fourmi_action;
  arg : int;
  pheromone : int option
}

type reine_etat = {
  nourriture : int;
  team_id : int;
  result : int;
  max_nourriture : int;
  max_eau : int;
  max_vie : int;
  max_degats : int;
  duree_amelioration : int;
  max_stockage : int;
  max_production : int;
  max_envoi : int
}

type reine_retour = {
  action : reine_action;
arg : int
}

let register_functions
  (fourmi_activation : fourmi_etat -> salle -> fourmi_retour)
  (reine_activation  : fourmi_etat array -> reine_etat -> salle -> reine_retour)
  =
  Callback.register "fourmi_activation" fourmi_activation;
  Callback.register "reine_activation" reine_activation
