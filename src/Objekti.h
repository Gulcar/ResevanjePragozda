#pragma once

#include "Scene/Scena.h"
#include <glm/glm.hpp>

class Tekstura;
class Sprite;

class Animacija
{
public:
    Animacija(int zac_x, int zac_y, int st_framov, float cas_frama, bool loop = true, int tile_px = 32);

    void posodobi(float delta_time);
    void narisi(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h = false, glm::vec4 barva = glm::vec4(1.0f));

private:
    glm::ivec2 m_zacetek;
    float m_trenuten_cas;
    int m_st_framov;
    float m_cas_frama;
    bool m_loop;
    int m_tile_px;
};

class Igralec
{
public:
    Igralec(const Tekstura& tekstura)
        : tekstura(tekstura) {}
    void posodobi(float delta_time);
    void narisi();

    glm::vec2 pozicija = { 0.0f, 0.0f };

    static constexpr float hitrost = 280.0f;

    bool flip_h = false;

    Animacija primiru = Animacija(1, 0, 1, 1.0f);
    Animacija hoja = Animacija(0, 1, 8, 0.100f);
    Animacija* trenutna_animacija = &hoja;

    const Tekstura& tekstura;
};

class TileMap
{
public:
    TileMap(const Tekstura& teks);
    void narisi();

    glm::vec2 pozicija = { 0.0f, 0.0f };

    const Tekstura& tekstura;
    std::vector<Sprite> mozni_tili;
};
