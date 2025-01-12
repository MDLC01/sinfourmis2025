#pragma once

#include "memory.cpp"

#include <bits/stdc++.h>
using namespace std;

#define MAX_NODES 256
#define UNKNOWN_NODE 0b00000000
#define BASE_NODE 0b01000000

typedef uint8_t choice;
typedef uint8_t node;
typedef vector<choice> path;

#define min(a, b) (a) < (b) ? (a) : (b)
#define max(a, b) (a) > (b) ? (a) : (b)

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


void write_type(rw *rw, type_fourmi type) {
    write_number(rw, TYPE_SIZE, (unsigned long long) type);
}

static long turn_count = 0;
