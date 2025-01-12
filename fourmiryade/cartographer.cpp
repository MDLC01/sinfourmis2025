#pragma once

#include "sinfourmis.h"
#include "utils.cpp"
#include "memory.cpp"

#include <stdlib.h>
#include <map>
using namespace std;


typedef struct {
    bool water;
    bool food;
    int food_timestamp;
    bool has_fighters;
} cartographer_node_info;


static map<path, node> known_paths;
// `adj[u][v]` indicates what choice to make to go from `u` to `v`.
static int adj[MAX_NODES][MAX_NODES] = {0};
static int costs[MAX_NODES][MAX_NODES] = {0};
static cartographer_node_info node_infos[MAX_NODES];
static bool is_known[MAX_NODES] = {false};

cartographer_node_info unify_node_info(cartographer_node_info lhs, cartographer_node_info rhs) {
    cartographer_node_info result;
    result.water = lhs.water || rhs.water;
    if (lhs.food_timestamp < rhs.food_timestamp) {
        result.food = rhs.food;
        result.food_timestamp = rhs.food_timestamp;
    } else {
        result.food = lhs.food;
        result.food_timestamp = lhs.food_timestamp;
    }
    result.has_fighters = lhs.has_fighters || rhs.has_fighters;
    return result;
}

// void unify_nodes(node m, node n) {
//     if (n == BASE_NODE) {
//         return unify_nodes(n, m);
//     }
//     // Mark neighbours of `n` as neighbours of `m`.
//     for (int i = 0; i < node_count; i++) {
//         if (adj[m][i] < 0) {
//             adj[m][i] = adj[n][i];
//             costs[m][i] = costs[n][i];
//         }
//         if (adj[i][m] < 0) {
//             adj[i][m] = adj[i][n];
//             costs[i][m] = costs[i][n];
//         }
//     }
//     // Combine node informations.
//     node_infos[m] = unify_node_info(node_infos[m], node_infos[n]);
//     // Swap `n` and the last node, effectively removing `n`.
//     node k = --node_count;
//     for (int i = 0; i < node_count; i++) {
//         adj[n][i] = adj[k][i];
//         adj[i][n] = adj[i][k];
//         costs[n][i] = costs[k][i];
//         costs[i][n] = costs[i][k];
//     }
//     // `n <- m` and `k <- n` in `known_paths` as well.
//     for (auto& [_, v] : known_paths) {
//         if (v == n) {
//             v = m;
//         } else if (v == k) {
//             v = n;
//         }
//     }
//     // `k <- n` in node informations.
//     node_infos[n] = node_infos[k];
// }

// Computes the shortest path between two nodes (given the information we have).
path shortest_path(node source, node destination) {
    vector<float> dist(MAX_NODES, 1.0 / 0.0);
    vector<node> prev(MAX_NODES, -1);
    vector<node> queue;
    for (int n = 1; n <= 255; n++) {
        if (is_known[n]) {
            queue.push_back(n);
        }
    }
    dist[source] = 0.0;

    while (!queue.empty()) {
        int index = -1;
        node u = -1;
        float min_distance = 1.0 / 0.0;
        for (int i = 0; i < (int) queue.size(); i++) {
            if (dist[queue[i]] < min_distance) {
                index = i;
                u = queue[i];
                min_distance = dist[queue[i]];
            }
        }
        queue[index] = queue.back();
        queue.pop_back();
        for (int v = 1; v <= 255; v++) {
            if (adj[u][v] >= 0) {
                float alt = dist[u] + costs[u][v];
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                }
            }
        }
    }
    path path;
    for (node current = destination; current != source; current = prev[current]) {
        path.push_back(adj[prev[current]][current]);
    }
    reverse(path.begin(), path.end());
    return path;
}

