#include "animation/animation.hpp"

Animation::Animation(const Map *map, const std::filesystem::path &path, unsigned int seed)
    : map(map), path(path) {
    data["data"] = json::object();
    data["seed"] = seed;
}

void to_json(json &j, const Edge &edge) {
    j = json{
        {"id_1", edge.get_node1()->get_id()},
        {"id_2", edge.get_node2()->get_id()},
        {"life_ratio", (float)edge.get_life() / (float)EDGE_LIFE},
    };
    auto groups = edge_groups(edge);
    for (auto [key, qt] : groups) {
        float progress = std::get<2>(key);
        float progress_anim = progress + EDGE_CROSS_SPEED;
        progress /= edge.get_length();
        progress_anim /= edge.get_length();
        if (std::get<0>(key) == edge.get_node2()->get_id()) {
            progress = 1.f - progress;
            progress_anim = 1.f - progress_anim;
        }
        progress = std::max(0.f, std::min(1.f, progress));
        progress_anim = std::max(0.f, std::min(1.f, progress_anim));
        if (progress_anim == progress && (progress == 0.f || progress == 1.f)) {
            continue;
        }
        j["groups"].push_back(AntGroupData{std::get<1>(key), qt, progress, progress_anim});
    }
}

void Animation::write_edges_departure_groups(const Node &node, const Edge *edge,
                                             json &json_edge) const {
    std::map<unsigned int, unsigned int> departures;
    for (const auto &ant_group : node.get_ants()) {
        for (auto ant : ant_group.second) {
            if (ant->get_action_state() == AntActionState::MOVING && ant->get_progress() == 0 &&
                ant->get_current_edge() == edge) {
                auto it = departures.find(ant->get_team_id());
                if (it == departures.end()) {
                    departures[ant->get_team_id()] = 1;
                } else {
                    departures[ant->get_team_id()]++;
                }
            }
        }
    }
    if (!departures.empty()) {
        if (!json_edge.contains("groups")) {
            json_edge["groups"] = json::array();
        }
        for (auto [team, qt] : departures) {
            float length = EDGE_CROSS_SPEED / edge->get_length();
            float start = 0;
            if (edge->get_node2()->get_id() == node.get_id()) {
                length = 1.f - EDGE_CROSS_SPEED / edge->get_length();
                start = 1;
            }
            json_edge["groups"].push_back(AntGroupData{team, qt, start, length});
        }
    }
}

json Animation::node_groups_json(const Node &node) const {
    auto groups = node_groups(node);
    auto res = json::array();
    for (auto [team, qt] : groups) {
        res.push_back(AntGroupData{team, qt});
    }
    return res;
}

json Animation::write_groups_animation(const Node &node, json &groups) const {
    auto groups_map = node_groups(node);
    auto res = json::array();
    for (auto group : groups) {
        auto it = groups_map.find(group["team"]);
        if (it == groups_map.end()) {
            res.push_back(AntGroupData{group["team"], 0});
        } else {
            res.push_back(AntGroupData{group["team"], it->second});
            groups_map.erase(it);
        }
    }
    for (auto [team, qt] : groups_map) {
        groups.push_back(AntGroupData{team, 0});
        res.push_back(AntGroupData{team, qt});
    }
    return res;
}

void Animation::start_frame() {
    turn++;
    if (started) {
        throw std::runtime_error("Start / end frame must be called in pairs");
    }
    started = true;
    frame = json::object();

    // save teams
    auto teams = json::array();
    for (auto team : map->get_teams()) {
        teams.push_back(team);
        teams.back()["score"] = team.get_score();
    }
    frame["teams"] = teams;

    // save nodes and edges
    auto nodes = json::array();
    auto edges = json::array();
    unsigned int max_food = 0;

    for (auto [_, node] : map->get_nodes()) {
        if (node.get_type() == salle_type::NOURRITURE) {
            max_food = std::max(max_food, node.get_max_food());
        }
        nodes.push_back(node);
        nodes.back()["ants"] = node_groups_json(node);
        for (auto edge : node.get_edges()) {
            if (edge->get_node1()->get_id() == node.get_id()) {
                edges.push_back(*edge);
            }
        }
    }

    for (auto [_, node] : map->get_nodes()) {
        for (auto edge : node.get_edges()) {
            int i = 0;
            while (
                edge->get_node1()->get_id() != edges[i].at("id_1").template get<unsigned int>() ||
                edge->get_node2()->get_id() != edges[i].at("id_2").template get<unsigned int>()) {
                i++;
            }
            write_edges_departure_groups(node, edge.get(), edges[i]);
        }
    }

    frame["max_food"] = max_food;
    frame["nodes"] = nodes;
    frame["edges"] = edges;
}

void Animation::end_frame(const std::vector<std::unique_ptr<Queen>> &queens) {
    if (!started) {
        throw std::runtime_error("Start / end frame must be called in pairs");
    }
    started = false;

    // teams animation
    auto teams = map->get_teams();
    for (unsigned int i = 0; i < teams.size(); i++) {
        frame["teams"][i]["next"] = json::object();
        if (frame["teams"][i]["score"] != teams[i].get_score()) {
            frame["teams"][i]["next"]["score"] = teams[i].get_score();
        }
        const auto &queen = std::ranges::find_if(queens, [&teams, i](const auto &queen) {
            return queen->get_team_id() == teams[i].get_id();
        });
        assert(queen != queens.end());
        if ((*queen)->is_upgrading()) {
            frame["teams"][i]["upgrading"] = (*queen)->current_upgrade();
        }
    }

    // nodes animation
    for (auto &node_json : frame["nodes"]) {
        node_json["anim"] = json::object();
        auto node = map->get_node(node_json["id"]);
        if (node.get_type() == salle_type::NOURRITURE && node_json["food"] != node.get_food()) {
            node_json["anim"]["food"] = node.get_food();
        }
        node_json["anim"]["ants"] = write_groups_animation(node, node_json["ants"]);
        if (node_json["anim"]["ants"].size() == 0) {
            node_json["anim"].erase("ants");
        }
        if (node_json["anim"].size() == 0) {
            node_json.erase("anim");
        }
    }
    data["data"][std::to_string(turn)] = frame;
}

void Animation::flush(bool formatted) {
    file.open(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << path << std::endl;
        exit(1);
    }
    if (formatted) {
        file << data.dump(4);
    } else {
        file << data;
    }
    file.close();

    std::cout << "Wrote output to " << path << std::endl;
}
