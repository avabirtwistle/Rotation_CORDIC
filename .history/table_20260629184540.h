#ifndef Z_TABLE_H
#define Z_TABLE_H

#define CORDIC_ITERATIONS 15
#define CORDIC_Q15_SCALE  15
#define CORDIC_ONE_Q15    (1 << CORDIC_Q15_SCALE)

//arctan(2^-i) in Q15, i = 0 .. 14 (from lecture testbench output)
static const int z_table[CORDIC_ITERATIONS] = {
    25735, 15192, 8027, 4074, 2045, 1023,
    511, 255, 127, 63, 31, 15, 7, 3, 1
};

//Processing gain A[n] = prod sqrt(1 + 2^(-2i)), n = 15 iterations indexed 0..14
#define CORDIC_GAIN_FLOAT 1.6467603f
#endif