extern crate sinfourmis;

use sinfourmis::{FourmiAction, FourmiEtat, FourmiRetour, ReineAction, ReineRetour, Salle, PheromoneType};

#[no_mangle]
pub extern "C" fn fourmi_activation(etat: *const FourmiEtat, salle: *const Salle) -> FourmiRetour {
    FourmiRetour {
        action: FourmiAction::Deplacement,
        arg: 1,
        depose_pheromone: PheromoneType::NoPheromone,
        pheormone: 0,
    }
}

#[no_mangle]
pub extern "C" fn reine_activation(
    fourmis_ptr: usize,
    nb_fourmis: usize,
    salle: *const Salle,
) -> ReineRetour {
    let fourmis;
    unsafe {
        let slice = std::slice::from_raw_parts(fourmis_ptr as *const FourmiEtat, nb_fourmis);
        fourmis = slice.to_vec();
    }

    let mut mem: [u8; 256] = [0; 256];
    let i = fourmis[0].nourriture;
    mem[0] = i as u8;
    ReineRetour {
        action: ReineAction::RecupererFourmi,
        arg: 0,
    }
}
