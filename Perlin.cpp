/*

Perlin noise functions

see http://freespace.virgin.net/hugo.elias/models/m_perlin.htm

*/


#include "Perlin.h"
#include <cmath>

#define TOTAL_PRIMES 10

const int primes[TOTAL_PRIMES][3] = {
  { 995615039, 600173719, 701464987 },
  { 831731269, 162318869, 136250887 },
  { 174329291, 946737083, 245679977 },
  { 362489573, 795918041, 350777237 },
  { 457025711, 880830799, 909678923 },
  { 787070341, 177340217, 593320781 },
  { 405493717, 291031019, 391950901 },
  { 458904767, 676625681, 424452397 },
  { 531736441, 939683957, 810651871 },
  { 997169939, 842027887, 423882827 }
};

double noise(int i, int x, int y) {
    int n, a, b, c, t;
    n = x + y*57; // why 57 to test with other
    n = (n << 13) ^ n;
    a = primes[i][0];
    b = primes[i][1];
    c = primes[i][2];
    t = (n * (n * n * a + b) + c) & 0x7fffffff;
    return 1.0 - (double)t / 1073741824.0;
}

double smoothed_noise(int i, int x, int y) {
    // itself/4 + sides/8 + corners/16
    return (noise(i, x-1, y-1) + noise(i, x-1, y+1) +
            noise(i, x+1, y+1) + noise(i, x+1, y-1)) / 16 +
           (noise(i, x, y-1) + noise(i, x, y+1) +
            noise(i, x-1, y) + noise(i, x+1, y)) / 8 +
           noise(i, x, y) / 4;
}

double interpolate(double a, double b, double x) {
    double ft(x * 3.1415927);
    double f(0.5 * (1-std::cos(ft)));
    return a * (1-f) + b * f;
}

double interpolated_noise(int i, double x, double y) {
    int xInt(x);
    double xFra(x - xInt);
    int yInt(y);
    double yFra(y - yInt);

    double v1, v2, v3, v4, i1, i2;

    v1 = smoothed_noise(i, xInt, yInt);
    v2 = smoothed_noise(i, xInt+1, yInt);
    v3 = smoothed_noise(i, xInt, yInt+1);
    v4 = smoothed_noise(i, xInt+1, yInt+1);

    i1 = interpolate(v1, v2, xFra);
    i2 = interpolate(v3, v4, xFra);

    return interpolate(i1, i2, yFra);
}

double noise_value(double x, double y) {
    double total(0);
    double frequency(std::pow(2, maxOctave));
    double amplitude(5);
    for (int i=minOctave; i<maxOctave; i++) {
        frequency /= 2;
        amplitude *= persistence;
        total += interpolated_noise((primeIndex+i)%TOTAL_PRIMES,
                    x/frequency, y/frequency) * amplitude;
    }
    return total/frequency;
}

