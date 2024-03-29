#pragma once

#include "../Animacija.h"
#include "../Risalnik.h"

class Igralec
{
public:
    Igralec(const Tekstura* vegovec, const Tekstura* voda);
    void posodobi(float delta_time, std::vector<class Zlobnez>& zlobnezi, class Gozd& gozd);
    void narisi();

    glm::vec2 pozicija = { 0.0f, 0.0f };

    bool flip_h = false;
    bool napada = false;

    Animacija animacije[3];
    int trenutna_anim;

    Animacija voda_anim;

    const Tekstura* tvegovec;
    const Tekstura* tvoda;
};
