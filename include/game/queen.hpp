#pragma once

#include "game/constants.hpp"
#include "game/team.hpp"
#include "sinfourmis.h"
#include <array>
#include <cstring>
#include <iostream>
#include <optional>
#include <vector>

class Node;

class Queen {
  public:
    Queen(Team *team, Node *node) : team(team), current_node(node) {}

    enum class Stat : uint32_t { LIFE, WATER, FOOD, ATTACK };
    enum class QueenStat : uint32_t {
        MAX_STORED_ANTS,
        PRODUCTION_DELAY,
        UPGRADE_DURATION,
        ANTS_SENDING
    };

    const static std::array<uint32_t, 4> upgrade_costs;
    const static std::array<uint32_t, 4> queen_upgrade_costs;
    /**
     * @brief Signal the queen that a game turn passed
     *
     */
    void game_turn();

    /**
     * @brief Check if the queen can perform an actions
     *
     * @return true if the queen can perform an action
     */
    bool can_perform_action() const;

    /**
     * @brief Check if the queen is upgrading
     *
     * @return true if the queen is upgrading
     */
    bool is_upgrading() const;

    /**
     * @brief Run the upgrade of the given ant stat
     *
     * @param type the stat to upgrade
     * @return true if the upgrade can be performed
     */
    bool upgrade(Stat type);

    /**
     * @brief Run the upgrade for the given queen stat
     *
     * @param type the stat to upgrade
     * @return true if the upgrade can be performed
     */
    bool upgrade_queen(QueenStat type);

	void remove_food(uint32_t food);

    std::string current_upgrade();

    uint32_t get_stat(Stat type) const;
    uint32_t get_queen_stat(QueenStat type) const;
    reine_etat as_reine_etat() const;

    fourmi_etat default_fourmi_etat() const;

    /**
     * @brief create a new ant in the queen storage
     *
     * @return true if the ant has been created
     */
    bool create_ant();

    /**
     * @brief put the queen in production mode (i.e: wait for the [PRODUCTION_DELAY] turns)
     *
     */
    void add_production_delay();

    /**
     * @brief push the given ant to the queen storage
     *
     * @param ant the ant state tu push
     * @return true if the ant has been successfully pushed
     */
    bool push_ant(fourmi_etat ant);

    /**
     * @brief pop an ant state from the queen storage
     *
     * @return std::optional<fourmi_etat> the state popped or `std::nullopt` if here is no ants to
     * pop
     */
    std::optional<fourmi_etat> pop_ant();

    unsigned int get_food() const {
        return team->get_food();
    }

    unsigned int get_team_id() const {
        return team->get_id();
    }

    Node *get_current_node() const {
        return current_node;
    }

    std::vector<fourmi_etat> &get_states() {
        return ants_memory;
    }

    void set_result(int32_t new_result) {
        this->result = new_result;
    }

    friend std::ostream &operator<<(std::ostream &os, const Queen &queen);

  private:
    Team *team = nullptr;
    Node *current_node = nullptr;
    int32_t result = 0;
    unsigned int waiting_upgrade = 0;

    enum class IsUpgrading { ANTS, QUEEN, NONE };
    IsUpgrading upgrading = IsUpgrading::NONE;
    QueenStat current_queen_upgrade;
    Stat current_ants_upgrade;

    std::array<uint32_t, 4> stats{DEFAULT_MAX_LIFE, DEFAULT_MAX_WATER, DEFAULT_MAX_FOOD,
                                  DEFAULT_ATTACK};

    std::array<uint32_t, 4> queen_stats{QUEEN_DEFAULT_MAX_STORED_ANT,
                                        QUEEN_DEFAULT_PRODUCTION_DELAY,
                                        QUEEN_DEFAULT_UPGRADE_DURATION, QUEEN_DEFAULT_SENT_ANTS};

    std::vector<fourmi_etat> ants_memory;
};

std::ostream &operator<<(std::ostream &os, const Queen::Stat &stat);
std::ostream &operator<<(std::ostream &os, const Queen::QueenStat &stat);
