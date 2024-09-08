#include "fastRandom.h"

#include <stdio.h>

// Constants for the LCG (Linear Congruential Generator)
#define MULTIPLIER 1664525
#define INCREMENT 1013904223
#define MODULUS 4294967296 // 2^32 for a 32-bit generator

// Global state variable to hold the current seed
unsigned int seed;

// Function to initialize the seed
void FastRandomSeed(int i) {
  seed = (unsigned int)i; // Cast to unsigned to avoid negative seeds
}

// Function to generate the next random number
unsigned int FastRandom() {
  seed = (MULTIPLIER * seed + INCREMENT) % MODULUS;
  return seed;
}

// Function to generate a random integer between s (inclusive) and e (exclusive)
int FastRand(int s, int e) {
  // Range between s and e (exclusive)
  return s + (FastRandom() % (e - s));
}

// Function to randomly choose between s and e
int FastRndOr(int s, int e) { return (FastRandom() % 2) ? s : e; }

// Function to generate a random float between 0 and 1
float FastRnd() { return (float)FastRandom() / (float)(MODULUS - 1); }
