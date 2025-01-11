#pragma once

#include "sinfourmis.h"
#include "structures.cpp"
#include "memory.cpp"
#include "structures.cpp"

#include <stdlib.h>
#include <map>
using namespace std;


static map<path, node> known_paths;
// `adj[u][v]` indicates what choice to make to go from `u` to `v`.
static int adj[MAX_NODES][MAX_NODES] = {0};
static int costs[MAX_NODES][MAX_NODES] = {0};
static node node_count = 1;

void unify_nodes(node m, node n) {
    // Mark neighbours of `n` as neighbours of `m`.
    for (int i = 0; i < node_count; i++) {
        if (adj[m][i] < 0) {
            adj[m][i] = adj[n][i];
            costs[m][i] = costs[n][i];
        }
        if (adj[i][m] < 0) {
            adj[i][m] = adj[i][n];
            costs[i][m] = costs[i][n];
        }
    }
    // Swap `n` and the last node, effectively removing `n`.
    node k = --node_count;
    for (int i = 0; i < node_count; i++) {
        adj[n][i] = adj[k][i];
        adj[i][n] = adj[i][k];
        costs[n][i] = costs[k][i];
        costs[i][n] = costs[i][k];
    }
    // `n <- m` and `k <- n` in `known_paths` as well.
    for (auto& [_, v] : known_paths) {
        if (v == n) {
            v = m;
        } else if (v == k) {
            v = n;
        }
    }
}

