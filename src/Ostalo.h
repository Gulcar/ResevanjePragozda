#pragma once
#include <stdio.h>
#include <cstdlib>
#include <cstdint>
#include <glm/vec2.hpp>

#define ERROR_EXIT(...) { printf("ERROR: "); printf(__VA_ARGS__); printf("\n"); std::exit(1); }

float perlin_noise(float x, float y);

template<typename T, size_t N>
T& nakljucno_iz(T (&tab)[N])
{
    return tab[rand() % N];
}

bool se_prekrivata(glm::vec2 poz_a, glm::vec2 vel_a, glm::vec2 poz_b, glm::vec2 vel_b);
bool narisi_trkalnike();
void set_risanje_trkalnikov(bool risanje);

