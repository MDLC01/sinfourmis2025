#include "map/node.hpp"
#include <iostream>

void Node::add_edge(Node &other, unsigned int life) {
    auto edge = std::make_shared<Edge>(this, &other, life);
    edges.push_back(edge);
    other.edges.push_back(edge);
}

void Node::remove_edge(const std::shared_ptr<Edge> &edge) {
    std::erase(edges, edge);
}

salle Node::as_salle(unsigned int team_id) const {
    size_t count = 0;
    for (const auto &team_ants : ants) {
        if (team_ants.second.size() > 0) {
            count++;
        }
    }
    fourmis_compteur *compteurs_fourmis = new fourmis_compteur[count];
    count = 0;
    for (const auto &team_ants : ants) {
        if (team_ants.second.size() > 0) {
            compteurs_fourmis[count].equipe = team_ants.first;
            compteurs_fourmis[count].nombre = team_ants.second.size();
            count++;
        }
    }

	uint8_t pheromone = 0;
	auto it = pheromones.find(team_id);
	if (it != pheromones.end()) {
		pheromone = it->second;
	}

    return {
        .type = type,
        .pheromone = pheromone,
        .degre = (int32_t)edges.size(),
        .nourriture = food,
        .taille_liste = count,
        .compteurs_fourmis = compteurs_fourmis,
    };
}

void Node::add_ant(Ant *ant) {
    ants[ant->get_team_id()].push_back(ant);
}

void Node::remove_ant(Ant *ant) {
    std::erase(ants.at(ant->get_team_id()), ant);
}

Edge *Node::get_edge(unsigned int edge_id) const {
    return edges.at(edge_id).get();
}

Edge *Node::get_edge_to_id(unsigned int id) const {
    for (const auto &edge : edges) {
        if (edge->get_other_node(this)->id == id) {
            return edge.get();
        }
    }
    return nullptr;
}

unsigned int Node::get_id_to(const Node *node) const {
    for (unsigned int i = 0; i < edges.size(); i++) {
        if (edges[i]->get_other_node(this) == node) {
            return i;
        }
    }
    return -1;
}

unsigned int Node::gather_food(unsigned int max_food_storage) {
    assert(this->type == salle_type::NOURRITURE);
    unsigned int gathered_food = std::min(max_food_storage, this->food);
    this->food -= gathered_food;
    return gathered_food;
}

void Node::regen_food() {
    if (this->type == salle_type::NOURRITURE) {
        if (total_available != -1) {
            unsigned int added = std::min(
                this->regen, std::min((unsigned int)total_available, this->max_food - this->food));
            this->food += added;
            total_available -= added;
        } else {
            this->food += std::min(this->regen, this->max_food - this->food);
        }
    }
}

void to_json(json &j, const Node &node) {
    j = json{
        {"id", node.id}, {"type", node.type},           {"x", node.x},
        {"y", node.y},   {"pheromones", node.pheromones},
    };
    if (node.type == salle_type::NOURRITURE) {
        j["food"] = node.food;
    }
    if (node.type == salle_type::REINE) {
        j["team"] = node.team;
    }
}

void from_json(const json &j, Node &node) {
    j.at("id").get_to(node.id);
    j.at("type").get_to(node.type);
    j.at("x").get_to(node.x);
    j.at("y").get_to(node.y);
    if (node.type == salle_type::NOURRITURE) {
        j.at("initial_food").get_to(node.food);
        j.at("regen").get_to(node.regen);
        j.at("max_food").get_to(node.max_food);
        j.at("total_available").get_to(node.total_available);
    }
    if (node.type == salle_type::REINE) {
        j.at("team").get_to(node.team);
    }
}

std::ostream &operator<<(std::ostream &os, const salle_type &type) {
    switch (type) {
        case salle_type::VIDE:
            os << "VIDE";
            break;
        case salle_type::NOURRITURE:
            os << "NOURRITURE";
            break;
        case salle_type::REINE:
            os << "REINE";
            break;
        case salle_type::EAU:
            os << "EAU";
            break;
    }

    return os;
}
