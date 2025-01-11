#include <bits/stdc++.h>
#include <cstdint>
#include <map>
#include "sinfourmis.h"

using namespace std;

#define MAX_NODES 1000
#define BASE_NODE 0

typedef int choice;
typedef int node;
typedef vector<choice> path;

typedef struct {
    int node;
    path path;
} path_to_node_map;

#define min(a,b) (a)<(b)?(a):(b)

enum queen_turn_t {RECUP_TURN, RELEASE_TURN, BIRTH_TURN};
queen_turn_t turn_type = RECUP_TURN;
long turn_count = 0;




void read_memory (fourmi_etat) {} 

void set_memory(); //TODO

reine_retour queen_main(fourmi_etat fourmis[], const unsigned int nb_fourmis, const reine_etat *etat, const salle *salle) {
  for (unsigned int i = 0; i<nb_fourmis; i++) {
    read_memory(fourmis[i]);
  }
  if (turn_count == 0) {
    turn_count +=1;
    return {.action = CREER_FOURMI, .arg = (int32_t)etat->nourriture/4};
  }
  else if (turn_count == 1) {
    turn_count +=1;
    for (unsigned int i=0; i<nb_fourmis; i++) {
      set_memory();
    }
  }
  else switch (turn_type) {
    case RECUP_TURN: //TODO
      turn_type = BIRTH_TURN;
      return {.action = RECUPERER_FOURMI, .arg=0};


    case BIRTH_TURN : //TODO
      turn_type = RELEASE_TURN;
      return {.action = CREER_FOURMI, .arg=0};

    case RELEASE_TURN: //TODO
      for (unsigned int i=0; i<nb_fourmis; i++) {
        set_memory();
      }
      return {.action = ENVOYER_FOURMI, .arg=0};
      turn_type = RECUP_TURN;
}

static map<path, node> known_paths;
// `adj[u][v]` indicates what choice to make to go from `u` to `v`.
static int adj[MAX_NODES][MAX_NODES] = {0};
static node node_count = 1;

void unify_nodes(node m, node n) {
    // Mark neighbours of `n` as neighbours of `m`.
    for (int i = 0; i < node_count; i++) {
        if (adj[m][i] < 0) {
            adj[m][i] = adj[n][i];
        }
        if (adj[i][m] < 0) {
            adj[i][m] = adj[i][n];
        }
    }
    // Swap `n` and the last node, effectively removing `n`.
    node k = --node_count;
    for (int i = 0; i < node_count; i++) {
        adj[n][i] = adj[k][i];
        adj[i][n] = adj[i][k];
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
                // TODO: Instead of just `+1`, remember the cost of this edge.
                float alt = dist[u] + 1;
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
void add_identity(path forward_path, path backward_path) {
    vector<node> news = {};
    node previous = BASE_NODE;
    for (int i = 1; i < forward_path.size(); i++) {
        path prefix(forward_path.begin(), forward_path.begin() + i);

        auto result = known_paths.find(prefix);
        bool is_base = i == forward_path.size() - 1;

        if (is_base) {
            adj[previous][BASE_NODE] = prefix.back();
            adj[BASE_NODE][previous] = backward_path[backward_path.size() - i - 1];
        } else if (result == known_paths.end()) {
            node n = node_count++;
            adj[previous][n] = prefix.back();
            adj[n][previous] = backward_path[backward_path.size() - i - 1];
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
