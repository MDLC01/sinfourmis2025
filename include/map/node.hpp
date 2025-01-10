#pragma once

#include "game/ant.hpp"
#include "map/edge.hpp"
#include "sinfourmis.h"
#include "utils/salle_parser.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <unordered_map>

using json = nlohmann::json;

class Ant;
class Edge;

class Node {
  public:
    Node() = default;
    ~Node() = default;

    // for json serialization / deserialization
    friend void to_json(json &j, const Node &node);
    friend void from_json(const json &j, Node &node);

    /**
     * @brief Convert the node to a salle that can be sent to the ants simulation
     *
	 * @param team_id the id of the team to which the pheromones belong
	 * 
     * @return salle the salle representation of the node
     */
    salle as_salle(unsigned int team_id) const;

    /**
     * @brief Add an edge from this node to another node. It is undirected so the other node will
     * also have an edge to this node.
     *
     * @param other the node to add an edge to
     * @param life the life of the edge
     */
    void add_edge(Node &other, unsigned int life);

    /**
     * @brief Add an ant to the node as a visitor
     *
     * @param ant the ant to add
     */
    void add_ant(Ant *ant);

    /**
     * @brief Remove an ant from the node
     *
     * @param ant the ant to remove
     */
    void remove_ant(Ant *ant);

    /**
     * @param id the id of the edge to get
     * @return Edge* the edge with the given id
     */
    Edge *get_edge(unsigned int id) const;

    /**
     * @param id the id of the node to get the edge to
     * @return Edge* the edge to the given node id
     */
    Edge *get_edge_to_id(unsigned int id) const;

    /**
     * @param node the node to get the edge to
     * @return Edge* the edge to the given node
     */
    unsigned int get_id_to(const Node *node) const;

    /**
     * @brief remove `max_food` food from the node
     *
     * @param max_food the maximum amount of food to gather
     * @return unsigned int the actual amount of food gathered
     */
    unsigned int gather_food(unsigned int max_food);

    /**
     * @brief Regenerate food on the node by `NODE_FOOD_REGEN`
     *
     */
    void regen_food();

    void set_pheromone(uint8_t pheromone, unsigned int team_id) {
        this->pheromones.at(team_id) = pheromone;
    }

    unsigned int get_id() const {
        return id;
    }
    std::vector<std::shared_ptr<Edge>> get_edges() const {
        return edges;
    }
    std::vector<std::shared_ptr<Edge>> &get_edges() {
        return edges;
    }

    const std::map<unsigned int, std::vector<Ant *>> &get_ants() const {
        return ants;
    }

    std::vector<Ant *> &get_team_ants(unsigned int team) {
        return ants.at(team);
    }

    salle_type get_type() const {
        return type;
    }

    unsigned int get_food() const {
        if (type != salle_type::NOURRITURE) {
            throw std::runtime_error("Node is not a food node");
        }
        return food;
    }

    unsigned int get_max_food() const {
        if (type != salle_type::NOURRITURE) {
            return 0;
        }
        return max_food;
    }

    unsigned int get_remaining() const {
        if (type != salle_type::NOURRITURE) {
            return 0;
        }
        return total_available;
    }

    unsigned int get_team() const {
        return team;
    }

    size_t degree() const {
        return edges.size();
    }

    std::pair<float, float> get_position() const {
        return {x, y};
    }

    float get_x() const {
        return x;
    }

    float get_y() const {
        return y;
    }
	
	const std::unordered_map<unsigned int, uint8_t> &get_pheromones() const {
		return pheromones;
	}

  private:
    void remove_edge(const std::shared_ptr<Edge> &edge);

    unsigned int id = 0;
    salle_type type = salle_type::VIDE;
    float x = 0;
    float y = 0;
    std::unordered_map<unsigned int, uint8_t> pheromones;

    unsigned int food = 0;
    unsigned int regen = 0;
    unsigned int max_food = 0;
    int total_available = 0;

    unsigned int team = 0;

    std::vector<std::shared_ptr<Edge>> edges;

    /// ants[team_id] is the ants present in this Node belonging to the given team
    std::map<unsigned int, std::vector<Ant *>> ants;
};

std::ostream &operator<<(std::ostream &os, const salle_type &type);
