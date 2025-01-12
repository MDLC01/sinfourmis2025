#pragma once

#include "game/team.hpp"
#include "map/neighbor_data.hpp"
#include "map/node.hpp"
#include "sinfourmis.h"
#include "utils/pair_hash.hpp"
#include "utils/salle_parser.hpp"
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * @brief Represents the game map
 *
 */
class Map {
  public:
    Map() = default;
    Map(Map &&other);
    Map &operator=(Map &&other);
    Map(const Map &other) = delete;
    Map &operator=(const Map &other) = delete;

    /**
     * @brief Load the map from a file
     *
     * @param filename the file to load the map from
     * @return true if the map was loaded successfully
     * @return false if the map could not be loaded because of an error or a malformed data type
     */
    bool load(const std::string_view &filename, bool gala);

    /**
     * @brief Function used for debugging purposes to print the map to a .dot file
     *
     * @param filename the file to write the map to
     */
    void to_dot(const std::string_view &filename) const;

    /**
     * @brief Regenerate the food on the map
     *
     */
    void regen_food();

    /**
     * @brief Get the starting node for a team
     *
     * @param team_id the id of the team
     * @return the starting node for the team
     */
    Node *get_starting_node(unsigned int team_id);

    /**
     * @brief Get the edge between two nodes
     *
     * @param id1 the id of the first node
     * @param id2 the id of the second node
     * @return Edge* the edge between the two nodes
     */
    Edge *get_edge(unsigned int id1, unsigned int id2) const;

    size_t get_node_count() const {
        return nodes.size();
    }

    size_t get_team_count() const {
        return teams.size();
    }

    std::vector<Team> &get_teams() {
        return teams;
    }

    const std::vector<Team> &get_teams() const {
        return teams;
    }

    const std::unordered_map<unsigned int, Node> &get_nodes() const {
        return nodes;
    }

    const Team &get_team(unsigned int id) const {
        return teams.at(id);
    }

    const Node &get_node(unsigned int id) const {
        return nodes.at(id);
    }

  private:
    bool load_teams(const json &data);
    bool load_nodes(const json &data);

    std::vector<Team> teams;
    std::unordered_map<unsigned int, Node> nodes;
    std::unordered_map<unsigned int, unsigned int> starting_nodes;
};