// Tells the queen that this path starts and ends at the base.
void add_identity(
    path forward_path,
    path backward_path,
    vector<int> path_costs,
    vector<uint8_t> ids,
    vector<cartographer_node_info> infos
) {
    vector<node> news = {};
    node previous = BASE_NODE;
    for (int i = 1; i < (int) forward_path.size(); i++) {
        node n = ids[i - 1];
        if (n == UNKNOWN_NODE) {
            return;
        }
        is_known[n] = true;

        path prefix(forward_path.begin(), forward_path.begin() + i);

        adj[previous][n] = prefix.back();
        adj[n][previous] = backward_path.back();
        backward_path.pop_back();
        costs[previous][n] = path_costs[i];
        costs[n][previous] = path_costs[i];
        node_infos[n] = unify_node_info(node_infos[n], infos[i - 1]);
        known_paths.insert_or_assign(prefix, n);
        news.push_back(n);
        previous = n;
    }
}

#define CARTOGRAPHER_COUNT 4

typedef enum {
    CARTOGRAPHER_STARTS,
    CARTOGRAPHER_TRIED_MOVING,
    CARTOGRAPHER_DIGGED_1,
    CARTOGRAPHER_DIGGED_2,
    CARTOGRAPHER_DIGGED_3,
    CARTOGRAPHER_DIGGED_4,
    NUM_CARTOGRAPHER_STATES,
} cartographer_state;

#define CARTOGRAPHER_STATE_SIZE ceil_log2(NUM_CARTOGRAPHER_STATES)

#define CELL_ID_SIZE 8

#define PATH_NODE_INFO_ID_SIZE CELL_ID_SIZE
#define PATH_NODE_INFO_PREV_SIZE 5
#define PATH_NODE_INFO_COST_SIZE 4
#define PATH_NODE_INFO_NEXT_SIZE 5
#define PATH_NODE_INFO_SIZE (1 + 1 + PATH_NODE_INFO_PREV_SIZE + PATH_NODE_INFO_COST_SIZE + PATH_NODE_INFO_NEXT_SIZE)

#define PREVIOUS_WATER_SIZE 5
#define PATH_LENGTH_SIZE 8

void handle_cartographer_from_queen(fourmi_etat *ant) {
    rw rw = create_rw(ant->memoire);
    rw.offset += TYPE_SIZE;
    int previous_water = read_number(&rw, PREVIOUS_WATER_SIZE);
    align_rw(&rw);
    int path_length = read_number(&rw, PATH_LENGTH_SIZE);
    vector<choice> forward_path;
    vector<choice> backward_path;
    vector<uint8_t> ids;
    vector<int> path_costs;
    vector<cartographer_node_info> infos;
    for (int i = 0; i < path_length; i++) {
        choice next = read_number(&rw, PATH_NODE_INFO_NEXT_SIZE);
        bool food = read_bool(&rw);
        bool water = read_bool(&rw);
        cartographer_node_info info;
        info.water = water;
        info.food = food;
        info.food_timestamp = turn_count;
        info.has_fighters = false;
        infos.push_back(info);
        uint8_t id = read_number(&rw, PATH_NODE_INFO_ID_SIZE);
        choice prev = read_number(&rw, PATH_NODE_INFO_PREV_SIZE);
        int cost = read_number(&rw, PATH_NODE_INFO_COST_SIZE);
        ids.push_back(id);
        forward_path.push_back(next);
        backward_path.push_back(prev);
        path_costs.push_back(cost);
    }
    // There is technically a last `next` here. We simply ignore it, as this is
    // where the ant would have gone if it were not picked up by the queen.
    reverse(backward_path.begin(), backward_path.end());
    add_identity(forward_path, backward_path, path_costs, ids, infos);
}

void initialize_cartographer(fourmi_etat *ant, int cartographer_id) {
    assert(0 <= cartographer_id && cartographer_id < CARTOGRAPHER_COUNT);
    // Ideally, this should be done at init time. Oh well...
    is_known[BASE_NODE] = true;
    rw rw = create_rw(ant->memoire);
    write_type(&rw, CARTOGRAPHER);
    write_number(&rw, PREVIOUS_WATER_SIZE, 0); // Whatever.
    write_number(&rw, CARTOGRAPHER_STATE_SIZE, CARTOGRAPHER_STARTS);
    align_rw(&rw);
    write_number(&rw, CELL_ID_SIZE, (cartographer_id << 6) + 1);
    write_number(&rw, PATH_LENGTH_SIZE, 0);
}

