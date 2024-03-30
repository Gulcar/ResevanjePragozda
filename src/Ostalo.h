#pragma once
#include <stdio.h>
#include <cstdlib>
#include <cstdint>
#include <glm/vec2.hpp>

#define ERROR_EXIT(...) { printf("ERROR: "); printf(__VA_ARGS__); printf("\n"); std::exit(1); }

constexpr float PI = 3.141592654;

constexpr float HITROST_IGRALCA = 5.6f;
constexpr float HITROST_ZLOBNEZEV = 2.7f;
constexpr float HITROST_POMAGACEV = 4.0f;

constexpr float DREVO_CAS_GORENJA = 20.0f;

float perlin_noise(float x, float y);

template<typename T, size_t N>
T& nakljucno_iz(T (&tab)[N])
{
    return tab[rand() % N];
}

bool se_prekrivata(glm::vec2 poz_a, glm::vec2 vel_a, glm::vec2 poz_b, glm::vec2 vel_b);
bool je_znotraj(glm::vec2 rect_poz, glm::vec2 rect_vel, glm::vec2 tocka);
bool narisi_trkalnike();
void set_risanje_trkalnikov(bool risanje);

float randf();
glm::vec2 vec2_iz_kota(float kot);
