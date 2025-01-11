#include "game/queen.hpp"

const std::array<uint32_t, 4> Queen::upgrade_costs = {
    QUEEN_UPGRADE_LIFE_COST, QUEEN_UPGRADE_WATER_COST, QUEEN_UPGRADE_FOOD_COST,
    QUEEN_UPGRADE_ATTACK_COST};

const std::array<uint32_t, 4> Queen::queen_upgrade_costs = {
    QUEEN_UPGRADE_NB_ANT_COST, QUEEN_UPGRADE_PRODUCED_ANTS_COST, QUEEN_REDUCE_UPGRADE_TIME_COST,
    QUEEN_UPGRADE_NB_ANT_SENDING_COST};

std::ostream &operator<<(std::ostream &os, const Queen::Stat &stat) {
    switch (stat) {
        case Queen::Stat::LIFE:
            os << "LIFE";
            break;
        case Queen::Stat::WATER:
            os << "WATER";
            break;
        case Queen::Stat::FOOD:
            os << "FOOD";
            break;
        case Queen::Stat::ATTACK:
            os << "ATTACK";
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Queen::QueenStat &stat) {
    switch (stat) {
        case Queen::QueenStat::MAX_STORED_ANTS:
            os << "MAX_STORED_ANTS";
            break;
        case Queen::QueenStat::PRODUCTION_DELAY:
            os << "PRODUCED_ANTS";
            break;
        case Queen::QueenStat::UPGRADE_DURATION:
            os << "UPGRADE_DURATION";
            break;
        case Queen::QueenStat::ANTS_SENDING:
            os << "ANTS_SENDING";
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Queen &queen) {
    os << "===== Queen =====" << std::endl;
    os << "Team: " << queen.team->get_id() << std::endl;
    os << "Stats:" << std::endl;
    for (uint32_t i = 0; i < 4; i++) {
        os << "\t" << Queen::Stat(i) << ": " << queen.stats[i] << std::endl;
    }
    os << "Queen Stats:" << std::endl;
    for (uint32_t i = 0; i < 4; i++) {
        os << "\t" << Queen::QueenStat(i) << ": " << queen.queen_stats[i] << std::endl;
    }
    os << "Food: " << queen.team->get_food() << std::endl;
    os << "Stored ants: " << queen.ants_memory.size() << std::endl;
    if (queen.waiting_upgrade > 0) {
        os << "Waiting remaining: " << queen.waiting_upgrade << std::endl;
    }
    return os;
}

void Queen::game_turn() {
    if (waiting_upgrade > 0) {
        waiting_upgrade--;
    }
}

bool Queen::can_perform_action() const {
    return waiting_upgrade == 0;
}

bool Queen::is_upgrading() const {
    return waiting_upgrade > 0;
}

bool Queen::upgrade(Stat type) {
    if (waiting_upgrade > 0) {
        return false;
    }
    if (!team->try_remove_food(upgrade_costs[(unsigned int)type])) {
        return false;
    }
    upgrading = IsUpgrading::ANTS;
    current_ants_upgrade = type;
    std::cout << "Queen " << team->get_id() << " upgrade " << (unsigned int)type << std::endl;
    if (type == Stat::WATER) {
        stats[(unsigned int)type] += 5;
    } else {
        stats[(unsigned int)type]++;
    }
    waiting_upgrade = get_queen_stat(QueenStat::UPGRADE_DURATION);
    return true;
}

bool Queen::upgrade_queen(QueenStat type) {
    if (waiting_upgrade > 0) {
        return false;
    }
    if (!team->try_remove_food(queen_upgrade_costs[(uint32_t)type])) {
        return false;
    }
    upgrading = IsUpgrading::QUEEN;
    current_queen_upgrade = type;
    std::cout << "Queen " << team->get_id() << " upgrade " << (unsigned int)type << std::endl;
    switch (type) {
        case QueenStat::UPGRADE_DURATION:
            {
                uint32_t &upgrade_duration = queen_stats[(uint32_t)type];
                if (upgrade_duration > 1) {
                    upgrade_duration--;
                }
            }
            break;
        default:
            queen_stats[(uint32_t)type]++;
            break;
    }
    waiting_upgrade = get_queen_stat(QueenStat::UPGRADE_DURATION);
    return true;
}

std::string Queen::current_upgrade() {
    std::stringstream ss;
    switch (upgrading) {
        case IsUpgrading::ANTS:
            ss << current_ants_upgrade;
            break;
        case IsUpgrading::QUEEN:
            ss << current_queen_upgrade;
            break;
        case IsUpgrading::NONE:
            ss << "NONE";
            break;
    }
    return ss.str();
}

void Queen::remove_food(uint32_t food) {
	assert(team->try_remove_food(food));
}

uint32_t Queen::get_stat(Stat type) const {
    return stats[(uint32_t)type];
}

uint32_t Queen::get_queen_stat(QueenStat type) const {
    return queen_stats[(uint32_t)type];
}

reine_etat Queen::as_reine_etat() const {
    reine_etat etat;
    etat.result = result;
	etat.team_id = team->get_id();
    etat.nourriture = team->get_food();
    etat.max_nourriture = get_stat(Stat::FOOD);
    etat.max_eau = get_stat(Stat::WATER);
    etat.max_vie = get_stat(Stat::LIFE);
    etat.max_degats = get_stat(Stat::ATTACK);
    etat.duree_amelioration = get_queen_stat(QueenStat::UPGRADE_DURATION);
    etat.max_stockage = get_queen_stat(QueenStat::MAX_STORED_ANTS);
    etat.max_production = get_queen_stat(QueenStat::PRODUCTION_DELAY);
    etat.max_envoi = get_queen_stat(QueenStat::ANTS_SENDING);
    return etat;
}

fourmi_etat Queen::default_fourmi_etat() const {
    fourmi_etat etat;
    etat.result = -1;

    etat.vie = std::min((uint8_t)(UINT8_MAX - 1), (uint8_t)get_stat(Stat::LIFE));

    etat.eau = get_stat(Stat::WATER);
    etat.nourriture = 0;
    memset(etat.memoire, 0, sizeof(etat.memoire));
    return etat;
}
bool Queen::create_ant() {
    if (ants_memory.size() >= get_queen_stat(QueenStat::MAX_STORED_ANTS)) {
        return false;
    }
    ants_memory.push_back(default_fourmi_etat());
    return true;
}

void Queen::add_production_delay() {
    waiting_upgrade += Queen::queen_upgrade_costs[(uint32_t)Queen::QueenStat::PRODUCTION_DELAY];
}
bool Queen::push_ant(fourmi_etat ant) {
    if (ants_memory.size() >= get_queen_stat(QueenStat::MAX_STORED_ANTS)) {
        return false;
    }
    team->add_food(ant.nourriture);
    ants_memory.push_back(ant);
    return true;
}

std::optional<fourmi_etat> Queen::pop_ant() {
    if (ants_memory.empty()) {
        return std::nullopt;
    }
    fourmi_etat ant = ants_memory.back();
    ants_memory.pop_back();
    ant.nourriture = 0;
    ant.eau = get_stat(Stat::WATER);
    ant.vie = std::min((uint8_t)(UINT8_MAX - 1), (uint8_t)get_stat(Stat::LIFE));
    return ant;
}
