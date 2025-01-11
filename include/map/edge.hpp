#pragma once

#include "game/constants.hpp"
#include "map/node.hpp"
#include "nlohmann/json.hpp"
#include <cmath>
#include <unordered_set>

class Node;
class Ant;

using json = nlohmann::json;

/**
 * @brief Represents an edge between two nodes.
 *
 */
class Edge {
  public:
    Edge() = default;
    Edge(Node *n1, Node *n2, unsigned int life);

    /**
     * @brief Get the other side node of the edge.
     *
     * @param node the current node
     * @return Node* the other node
     */
    Node *get_other_node(const Node *node) const;

    /**
     * @brief Check if the edge can be crossed
     *
     * @return true if the edge can be crossed
     */
    bool can_be_crossed() const;

    /**
     * @brief Apply damages to the edge
     *
     * @param damages the damages to apply to the edge
     * @return unsigned int the remaining life of the edge
     */
    unsigned int attack(unsigned int damages);

    /**
     * @brief Dig (Heal) the edge
     *
     * @param damages the value to repair the edge
     */
    void dig(unsigned int damages);

    float get_length() const {
        return length;
    }

    Node *get_node1() const {
        return node1;
    }
    Node *get_node2() const {
        return node2;
    }

    void add_ant(Ant *ant) {
        ants.insert(ant);
    }

    void remove_ant(Ant *ant) {
        ants.erase(ant);
    }

    unsigned int get_life() const {
        return life;
    }

    const std::unordered_set<Ant *> &get_ants() const {
        return ants;
    }

  private:
    Node *node1 = nullptr;
    Node *node2 = nullptr;
    float length = 0;
    unsigned int life = EDGE_LIFE;
    std::unordered_set<Ant *> ants;
};
