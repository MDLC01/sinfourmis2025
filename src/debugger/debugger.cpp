#include "debugger/debugger.hpp"

void Debugger::print_node(Debugger::DisplayMode mode, const Node &node) {
    std::cout << "Node " << node.get_id() << " (" << node.get_x() << ", " << node.get_y() << ")\n";
    std::cout << "Ants:\n";
    auto groups = node_groups(node);
    for (const auto &[team_id, count] : groups) {
        std::cout << "\tTeam " << team_id << ": " << count << "\n";
    }
    std::cout << "Type: " << node.get_type() << "\n";
    if (node.get_type() == salle_type::NOURRITURE) {
        std::cout << "Food: " << node.get_food() << "\n";
        std::cout << "Remaining: " << node.get_remaining() << "\n";
    }
    if (node.get_type() == salle_type::REINE) {
        std::cout << "Queen: " << node.get_team() << "\n";
    }
    std::cout << "Pheromones: ";
	for (const auto &[team_id, pheromone] : node.get_pheromones()) {
		std::cout << "\t" << team_id << ": ";
		switch (mode) {
			case DisplayMode::HEX:
				std::cout << "0x" << std::hex << pheromone << "\n";
				break;
			case DisplayMode::BIN:
				std::cout << "0b" << std::bitset<8>(pheromone) << "\n";
				break;
			case DisplayMode::DEC:
				std::cout << (int)pheromone << "\n";
				break;
		}
    	std::cout << std::dec << std::endl;
	}
	std::cout << "\tpublic:";
    switch (mode) {
        case DisplayMode::HEX:
            std::cout << "0x" << std::hex << node.get_public_pheromone() << "\n";
            break;
        case DisplayMode::BIN:
            std::cout << "0b" << std::bitset<8>(node.get_public_pheromone()) << "\n";
            break;
        case DisplayMode::DEC:
            std::cout << (int)node.get_public_pheromone() << "\n";
            break;
    }
    std::cout << std::dec << std::endl;
}

void Debugger::print_edge(const Edge &edge) {
    std::cout << "Edge " << edge.get_node1()->get_id() << " -- " << edge.get_node2()->get_id()
              << "\n";
    std::cout << "Ants:\n";
    auto groups = edge_groups(edge);
    for (const auto &[key, count] : groups) {
        const auto &[start, team, progress] = key;
        std::cout << "- Group of " << team << ": ";
        std::cout << count << "ants (" << progress << "/" << edge.get_length() << ") "
                  << "from node: " << start << "\n";
    }
    std::cout << std::endl;
}

void Debugger::print_ant(Debugger::DisplayMode mode, const Ant &ant) {
    std::cout << "======= Ant (" << ant.get_id() << ") =======\n";
    std::cout << "Team: " << ant.get_team_id() << "\n";
    std::cout << "Status: ";
    switch (ant.get_action_state()) {
        case AntActionState::NONE:
            std::cout << "Normal\n";
            break;
        case AntActionState::MOVING:
            std::cout << "Moving along an edge\n";
            break;
        case AntActionState::DIGGING:
            std::cout << "Digging\n";
            break;
    }
    if (ant.get_current_node() != nullptr) {
        std::cout << "Current node: " << ant.get_current_node()->get_id() << "\n";
    }
    if (ant.get_current_edge() != nullptr) {
        std::cout << "Current edge: " << ant.get_current_edge()->get_node1()->get_id() << " -- "
                  << ant.get_current_edge()->get_node2()->get_id() << "\n";
        std::cout << "Progress: " << ant.get_progress() / ant.get_current_edge()->get_length()
                  << "\n";
    }
    std::cout << "== Stats: \n";
    std::cout << "Health: " << (int)ant.as_fourmi_etat().vie << "/"
              << ant.get_queen()->get_stat(Queen::Stat::LIFE) << "\n";
    std::cout << "Food: " << ant.as_fourmi_etat().nourriture << "/" << ant.get_max_food() << "\n";
    std::cout << "Water: " << ant.as_fourmi_etat().eau << "/" << ant.get_max_water() << "\n";
    std::cout << "Attack: " << ant.get_attack() << "\n";
    std::cout << "== Memory: \n";
    using enum Debugger::DisplayMode;
    for (unsigned int i = 0;
         i < sizeof(ant.as_fourmi_etat().memoire) / sizeof(ant.as_fourmi_etat().memoire[0]); i++) {
        switch (mode) {
            case DEC:
                std::cout << std::setfill(' ') << std::setw(4)
                          << (int)ant.as_fourmi_etat().memoire[i];
                break;
            case HEX:
                std::cout << std::hex << (uint8_t)ant.as_fourmi_etat().memoire[i];
                break;
            case BIN:
                std::cout << std::bitset<8>(ant.as_fourmi_etat().memoire[i]);
                break;
        }
        std::cout << std::dec << ' ';
        if ((i + 1) % 16 == 0) {
            std::cout << "\n";
        }
    }
    std::cout << std::endl;
}

