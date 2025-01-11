#include "map/map.hpp"
#include <fstream>
#include <iostream>

Map::Map(Map &&other)
    : teams(std::move(other.teams)), nodes(std::move(other.nodes)),
      starting_nodes(std::move(other.starting_nodes)) {}

Map &Map::operator=(Map &&other) {
    if (this != &other) {
        nodes = std::move(other.nodes);
        teams = std::move(other.teams);
        starting_nodes = std::move(other.starting_nodes);
    }
    return *this;
}

bool Map::load_teams(const json &data) {
    auto json_teams = data.find("teams");
    if (json_teams == data.end()) {
        std::cerr << "Failed to find teams in JSON" << std::endl;
        return false;
    }
    for (const auto &team : *json_teams) {
        try {
            teams.emplace_back(team.get<Team>());
        } catch (json::exception &e) {
            std::cerr << "Failed to parse team: " << e.what() << std::endl;
            return false;
        }
    }
    return true;
}

bool Map::load_nodes(const json &data) {
    auto json_nodes = data.find("nodes");
    if (json_nodes == data.end()) {
        std::cerr << "Failed to find nodes in JSON" << std::endl;
        return false;
    }
    std::vector<std::pair<unsigned int, NeighborData>> edges;
    std::unordered_set<std::pair<unsigned int, unsigned int>> edges_set;
    for (const auto &node : *json_nodes) {
        try {
            auto node_obj = node.get<Node>();
            auto res = nodes.try_emplace(node_obj.get_id(), node_obj);
            if (!res.second) {
                std::cerr << "Duplicate node: " << node_obj.get_id() << std::endl;
                return false;
            }
            auto neighbors_it = node.find("neighbors");
            if (neighbors_it == node.end()) {
                continue;
            }
            auto neighbors = neighbors_it->get<std::vector<NeighborData>>();
            for (const auto &neighbor : neighbors) {
                if (neighbor.life <= 0 || neighbor.life > 1) {
                    std::cerr << "Invalid life value: " << neighbor.life << " in node "
                              << node_obj.get_id() << std::endl;
                    return false;
                }
                edges.push_back({node_obj.get_id(), neighbor});
                bool inserted = true;
                if (node_obj.get_id() < neighbor.to) {
                    inserted = edges_set.emplace(node_obj.get_id(), neighbor.to).second;
                } else {
                    inserted = edges_set.emplace(neighbor.to, node_obj.get_id()).second;
                }
                if (!inserted) {
                    std::cerr << "Duplicate edge: " << node_obj.get_id() << " " << neighbor.to
                              << std::endl;
                    return false;
                }
            }

            if (node_obj.get_type() == salle_type::REINE) {
                auto team_it = node.find("team");
                if (team_it != node.end()) {
                    auto team = team_it->get<unsigned int>();
                    if (std::find_if(teams.begin(), teams.end(), [team](const Team &t) {
                            return t.get_id() == team;
                        }) == teams.end()) {
                        std::cerr << "Team not found: " << team << std::endl;
                        return false;
                    }
                    auto res = starting_nodes.try_emplace(team, node_obj.get_id());
                    if (!res.second) {
                        std::cerr << "Duplicate starting node for team: " << team << std::endl;
                        return false;
                    }
                }
            }
        } catch (json::exception &e) {
            std::cerr << "Failed to parse node: " << e.what() << std::endl;
            return false;
        }
    }

    if (teams.size() != starting_nodes.size()) {
        std::cerr << "Not enough starting nodes for teams, got " << starting_nodes.size()
				  << " expected " << teams.size() << std::endl;
        return false;
    }

    for (auto &[node1, neighbor] : edges) {
        auto node1_it = nodes.find(node1);
        if (node1_it == nodes.end()) {
            std::cerr << "Node not found: " << node1 << std::endl;
            return false;
        }
        auto node2_it = nodes.find(neighbor.to);
        if (node2_it == nodes.end()) {
            std::cerr << "Node not found: " << neighbor.to << std::endl;
            return false;
        }
        node1_it->second.add_edge(node2_it->second, neighbor.life * EDGE_LIFE);
    }
    return true;
}

bool Map::load(const std::string_view &filename) {
    std::ifstream file(filename.data());
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    if (file.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "File is empty: " << filename << std::endl;
        return false;
    }
    json data;
    try {
        data = json::parse(file);
    } catch (json::parse_error &e) {
        std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
        return false;
    }

	if (!data.contains("base_food")) {
		std::cerr << "Missing base_food field" << std::endl;
	}

	auto base_food = data["base_food"].get<unsigned int>();

    if (!load_teams(data)) {
        return false;
    }
    if (teams.empty()) {
        std::cerr << "No teams found" << std::endl;
        return false;
    }
	for (auto &team: teams) {
		team.add_food(base_food);
	}

    if (!load_nodes(data)) {
        return false;
    }

    return true;
}

void Map::to_dot(const std::string_view &filename) const {
    std::ofstream file(filename.data());
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    file << "graph {" << std::endl;
    for (const auto &node : nodes) {
        file << "  " << node.first << std::endl;
        for (const auto &edge : node.second.get_edges()) {
            if (edge->get_node1()->get_id() == node.first) {
                file << "  " << node.first << " -- " << edge->get_node2()->get_id() << std::endl;
            }
        }
    }
    file << "}" << std::endl;
    file.close();
}

void Map::regen_food() {
    for (auto &node : nodes) {
        node.second.regen_food();
    }
}

Node *Map::get_starting_node(unsigned int team_id) {
    auto it = starting_nodes.find(team_id);
    if (it == starting_nodes.end()) {
        throw std::runtime_error("Team not found in starting nodes");
    }
    return &nodes[it->second];
}

Edge *Map::get_edge(unsigned int id1, unsigned int id2) const {
    auto node1 = nodes.find(id1);
    if (node1 == nodes.end()) {
        return nullptr;
    }
    return node1->second.get_edge_to_id(id2);
}
