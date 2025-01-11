#pragma once

#include "game/queen.hpp"
#include "map/edge.hpp"
#include "map/node.hpp"
#include "sinfourmis.h"

class Edge;

enum class AntActionState {
    NONE,
    MOVING,
    DIGGING,
};

class Ant {
  public:
    Ant(Queen *queen, fourmi_etat &&etat);
    explicit Ant(Queen *queen);
    explicit Ant(const Ant &&ant) noexcept;
    Ant &operator=(const Ant &&ant) noexcept;
    ~Ant();

    Node *get_current_node() const;
    void set_current_node(Node *node);

    /**
     * @brief Apply `damages` damages to the ant
     *
     * @param damages
     */
    void apply_damages(uint8_t damages);

    /**
     * @brief Kill the ant
     *
     */
    void kill();

    /**
     * @brief Consume one unit of water. Kill the ant if the water is 0
     *
     */
    void consume_water();

    /**
     * @brief Check if the ant is alive
     *
     * @return true if the ant is alive
     */
    bool alive();

    /**
     * @brief Put the ant on the edge and put it in the moving state
     *
     * @param edge the edge to move along
     */
    void move_along(Edge *edge);
    /**
     * @brief Move the ant along the edge. The ant must be in the moving state
     *
     */
    void displace();

    /**
     * @brief Start the digging of the given edge
     *
     * @param edge the edge to dig
     */
    void begin_digging(Edge *edge);

    /**
     * @brief Dig the current edge. Crashes if the ant is not in digging state
     *
     * @param damages the value to repair the edge
     */
    void dig(unsigned int damages);

    /**
     * @brief Stop the digging
     *
     */
    void stop_digging();

    /**
     * @brief Gather food from the current node. The current node must be a food node
     *
     * @return unsigned int the quantity of gathered food
     */
    unsigned int gather_food();

    fourmi_etat &as_fourmi_etat() {
        return etat;
    }

    const fourmi_etat &as_fourmi_etat() const {
        return etat;
    }

    unsigned int get_team_id() const {
        return queen->get_team_id();
    }

    unsigned int get_attack() const {
        return queen->get_stat(Queen::Stat::ATTACK);
    }

    float get_progress() const {
        return progress;
    }

    void set_result(int32_t result) {
        this->etat.result = result;
    }

    AntActionState get_action_state() const {
        return action_state;
    }

    Edge *get_current_edge() const {
        return current_edge;
    }

    unsigned int get_id() const {
        return id;
    }

    Queen *get_queen() const {
        return queen;
    }

    unsigned int get_max_water() const {
        return max_water;
    }

    unsigned int get_max_food() const {
        return max_food;
    }

  private:
    Node *current_Node = nullptr;
    Queen *queen = nullptr;
    fourmi_etat etat;
    unsigned int max_water = 0;
    unsigned int max_food = 0;

    unsigned int id = 0;
    static unsigned int next_id;

    AntActionState action_state = AntActionState::NONE;
    float progress = 0;
    Edge *current_edge = nullptr;
};