// Computes the shortest path between two nodes (given the information we have).
path shortest_path(node source, node destination) {
    float dist[node_count];
    node prev[node_count];
    vector<node> queue;
    for (node n = 0; n < node_count; n++) {
        dist[n] = 1.0 / 0.0;
        prev[n] = -1;
        queue.push_back(n);
    }
     dist[source] = 0.0;

    while (!queue.empty()) {
        int index = -1;
        node u = -1;
        float min_distance = 1.0 / 0.0;
        for (int i = 0; i < queue.size(); i++) {
            if (dist[queue[i]] < min_distance) {
                index = i;
                u = queue[i];
                min_distance = dist[queue[i]];
            }
        }
        queue[index] = queue.back();
        queue.pop_back();
        for (node v = 0; v < node_count; v++) {
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
void add_identity(path forward_path, path backward_path, vector<int> path_costs) {
    vector<node> news = {};
    node previous = BASE_NODE;
    for (int i = 1; i < forward_path.size(); i++) {
        path prefix(forward_path.begin(), forward_path.begin() + i);

        auto result = known_paths.find(prefix);
        bool is_base = i == forward_path.size() - 1;

        if (is_base) {
            adj[previous][BASE_NODE] = prefix.back();
            adj[BASE_NODE][previous] = backward_path[backward_path.size() - i - 1];
            costs[previous][BASE_NODE] = path_costs[i];
            costs[BASE_NODE][previous] = path_costs[i];
        } else if (result == known_paths.end()) {
            node n = node_count++;
            adj[previous][n] = prefix.back();
            adj[n][previous] = backward_path[backward_path.size() - i - 1];
            costs[previous][BASE_NODE] = path_costs[i];
            costs[BASE_NODE][previous] = path_costs[i];
            known_paths.insert_or_assign(prefix, n);
            news.push_back(n);
            previous = n;
        } else {
            previous = result->second;
        }
    }
    for (int i = 1; i < backward_path.size(); i++) {
        if (news.empty()) {
            break;
        }
        path prefix(backward_path.begin(), backward_path.begin() + i);
        auto n = known_paths.find(prefix);
        if (n == known_paths.end()) {
            break;
        }
        node m = news.back();
        news.pop_back();
        unify_nodes(m, n->second);
    }
}

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

#define PATH_NODE_INFO_PREV_SIZE 4
#define PATH_NODE_INFO_COST_SIZE 4
#define PATH_NODE_INFO_NEXT_SIZE 4
#define PATH_NODE_INFO_SIZE (1 + 1 + PATH_NODE_INFO_PREV_SIZE + PATH_NODE_INFO_COST_SIZE + PATH_NODE_INFO_NEXT_SIZE)

#define PREVIOUS_WATER_SIZE 5
// Max encodable path length: 127.
#define PATH_LENGTH_SIZE 7

void handle_cartographer_from_queen(fourmi_etat *ant) {
    rw rw = create_rw(ant->memoire);
    rw.offset += TYPE_SIZE;
    int previous_water = read_number(&rw, PREVIOUS_WATER_SIZE);
    int path_length = read_number(&rw, PATH_LENGTH_SIZE);
    vector<int> forward_path;
    vector<int> backward_path;
    vector<int> path_costs;
    for (int i = 0; i < path_length; i++) {
        choice next = read_number(&rw, PATH_NODE_INFO_NEXT_SIZE);
        bool food = read_bool(&rw);
        bool water = read_bool(&rw);
        choice prev = read_number(&rw, PATH_NODE_INFO_PREV_SIZE);
        int cost = read_number(&rw, PATH_NODE_INFO_COST_SIZE);
        forward_path.push_back(next);
        backward_path.push_back(prev);
        path_costs.push_back(cost);
    }
    choice next = read_number(&rw, PATH_NODE_INFO_NEXT_SIZE);
    // FIXME: If the ant did not get picked up immediately, then this is not the right result.
    choice prev = ant->result;
    int cost = previous_water - ant->eau;
    backward_path.push_back(prev);
    path_costs.push_back(cost);
    forward_path.push_back(next);
    reverse(backward_path.begin(), backward_path.end());
    add_identity(forward_path, backward_path, path_costs);
}

void initialize_cartographer(fourmi_etat *ant) {
    rw rw = create_rw(ant->memoire);
    write_type(&rw, CARTOGRAPHER);
    write_number(&rw, PREVIOUS_WATER_SIZE, 0); // Whatever.
    write_number(&rw, CARTOGRAPHER_STATE_SIZE, CARTOGRAPHER_STARTS);
    write_number(&rw, PATH_LENGTH_SIZE, 0);
}

fourmi_retour cartographer_activation(fourmi_etat *etat, rw *rw, const salle *salle) {
    // First info is water amount.
    int previous_water = override_number(rw, PREVIOUS_WATER_SIZE, etat->eau);

    // State.
    auto rw_at_state = clone_rw(rw);
    cartographer_state state = (cartographer_state) read_number(rw, CARTOGRAPHER_STATE_SIZE);

    auto rw_at_path_length = clone_rw(rw);
    int path_length = read_and_increment(rw, PATH_LENGTH_SIZE);
    // Initial choice + rest of the path.
    int path_size = PATH_NODE_INFO_NEXT_SIZE + path_length * PATH_NODE_INFO_SIZE;

    switch (state) {
        CARTOGRAPHER_STARTS: {
            choice c = rand() % salle->degre;
            write_number(rw, PATH_NODE_INFO_NEXT_SIZE, c);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        CARTOGRAPHER_TRIED_MOVING: {
            rw->offset += path_size;
            if (etat->result < 0) {
                // Unable to move there.
                rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
                choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
                write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_1);
                return {.action = COMMENCE_CONSTRUCTION, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
            }
            write_bool(rw, salle->nourriture > 0);
            write_bool(rw, salle->type == EAU);
            write_number(rw, PATH_NODE_INFO_PREV_SIZE, etat->result);
            write_number(rw, PATH_NODE_INFO_COST_SIZE, previous_water - etat->eau);
            choice c = rand() % salle->degre;
            write_number(rw, PATH_NODE_INFO_NEXT_SIZE, c);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        CARTOGRAPHER_DIGGED_1: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_2);
            return {.action = FOURMI_PASSE, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }
        CARTOGRAPHER_DIGGED_2: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_3);
            return {.action = FOURMI_PASSE, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }
        CARTOGRAPHER_DIGGED_3: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_DIGGED_4);
            return {.action = TERMINE_CONSTRUCTION, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }
        CARTOGRAPHER_DIGGED_4: {
            rw->offset -= PATH_NODE_INFO_NEXT_SIZE;
            choice c = read_number(rw, PATH_NODE_INFO_NEXT_SIZE);
            write_number(&rw_at_state, CARTOGRAPHER_STATE_SIZE, (unsigned long long) CARTOGRAPHER_TRIED_MOVING);
            return {.action = DEPLACEMENT, .arg = c, .depose_pheromone = NO_PHEROMONE, .pheromone = 0};
        }

        default: {
            assert(false);
        }
    }
}