char *mem_at_cartographer_path_start(fourmi_etat *etat) {
    rw rw = create_rw(etat->memoire);
    read_type(&rw);
    rw.offset += PREVIOUS_WATER_SIZE + CARTOGRAPHER_STATE_SIZE;
    align_rw(&rw);
    rw.offset += CELL_ID_SIZE;
    return rw.mem + (rw.offset / 8);
}

fourmi_retour cartographer_activation(fourmi_etat *etat, rw *rw, const salle *salle) {
    // First info is water amount.
    int previous_water = override_number(rw, PREVIOUS_WATER_SIZE, etat->eau);

    // State.
    auto rw_at_state = clone_rw(rw);
    cartographer_state state = (cartographer_state) read_number(rw, CARTOGRAPHER_STATE_SIZE);

    align_rw(rw);
    auto rw_at_next_cell_id = clone_rw(rw);
    uint8_t next_cell_id = read_number(rw, CELL_ID_SIZE);

    auto rw_at_path_length = clone_rw(rw);
    int path_length = read_and_increment(rw, PATH_LENGTH_SIZE);
    // Initial choice + rest of the path.
    int path_size = PATH_NODE_INFO_NEXT_SIZE + path_length * PATH_NODE_INFO_SIZE;

    pheromone_type depose_pheromone = NO_PHEROMONE;
    uint8_t pheromone = UNKNOWN_NODE;

    uint8_t current_id = 0;
    if (salle->pheromone == 0) {
        if (next_cell_id % CARTOGRAPHER_COUNT != 0) {
            pheromone = next_cell_id;
            depose_pheromone = PRIVE;
            current_id = pheromone;
            write_number(&rw_at_next_cell_id, CELL_ID_SIZE, next_cell_id + 1);
        }
    } else {
        current_id = salle->pheromone;
    }

    switch (state) {
        case CARTOGRAPHER_STARTS: {
            choice c = rand() % salle->degre;
            write_number(rw, PATH_NODE_INFO_NEXT_SIZE, c);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = PRIVE, .pheromone = BASE_NODE};
        }

        case CARTOGRAPHER_TRIED_MOVING: {
            rw->offset += path_size;
            if (etat->result < 0) {
                // Unable to move there.
                rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
                choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
                write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_1);
                return {.action = COMMENCE_CONSTRUCTION, .arg = c, .depose_pheromone = depose_pheromone, .pheromone = pheromone};
            }
            write_bool(rw, salle->nourriture > 0);
            write_bool(rw, salle->type == EAU);
            write_number(rw, PATH_NODE_INFO_ID_SIZE, current_id);
            write_number(rw, PATH_NODE_INFO_PREV_SIZE, etat->result);
            write_number(rw, PATH_NODE_INFO_COST_SIZE, previous_water - etat->eau);
            choice c = rand() % salle->degre;
            write_number(rw, PATH_NODE_INFO_NEXT_SIZE, c);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_TRIED_MOVING);
            write_number(&rw_at_path_length, PATH_LENGTH_SIZE, path_length + 1);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = depose_pheromone, .pheromone = pheromone};
        }

        case CARTOGRAPHER_DIGGED_1: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_2);
            return {.action = FOURMI_PASSE, .arg = c, .depose_pheromone = depose_pheromone, .pheromone = pheromone};
        }
        case CARTOGRAPHER_DIGGED_2: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_3);
            return {.action = FOURMI_PASSE, .arg = c, .depose_pheromone = depose_pheromone, .pheromone = pheromone};
        }
        case CARTOGRAPHER_DIGGED_3: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_4);
            return {.action = TERMINE_CONSTRUCTION, .arg = c, .depose_pheromone = depose_pheromone, .pheromone = pheromone};
        }
        case CARTOGRAPHER_DIGGED_4: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = depose_pheromone, .pheromone = pheromone};
        }

        case NUM_CARTOGRAPHER_STATES: {
            assert(false);
        }
    }

    assert(false);
}
