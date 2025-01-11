#pragma once

#include <assert.h>


// From https://stackoverflow.com/a/15327567.
int ceil_log2(unsigned long long x)
{
  static const unsigned long long t[6] = {
    0xFFFFFFFF00000000ull,
    0x00000000FFFF0000ull,
    0x000000000000FF00ull,
    0x00000000000000F0ull,
    0x000000000000000Cull,
    0x0000000000000002ull
  };

  int y = (((x & (x - 1)) == 0) ? 0 : 1);
  int j = 32;
  int i;

  for (i = 0; i < 6; i++) {
    int k = (((x & t[i]) == 0) ? 0 : j);
    y += k;
    x >>= k;
    j >>= 1;
  }

  return y;
}

char get_bit(char* mem, int x) {
    int i = x/8;
    int j = x%8;
    assert(x < 256);
    assert(x >=0);
    char mask = 1 << j;
    char oct = mem[i];
    char res = mask & oct;
    return (res != 0);
}

void set_bit(char*mem, int x, char bit) {
    int i = x/8;
    int j = x%8;
    assert(x < 256);
    assert(x >=0);
    char oct = mem[i];
    char mask = 1<<j;
    char mask2 = ~mask;
    oct = bit?oct|mask:oct&mask2;
    mem[i] = oct;
}


// get number form bit number min (included) from bit number max (excluded)
unsigned long long get_number(char* mem, int min, int max) {
    assert(min < max);
    assert(max-min <= 8*sizeof(unsigned long long));
    unsigned long long sum = 0;
    for (int x = min; x<max; x++){
        sum = sum<<1;
        sum += get_bit(mem, x);
    }
    return sum;
}
// set number form bit number min (included) from bit number max (excluded) to the value of number
void set_number (char* mem, int min, int max, unsigned long long number) {
    assert(min < max);
    assert(max-min <= 8*sizeof(unsigned long long));
    unsigned long long mask = 0b1;
    for (int x = max-1; x>=min; x--) {
        char bit = number & mask;
        set_bit(mem, x, bit);
        number = number >> 1;
    }
}


typedef struct {
    char *mem;
    int offset;
} rw;

rw create_rw(char *mem) {
    return (rw) { .mem = mem, .offset = 0 };
}

rw clone_rw(rw *rw) {
    return *rw;
}

unsigned long long read_number(rw *rw, int size) {
    unsigned long long value = get_number(rw->mem, rw->offset, rw->offset + size);
    rw->offset += size;
    return value;
}

bool read_bool(rw *rw) {
    return (bool) read_number(rw, 1);
}

void write_number(rw *rw, int size, unsigned long long value) {
    set_number(rw->mem, rw->offset, rw->offset + size, value);
    rw->offset += size;
}

void write_bool(rw *rw, bool value) {
    write_number(rw, 1, (unsigned long long) value);
}

// Reads a number and overrides it.
unsigned long long override_number(rw *rw, int size, unsigned long long value) {
    unsigned long long result = get_number(rw->mem, rw->offset, rw->offset + size);
    set_number(rw->mem, rw->offset, rw->offset + size, value);
    rw->offset += size;
    return result;
}
