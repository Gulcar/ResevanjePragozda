#pragma once

#include "Scene/Scena.h"
#include <glm/glm.hpp>

class Tekstura;

class Animacija
{
public:
    Animacija(int zac_x, int zac_y, int st_framov, float cas_frama, bool loop = true, int tile_px = 32);

    void posodobi(float delta_time);
    void narisi(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, glm::vec4 barva = glm::vec4(1.0f));

private:
    glm::ivec2 m_zacetek;
    float m_trenuten_cas;
    int m_st_framov;
    float m_cas_frama;
    bool m_loop;
    int m_tile_px;
};

class Igralec : public Objekt
{
public:
    Igralec(const Tekstura& tekstura)
        : tekstura(tekstura) {}
    void posodobi(float delta_time) override;
    void narisi() override;

    static constexpr float hitrost = 250.0f;

    Animacija primiru = Animacija(1, 0, 1, 1.0f);
    Animacija hoja = Animacija(0, 1, 8, 0.100f);
    Animacija* trenutna_animacija = &hoja;

    const Tekstura& tekstura;
};
