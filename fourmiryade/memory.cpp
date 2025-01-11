#include <assert.h>
#include "sinfourmis.h"


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

#define BYTE_SIZE 8

int read_bit(unsigned char n, int offset) {
    assert(0 <= offset && offset < BYTE_SIZE);
    return (n << offset) >> BYTE_SIZE;
}

unsigned char write_bit(unsigned char n, int offset, unsigned int value) {
    assert(0 <= offset && offset < BYTE_SIZE);
    assert(value == 0 || value == 1);
    unsigned char masks[8] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
        0b00001000,
        0b00000100,
        0b00000010,
        0b00000001,
    };
    n = n & ~masks[offset];
    return n | (value << (BYTE_SIZE - offset + 1));
}

unsigned int read_int(unsigned char *mem, int bit_offset, unsigned int size) {
    assert(0 <= bit_offset);
    if (size == 0) {
        return 0;
    }
    while (bit_offset >= BYTE_SIZE) {
        mem = mem + 1;
        bit_offset = bit_offset - BYTE_SIZE;
    }
    int n = read_int(mem, bit_offset + 1, size - 1);
    return (n << 1) + read_bit(*mem, bit_offset);
}

void write_int(char *mem, int bit_offset, int size, unsigned int value) {
    assert(0 <= bit_offset);
    if (size == 0) {
        return;
    }
    while (bit_offset >= sizeof(char)) {
        mem = mem + 1;
        bit_offset = bit_offset - sizeof(char);
    }
    *mem = write_bit(*mem, bit_offset, value % 2);
    write_int(mem, bit_offset + 1, size - 1, value >> 1);
}

