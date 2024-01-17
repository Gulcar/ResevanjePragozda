#pragma once
#include <stdio.h>
#include <cstdlib>

#define ERROR_EXIT(...) { printf("ERROR: "); printf(__VA_ARGS__); printf("\n"); std::exit(1); }

float perlin_noise(float x, float y);
