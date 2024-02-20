#pragma once

#include "../Risalnik.h"
#include <vector>

class Drevo
{
public:
    Drevo(glm::vec2 poz, const Sprite& sprite)
        : pozicija(poz), sprite(sprite) {}

    void posodobi(float delta_time);
    void narisi(const Tekstura* togenj);

    glm::vec2 pozicija;
    Sprite sprite;
    float cas_ognja = 0.0f;
};

class Gozd
{
public:
    Gozd(const Tekstura* teks, const Tekstura* ogenj, glm::vec2 obmocje, float radij_sredine, bool znotraj);

    void posodobi(float delta_time);
    void narisi();

    glm::vec2 najblizje_drevo(glm::vec2 poz) const;
    void zaneti_ogenj(glm::vec2 poz);
    void razsiri_ogenj(glm::vec2 poz);
    void podri_drevo(glm::vec2 poz);

    const Tekstura* tekstura;
    const Tekstura* togenj;
    std::vector<Drevo> drevesa;
};
