#pragma once
#include <stdio.h>
#include <cstdlib>
#include <cstdint>

#define ERROR_EXIT(...) { printf("ERROR: "); printf(__VA_ARGS__); printf("\n"); std::exit(1); }

float perlin_noise(float x, float y);

template<typename T, size_t N>
T& nakljucno_iz(T (&tab)[N])
{
    return tab[rand() % N];
}
