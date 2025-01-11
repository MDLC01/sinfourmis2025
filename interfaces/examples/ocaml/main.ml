let fourmi_activation (etat : Sinfourmis.fourmi_etat) (salle : Sinfourmis.salle) : Sinfourmis.fourmi_retour =
  etat.memoire.(0) <- char_of_int ((int_of_char etat.memoire.(0)) + 1);
  (if etat.memoire.(0) = char_of_int 1 then
    print_endline "Fourmi : Je suis une fourmi"
  else
    print_endline "Fourmi : Je suis une fourmi qui sait compter"
  );
  {action = FOURMI_PASSE; arg = 0; pheromone_type = NO_PHEROMONE; pheromone = 0}


let nb_sent = ref 0
let reine_activation (fourmis : Sinfourmis.fourmi_etat array) (reine : Sinfourmis.reine_etat) (salle : Sinfourmis.salle) : Sinfourmis.reine_retour =
  if !nb_sent > 2 then
    {action = REINE_PASSE; arg = 0}
  else if Array.length fourmis < 1 then (
    print_endline "Création d'une fourmis";
    {action = CREER_FOURMI; arg = 1})
  else (
    for i = 0 to 255 do
      fourmis.(0).memoire.(i) <- char_of_int i
    done;
    print_endline "Envoi d'une fourmi";
    nb_sent := !nb_sent + 1;
    {action = ENVOYER_FOURMI; arg = 1}
  )

let _ = Sinfourmis.register_functions fourmi_activation reine_activation

