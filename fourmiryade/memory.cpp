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

typedef struct {
    unsigned char *mem;
    int offset;
} reader;

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


unsigned long long get_number(char* mem, int min, int max) {
  """get number form bit number min (included) from bit number max (excluded)"""
    assert(max-min <= 8*sizeof(unsigned long long));
    unsigned long long sum = 0;
    for (int x = min; x<max; x++){
        sum = sum<<1;
        sum += get_bit(mem, x);
    }
    return sum;
}
void set_number (char* mem, int min, int max, unsigned long long number) {
  """set number form bit number min (included) from bit number max (excluded) to the value of number"""
    assert(max-min <= 8*sizeof(unsigned long long));
    unsigned long long mask = 0b1;
    for (int x = max-1; x>=min; x--) {
        char bit = number & mask;
        set_bit(mem, x, bit);
        number = number >> 1;
    }
}


typedef enum {
    EXPLORATRICE,
    CARTOGRAPHE,
    OUVRIERE,
    VARIANT_COUNT,
} type_fourmi;

type_fourmi get_type(unsigned char *mem) {
    int size = ceil_log2(VARIANT_COUNT);
    return (type_fourmi) read_int(mem, 0, ceil_log2(VARIANT_COUNT));
}
