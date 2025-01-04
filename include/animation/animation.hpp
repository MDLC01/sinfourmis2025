#pragma once

#include "animation/ant_group_data.hpp"
#include "game/ant.hpp"
#include "game/constants.hpp"
#include "map/edge.hpp"
#include "map/map.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

class Animation {
  public:
    Animation(const Map *map, const std::filesystem::path &path) : map(map), path(path) {}

    void start_frame();
    void end_frame();


    unsigned int game_turn() const {
        return turn;
    }

  private:
    /**
	 * @brief Write the groups of ants that are leaving a node to the json data,
	 *        it adds the animation from start of the edge to the next crossing step
	 *
	 * @param node the node to write the groups from
	 */
    void write_edges_departure_groups(const Node &node);

    /**
     * @brief Build the json data for the groups of ants on a node
     *
     * @param node the node to build the groups from
     * @return json the groups of ants on the node as json
     */
    json node_groups(const Node &node);

    /**
     * @brief Check the difference between the groups of ants on a node and the previous frame
     *        if there is a difference, add the new group to the json data
     *        if the group was not present in the previous frame, it add it in the previous frame
     * with 0 ants
     *
     * @param node the node to build the groups from
     * @param groups the groups of ants on the node as json
     */
    json write_groups_animation(const Node &node, json &groups);

    unsigned int turn = 0;
    bool started = false;
    json data;

    const Map *map;
    std::filesystem::path path;

    std::ofstream file;
};