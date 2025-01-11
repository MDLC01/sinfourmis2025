#pragma once

#include "memory.cpp"

#include <bits/stdc++.h>
using namespace std;

#define MAX_NODES 1000
#define BASE_NODE 0

typedef int choice;
typedef int node;
typedef vector<choice> path;

typedef enum {
    EXPLORER,
    CARTOGRAPHER,
    WORKER,
    VARIANT_COUNT,
} type_fourmi;

#define TYPE_SIZE ceil_log2(VARIANT_COUNT)

type_fourmi read_type(rw *rw) {
    return (type_fourmi) read_number(rw, TYPE_SIZE);
}