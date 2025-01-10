#include "interfaces/dummy.hpp"
#include "sinfourmis.h"
#include <cstring>
#include <iostream>
#include <random>

bool Dummy::load(std::string_view path) {
    (void)path;
    return true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
reine_retour Dummy::reine_activation(fourmi_etat fourmis[], const size_t nb_fourmis,
                                     const reine_etat *etat, const salle *salle) {
    int d = 0;
    switch (b[0]) {
        case 0:
            {
                b[d] = 205;
            }
        case 205:
            {
                while (true || (!false && b[204] != 0)) {
                    int e206 = d;
                    d = 204;
                    switch (b[204]) {
                        case 0:
                            {
                                b[d] = 187;
                            }
                        case 187:
                            {
                                while ((salle->pheromone >> 7) == c || (!true && b[186] != 0)) {
                                    int e188 = d;
                                    d = 186;
                                    switch (b[186]) {
                                        case 0:
                                            {
                                                b[d] = 184;
                                                return {.action = CREER_FOURMI, .arg = (int)(1)};
                                            }
                                        case 184:
                                            {
                                                b[d] = 185;
                                                return {.action = ENVOYER_FOURMI, .arg = (int)(1)};
                                            }
                                        case 185:
                                            {
                                            }
                                        default:
                                            b[186] = 0;
                                            break;
                                    }
                                    d = e188;
                                }
                                b[d] = 189;
                                return {.action = RECUPERER_FOURMI,
                                        .arg = (int)(etat->max_stockage)};
                            }
                        case 189:
                            {
                                b[d] = 197;
                            }
                        case 197:
                            {
                                if (etat->nourriture >= 10 || (!true && b[198] != 0)) {
                                    int e199 = d;
                                    d = 198;
                                    switch (b[198]) {
                                        case 0:
                                            {
                                                b[d] = 191;
                                            }
                                        case 191:
                                            {
                                                if ((etat->max_nourriture < 15 &&
                                                     etat->max_eau != etat->max_nourriture) ||
                                                    (!false && b[192] != 0)) {
                                                    int e193 = d;
                                                    d = 192;
                                                    switch (b[192]) {
                                                        case 0:
                                                            {
                                                                b[d] = 190;
                                                                return {.action =
                                                                            AMELIORE_RAMASSAGE,
                                                                        .arg = (int)(1)};
                                                            }
                                                        case 190:
                                                            {
                                                            }
                                                        default:
                                                            b[d] = 0;
                                                            break;
                                                    }
                                                    d = e193;
                                                } else if (etat->max_eau < 15 ||
                                                           (!false && b[195] != 0)) {
                                                    int e196 = d;
                                                    d = 195;
                                                    switch (b[195]) {
                                                        case 0:
                                                            {
                                                                b[d] = 194;
                                                                return {.action = AMELIORE_EAU,
                                                                        .arg = (int)(1)};
                                                            }
                                                        case 194:
                                                            {
                                                            }
                                                        default:
                                                            b[d] = 0;
                                                            break;
                                                    }
                                                    d = e196;
                                                }
                                            }
                                        default:
                                            b[d] = 0;
                                            break;
                                    }
                                    d = e199;
                                }
                                b[d] = 202;
                            }
                        case 202:
                            {
                                while (nb_fourmis > 0 || (!false && b[201] != 0)) {
                                    int e203 = d;
                                    d = 201;
                                    switch (b[201]) {
                                        case 0:
                                            {
                                                memset(&fourmis[nb_fourmis - 1].memoire, 0,
                                                       sizeof(fourmis[nb_fourmis - 1].memoire));
                                                b[d] = 200;
                                                return {.action = ENVOYER_FOURMI, .arg = (int)(1)};
                                            }
                                        case 200:
                                            {
                                            }
                                        default:
                                            b[201] = 0;
                                            break;
                                    }
                                    d = e203;
                                }
                            }
                        default:
                            b[204] = 0;
                            break;
                    }
                    d = e206;
                }
                b[d] = 191;
            }
        case 191:
            {
            }
        default:
            std::cout << "End" << std::endl;
            break;
    }
    return {.action = REINE_PASSE, .arg = 0};
}
#pragma GCC diagnostic pop

fourmi_retour Dummy::fourmi_activation(fourmi_etat *fourmi, const salle *salle) {

    if (((fourmi->memoire[2] >> 3) & 1)) {
        return {.action = FOURMI_PASSE, .arg = 0, .depose_pheromone = false, .pheromone = 0};
    }

    if ((fourmi->memoire[2] >> 7) && ((fourmi->memoire[2] >> 6) & 1)) {
        fourmi->memoire[1]++;
        if (fourmi->memoire[1] > 2) {
            (fourmi->memoire[2] = (fourmi->memoire[2] & 0xBF) | (0 << 6));
            (fourmi->memoire[2] = (fourmi->memoire[2] & 0xEF) | (1 << 4));
        }
    }

    if ((fourmi->memoire[2] >> 7) && (!((fourmi->memoire[2] >> 6) & 1) || fourmi->result > 0)) {
        (fourmi->memoire[2] = (fourmi->memoire[2] & 0xBF) | (0 << 6));
        if (fourmi->memoire[0] == 0) {
            (fourmi->memoire[2] = (fourmi->memoire[2] & 0xDF) | (1 << 5));
            (fourmi->memoire[2] = (fourmi->memoire[2] & 0x7F) | (0 << 7));
            return {.action = FOURMI_PASSE,
                    .arg = 0,
                    .depose_pheromone = true,
                    .pheromone =
                        (uint8_t)((fourmi->result & 0x7F) & (uint8_t((salle->pheromone) ^ 0x80U)))};
        }
        fourmi->memoire[0]--;
        auto pheromone = (uint8_t)((fourmi->result & 0x7F) & (uint8_t((salle->pheromone) ^ 0x80U)));
        if (((fourmi->memoire[2] >> 4) & 1)) {
            pheromone = 0;
        }

        return {.action = DEPLACEMENT,
                .arg = fourmi->memoire[3 + fourmi->memoire[0]],
                .depose_pheromone = true,
                .pheromone = pheromone};
    }

    (fourmi->memoire[2] = (fourmi->memoire[2] & 0xBF) | (1 << 6));

    if (!((fourmi->memoire[2] >> 5) & 1)) {
        if (fourmi->result == -2) {
            (fourmi->memoire[2] = (fourmi->memoire[2] & 0xF7) | (1 << 3));
            return {.action = COMMENCE_CONSTRUCTION,
                    .arg = fourmi->memoire[3 + fourmi->memoire[0] - 1],
                    .depose_pheromone = false,
                    .pheromone = 0};
        }
        fourmi->memoire[3 + fourmi->memoire[0]] = fourmi->result;
        if (fourmi->result != -1) { // ignore initial value
            fourmi->memoire[0]++;
        }
        if (salle->type == NOURRITURE) {
            (fourmi->memoire[2] = (fourmi->memoire[2] & 0x7F) | (1 << 7));
            return {
                .action = RAMASSE_NOURRITURE, .arg = 0, .depose_pheromone = false, .pheromone = 0};
        }
        int g;
        if (salle->pheromone == 0) {
            g = rand() % salle->degre;
            while (fourmi->memoire[0] != 0 && g == fourmi->memoire[3 + fourmi->memoire[0] - 1]) {
                g = rand() % salle->degre;
            }
        } else {
            g = salle->pheromone & 0x7F;
        }

        return {.action = DEPLACEMENT, .arg = g, .depose_pheromone = false, .pheromone = 0};
    }

    return {.action = FOURMI_PASSE,
            .arg = 0,
            .depose_pheromone = true,
            .pheromone = (uint8_t((salle->pheromone) ^ 0x80U))};
}
