#pragma once

#include <vector>
#include <glm/glm.hpp>

class Tekstura;

class Animacija
{
public:
    Animacija() = default;
    Animacija(int zac_x, int zac_y, int st_framov, float cas_frama, bool loop = true, int tile_px = 32);

    void posodobi(float delta_time);
    void narisi(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h = false, glm::vec4 barva = glm::vec4(1.0f), float rotacija = 0.0f);

    bool je_koncana();
    void reset();

    int vrni_tile_px();
    float vrni_cas();

private:
    glm::ivec2 m_zacetek;
    float m_trenuten_cas;
    int m_st_framov;
    float m_cas_frama;
    bool m_loop;
    int m_tile_px;
};
