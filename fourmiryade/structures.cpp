#include <bits/stdc++.h>
using namespace std;

#define MAX_NODES 1000
#define BASE_NODE 0

typedef int choice;
typedef int node;
typedef vector<choice> path;

typedef struct {
    bool water;
    bool food;
    choice prev;
    // The cost of the previous edge.
    int cost;
    choice next;
} path_node_info;