void Debugger::debug(unsigned int turn, const Map &map,
                     const std::vector<std::unique_ptr<Ant>> &ants,
                     const std::vector<std::unique_ptr<Queen>> &queens) {
    if (!m_debug) {
        return;
    }
    if (skip > 0) {
        skip--;
        return;
    }
    std::string command;
    while (m_debug && skip == 0 && !m_exit) {
        std::cout << turn << "> ";
        std::getline(std::cin, command);
        DebugCommand cmd = DebugCommand::exit();
        if (!std::cin.eof()) {
            cmd = parse_command(command);
        } else {
            std::cout << std::endl;
        }
        using enum DebugCommand::Type;
        switch (cmd.type) {
            case EXIT:
                m_exit = true;
                break;
            case HELP:
                display_help();
                break;
            case STEP:
                skip = cmd.get_int_arg() + 1;
                break;
            case DISPLAY:
                {
                    auto [type, id1, id2] = cmd.get_tuple_arg();
                    display_element(map, ants, queens, type, id1, id2);
                }
                break;
            case LIST:
                list_elements(map, ants, queens, cmd.get_string_arg());
                break;
            case LISTIN:
                {
                    auto [type, id1, id2] = cmd.get_tuple_arg();
                    list_ants_in(map, ants, type, id1, id2);
                }
                break;
            case GOTO:
                if (cmd.get_int_arg() <= (int)turn) {
                    std::cerr << "Turn must be greater than current turn (" << turn << ")"
                              << std::endl;
                } else {
                    skip = cmd.get_int_arg() - turn;
                }
                break;
            case CONTINUE:
                m_debug = false;
                break;
            case HEX_MODE:
                display_mode = DisplayMode::HEX;
                break;
            case BIN_MODE:
                display_mode = DisplayMode::BIN;
                break;
            case DEC_MODE:
                display_mode = DisplayMode::DEC;
                break;
            case INVALID:
                break;
        }
    }
    // here skip is at least 1 so ok
    skip--;
}

Debugger::DebugCommand Debugger::parse_command(const std::string &command) const {
    using enum DebugCommand::Type;
    if (command == "exit") {
        return DebugCommand::exit();
    } else if (command == "help") {
        return DebugCommand::help();
    } else if (command == "step") {
        return DebugCommand::step(0);
    } else if (command == "continue") {
        return DebugCommand::continue_();
    } else if (command == "hexmode") {
        return DebugCommand::display_hex();
    } else if (command == "binmode") {
        return DebugCommand::display_bin();
    } else if (command == "decmode") {
        return DebugCommand::display_dec();
    } else {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        if (cmd == "step" || cmd == "goto") {
            int steps;
            auto cursor = iss.tellg();
            iss >> steps;
            if (iss.fail()) {
                print_error(command, iss.tellg(), "Invalid number of steps");
                return DebugCommand::invalid();
            }
            if (steps < 1) {
                print_error(command, cursor, "The argument must be greater than 0");
            }
            if (iss.tellg() != -1) {
                print_error(command, iss.tellg(), "Invalid command");
                return DebugCommand::invalid();
            }
            if (cmd == "step") {
                return DebugCommand::step(steps);
            } else {
                return DebugCommand::goto_step(steps);
            }
        } else if (cmd == "display" || cmd == "listin") {
            auto debug_command = parse_id_command(iss, command);
            if (debug_command.type != INVALID && cmd == "listin") {
                debug_command.type = LISTIN;
                auto type = std::get<0>(debug_command.get_tuple_arg());
                if (type != "node" && type != "edge") {
                    print_error(command, 0, "Invalid listin type, expected node or edge");
                    return DebugCommand::invalid();
                }
            }
            return debug_command;
        } else if (cmd == "list") {
            std::string type;
            auto cursor = iss.tellg();
            iss >> type;
            if (iss.fail()) {
                print_error(command, cursor,
                            "Invalid list type, expected nodes, edges, ants or queens");
                return DebugCommand::invalid();
            }
            if (iss.tellg() != -1) {
                print_error(command, 0, "Invalid command");
                return DebugCommand::invalid();
            }
            if (type != "nodes" && type != "edges" && type != "ants" && type != "queens") {
                print_error(command, cursor,
                            "Invalid list type, expected nodes, edges, ants or queens");
                return DebugCommand::invalid();
            }
            return DebugCommand::list(type);
        } else {
            print_error(command, 0, "Invalid command");
        }
    }
    return DebugCommand::invalid();
}

