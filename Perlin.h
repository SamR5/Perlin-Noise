#ifndef PERLIN_H
#define PERLIN_H

const int TOTAL_PRIMES = 10;

#include "Perlin.h"


extern double persistence; // useless after 1.5
extern double deltaPerisistance;
extern int maxOctave;
extern int minOctave;
extern int primeIndex;



double noise(int i, int x, int y);
double smoothed_noise(int i, int x, int y);
double interpolate(double a, double b, double x);
double interpolated_noise(int i, double x, double y);
double noise_value(double x, double y);




#endif
