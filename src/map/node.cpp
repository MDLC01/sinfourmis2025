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

salle Node::as_salle() const {
    return {
		.type = type,
		.pheromone = pheromone,
		.degre = (int32_t)edges.size()
	};
}

void Node::add_ant(Ant *ant) {
    ants.insert(ant);
}

void Node::remove_ant(Ant *ant) {
    ants.erase(ant);
}

Edge *Node::get_edge(unsigned int edge_id) const {
    return edges[edge_id].get();
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
        	unsigned int added = std::min(this->regen, std::min((unsigned int)total_available, this->max_food - this->food));
			this->food += added;
			total_available -= added;
		} else {
			this->food += std::min(this->regen, this->max_food - this->food);
		}
    }
}

void to_json(json &j, const Node &node) {
	j = json{
		{"id", node.id},
		{"type", node.type},
		{"x", node.x},
		{"y", node.y},
		{"pheromone", node.pheromone},
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