const Debugger::DebugCommand Debugger::parse_id_command(std::istringstream &iss,
                                                        const std::string &command) const {
    std::string type;
    iss >> type;
    if (iss.fail()) {
        print_error(command, iss.tellg(),
                    "Invalid display type, expected node, edge, ant or queen");
        return DebugCommand::invalid();
    }
    if (type == "global") {
        if (iss.tellg() != -1) {
            print_error(command, iss.tellg(), "Invalid command");
            return DebugCommand::invalid();
        }
        return DebugCommand::display("global", -1, -1);
    }
    if (type != "node" && type != "edge" && type != "ant" && type != "queen") {
        print_error(command, iss.tellg(),
                    "Invalid display type, expected node, edge, ant or queen");
        return DebugCommand::invalid();
    }
    int id = -1;
    iss >> id;
    if (iss.fail()) {
        print_error(command, iss.tellg(), "Invalid id");
        return DebugCommand::invalid();
    }
    int id2 = -1;
    if (type == "edge") {
        iss >> id2;
        if (iss.fail()) {
            print_error(command, iss.tellg(), "Invalid id");
            return DebugCommand::invalid();
        }
    }
    if (iss.tellg() != -1) {
        print_error(command, iss.tellg(), "Invalid command");
        return DebugCommand::invalid();
    }
    return DebugCommand::display(type, id, id2);
}
void Debugger::print_error(const std::string &command, int column,
                           const std::string &message) const {
    std::cerr << "Error: " << message << "\n";
    std::cerr << command << "\n";
    if (column >= 0) {
        for (int i = 0; i < column; i++) {
            std::cerr << " ";
        }
        std::cerr << "^";
        for (unsigned int i = 0; i < command.size() - column - 1; i++) {
            std::cerr << "~";
        }
    }
    std::cerr << std::endl;
}

void Debugger::display_element(const Map &map, const std::vector<std::unique_ptr<Ant>> &ants,
                               const std::vector<std::unique_ptr<Queen>> &queens,
                               const std::string &type, int id1, int id2) const {
    if (id1 < 0 && type != "global") {
        std::cerr << "Invalid id\n";
        return;
    }
    if (id2 < 0 && type == "edge") {
        std::cerr << "Invalid id\n";
        return;
    }
    auto uid1 = static_cast<unsigned int>(id1);
    auto uid2 = static_cast<unsigned int>(id2);

    if (type == "global") {
        std::cout << "== Global statistics ==\n";
        std::cout << "Nodes: " << map.get_node_count() << "\n";
        std::cout << "Teams: " << map.get_team_count() << "\n";
        std::cout << "Ants: " << ants.size() << "\n";
        for (const auto &team : map.get_teams()) {
            std::cout << "Team " << team.get_name() << " (" << team.get_id()
                      << ") score: " << team.get_score() << "\n";
        }
    } else if (type == "node") {
        const auto &nodes = map.get_nodes();
        const auto &node = nodes.find(uid1);
        if (node == nodes.end()) {
            std::cerr << "Node not found\n";
            return;
        }
        print_node(display_mode, node->second);
    } else if (type == "edge") {
        auto edge = map.get_edge(uid1, uid2);
        if (edge == nullptr) {
            std::cerr << "Edge not found\n";
            return;
        }
        print_edge(*edge);
    } else if (type == "ant") {
        const auto &ant =
            std::ranges::find_if(ants, [uid1](const auto &ant) { return ant->get_id() == uid1; });
        if (ant == ants.end()) {
            std::cerr << "Ant not found\n";
            return;
        }
        print_ant(display_mode, **ant);
    } else if (type == "queen") {
        const auto &queen = std::ranges::find_if(
            queens, [uid1](const auto &queen) { return queen->get_team_id() == uid1; });
        if (queen == queens.end()) {
            std::cerr << "Queen not found\n";
            return;
        }
        std::cout << **queen << std::endl;
    }
    std::cout << std::endl;
}

