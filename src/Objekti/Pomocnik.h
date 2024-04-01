#pragma once

#include "../Risalnik.h"
#include "../Animacija.h"
#include <glm/vec2.hpp>
#include <iosfwd>

class Pomocnik
{
public:
    Pomocnik(const Tekstura* tekstura, int sprite, glm::vec2 pozicija, float razdalja_napada);

    void posodobi(float delta_time, std::vector<class Zlobnez>& zlobnezi);
    void narisi();

    static void daj_narazen(std::vector<Pomocnik>& pomocniki);

    void shrani(std::ofstream& file);

    const Tekstura* tekstura;
    glm::vec2 pozicija;

    Animacija animacije[3];
    int trenutna_anim;
    bool flip_h;
    int sprite;

    float cas_napada = 0.0f;
    float razdalja_napada;
};
