/// décrit le type d'une salle
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub enum SalleType {
    Vide,
    Eau,
    Nourriture,
    Reine,
}

/// décrit l'action effectuée par une fourmi
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub enum FourmiAction {
    Deplacement,
    RamasseNourriture,
    CommenceConstruction,
    TermineConstruction,
    Attaque,
    AttaqueTunnel,
    Passe,
}

/// décrit l'action effectuée par la reine
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub enum ReineAction {
    CreerFourmi,
    EnvoyerFourmi,
    RecupererFourmi,
    AmelioreStockage,
    AmelioreProduction,
    AmelioreEnvoi,
    AmelioreVitesseAmelioration,
    AmelioreRamassage,
    AmelioreVie,
    AmelioreEau,
    AmelioreDegats,
    Passe,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct FourmisCompteur {
    pub equipe: u32,
    pub nombre: u32,
}

/// structure représentant une salle
#[repr(C)]
#[derive(Debug, Clone)]
pub struct Salle {
    pub salle_type: SalleType,
    pub pheromone: u8,
    pub degre: i32,
    pub taille_liste: usize,
    pub compteurs_fourmis: *const FourmisCompteur,
}

/// structure représentant l'état d'une fourmi
///
/// Cette structure est utilisée lors de l'éveil d'une fourmi. Le champ `memoire`
/// n'est jamais modifié entre deux activations.
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct FourmiEtat {
    pub vie: u8,
    pub memoire: [u8; 256],
    pub result: i32,
    /// valeur dela réserve d'eau (en nombre de tours)
    pub eau: i32,
    pub nourriture: i32,
}

/// structure décrivant une action de fourmi
///
/// Cette structure est renvoyée par la fonction d'activation des fourmis. Elle
/// décrit le type d'action réalisé à la fin du tour, et contient l'argument de
/// l'action correspondante (pas toujours nécessaire)
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct FourmiRetour {
    pub action: FourmiAction,
    pub arg: i32,
    pub depose_pheromone: bool,
    pub pheormone: u8,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
/// structure représentant l'état d'une reine
///
/// Cette structure est utilisée lors de l'éveil d'une reine.
/// Elle contient les fourmis gérées par la reine, les points de nourriture
/// et le résultat de l'action précédente
pub struct ReineEtat {
    nourriture: u32,
	team_id: u32,
    result: i32,
    max_nourriture: u32,
    max_eau: u32,
    max_vie: u32,
    max_degats: u32,
    dureee_amelioration: u32,
    max_stocakge: u32,
    max_production: u32,
    max_envoi: u32,
}

/// structure décrivant une action de la reine
///
/// Cette structure est renvoyée par la fonction d'activation de la reine. Elle
/// décrit le type d'action réalisé à la fin du tour, et contient la mémoire
/// initiale de la fourmi à envoyer (si l'action est un envoi de fourmi)
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct ReineRetour {
    pub action: ReineAction,
    pub arg: i32,
}