void Debugger::list_elements(const Map &map, const std::vector<std::unique_ptr<Ant>> &ants,
                             const std::vector<std::unique_ptr<Queen>> &queens,
                             const std::string &type) const {
    if (type == "nodes") {
        std::cout << "== Nodes ==\n";
        for (const auto &[_, node] : map.get_nodes()) {
            std::cout << "Node :" << node.get_id() << " (" << node.get_type() << ")\n";
        }
    } else if (type == "edges") {
        std::cout << "== Edges ==\n";
        for (const auto &[_, node] : map.get_nodes()) {
            for (const auto &edge : node.get_edges()) {
                if (edge->get_node1()->get_id() == node.get_id()) {
                    std::cout << "Edge :" << edge->get_node1()->get_id() << " -- "
                              << edge->get_node2()->get_id() << "\n";
                }
            }
        }
    } else if (type == "ants") {
        std::cout << "== Ants ==\n";
        for (const auto &ant : ants) {
            std::cout << "Ant (" << ant->get_team_id() << ") :" << ant->get_id() << "\n";
        }
    } else if (type == "queens") {
        std::cout << "== Queens ==\n";
        for (const auto &queen : queens) {
            std::cout << "Queen :" << queen->get_team_id() << "\n";
        }
    } else {
        std::cerr << "Invalid type\n";
    }
    std::cout << std::endl;
}
void Debugger::list_ants_in(const Map &map, const std::vector<std::unique_ptr<Ant>> &ants,
                            const std::string &type, int id1, int id2) const {
    if (id1 < 0) {
        std::cerr << "Invalid id\n";
        return;
    }
    unsigned int uid1 = static_cast<unsigned int>(id1);
    if (id2 < 0 && type == "edge") {
        std::cerr << "Invalid id\n";
        return;
    }
    unsigned int uid2 = static_cast<unsigned int>(id2);
    if (type == "node") {
        const auto &nodes = map.get_nodes();
        const auto &nodeit = nodes.find(uid1);
        if (nodeit == nodes.end()) {
            std::cerr << "Node not found\n";
            return;
        }
        const auto &node = nodeit->second;
        std::cout << "== Ants in node " << node.get_id() << " ==\n";
        for (const auto &group : node.get_ants()) {
            for (auto ant : group.second) {
                std::cout << "Ant (" << ant->get_team_id() << ") :" << ant->get_id() << "\n";
            }
        }
    } else if (type == "edge") {
        auto edge = map.get_edge(uid1, uid2);
        if (edge == nullptr) {
            std::cerr << "Edge not found\n";
            return;
        }
        std::cout << "== Ants in edge " << edge->get_node1()->get_id() << " -- "
                  << edge->get_node2()->get_id() << " ==\n";
        for (const auto &ant : edge->get_ants()) {
            std::cout << "Ant :" << ant->get_id() << " " << ant->get_team_id() << "\n";
        }
    } else {
        std::cerr << "Invalid type\n";
    }
    std::cout << std::endl;
}

void Debugger::display_help() const {
    std::cout << "Help :\n";
    std::cout << "Navigation :\n";
    std::cout << "\tstep [n] : execute n steps, default is 1\n";
    std::cout << "\tgoto <n> : go to turn n\n";
    std::cout << "\tcontinue : exit the debugger (it does not stop the simulation)\n";
    std::cout << "\texit : exit the simulation (it stops the simulation completely)\n";
    std::cout << "Informations gathering :\n";
    std::cout << "\tdisplay node <id> : display node with id\n";
    std::cout << "\tdisplay edge <id1> <id2> : display edge between nodes with id1 and id2\n";
    std::cout << "\tdisplay ant <id> : display ant with id\n";
    std::cout << "\tdisplay queen <id> : display queen with id\n";
    std::cout << "\tdisplay global : display global statistics\n";
    std::cout << "\tlist nodes : list all nodes\n";
    std::cout << "\tlist edges : list all edges\n";
    std::cout << "\tlist ants : list all ants\n";
    std::cout << "\tlist queens : list all queens\n";
    std::cout << "\tlistin node <id> : list all ants in node with id\n";
    std::cout << "\tlistin edge <id1> <id2> : list all ants in the edge between the two nodes\n";
    std::cout << "Display modes\n";
    std::cout << "\tbinmode : display data in binary\n";
    std::cout << "\thexmode : display data in hexadecimal\n";
    std::cout << "\tdecmode : display data in decimal\n";
    std::cout << "Useful commands :\n";
    std::cout << "\thelp : display this help\n";
    std::cout
        << " - listin edge <id1> <id2> : list all ants in edge between nodes with id1 and id2\n";
    std::cout << std::endl;
}
