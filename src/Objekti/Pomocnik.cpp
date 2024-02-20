#include "Pomocnik.h"
#include "Zlobnez.h"
#include "../Ostalo.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

Pomocnik::Pomocnik(const Tekstura* tekstura, int sprite, glm::vec2 pozicija, float razdalja_napada)
    : tekstura(tekstura), pozicija(pozicija), razdalja_napada(razdalja_napada)
{
    animacije[0] = Animacija(0, sprite, 1, 1.000f); // idle
    animacije[1] = Animacija(0, sprite, 4, 0.100f); // walk

    switch (sprite)
    { // za napad
    case 0: animacije[2] = Animacija(4, sprite, 4, 0.060f, false); break;
    case 1: animacije[2] = Animacija(4, sprite, 4, 0.060f, false); break;
    case 2: animacije[2] = Animacija(4, sprite, 3, 0.060f, false); break;
    case 3: animacije[2] = Animacija(4, sprite, 3, 0.080f, false); break;
    case 4: animacije[2] = Animacija(4, sprite, 3, 0.080f, false); break;
    default: assert(false);
    }

    trenutna_anim = 0;
}

void Pomocnik::posodobi(float delta_time, std::vector<Zlobnez>& zlobnezi)
{
    Zlobnez* najblizji = nullptr;
    float najblizji_raz = FLT_MAX;

    for (auto& zlobnez : zlobnezi)
    {
        if (glm::length2(pozicija) > 50.0f * 50.0f)
            continue;
        float raz = glm::distance2(zlobnez.pozicija, pozicija);
        if (raz < najblizji_raz)
        {
            najblizji = &zlobnez;
            najblizji_raz = raz;
        }
    }

    animacije[trenutna_anim].posodobi(delta_time);
    cas_napada += delta_time;

    if (cas_napada > 1.0f && najblizji_raz < razdalja_napada * razdalja_napada)
    {
        cas_napada = 0.0f;
        trenutna_anim = 2;
        animacije[trenutna_anim].reset();
        flip_h = najblizji->pozicija.x > pozicija.x;
        najblizji->zdravje -= 25;
    }
    else if (cas_napada > 1.0f && najblizji_raz < 20.0f * 20.0f)
    {
        pozicija += glm::normalize(najblizji->pozicija - pozicija) * HITROST_POMAGACEV * delta_time;
        trenutna_anim = 1;
        flip_h = najblizji->pozicija.x > pozicija.x;
    }
    else if (cas_napada > 10.0f && glm::length2(pozicija) > 15.0f * 15.0f)
    {
        pozicija -= glm::normalize(pozicija) * 3.0f * delta_time;
        trenutna_anim = 1;
        flip_h = pozicija.x < 0.0f;
    }
    else if (cas_napada > 1.0f || animacije[2].je_koncana())
    {
        trenutna_anim = 0;
    }
}

void Pomocnik::narisi()
{
    animacije[trenutna_anim].narisi(*tekstura, glm::vec3(pozicija, -pozicija.y / 10000.0f), glm::vec2(3.0f), flip_h);
}

void Pomocnik::daj_narazen(std::vector<Pomocnik>& pomocniki)
{
    for (int i = 0; i < pomocniki.size(); i++)
    {
        for (int j = i + 1; j < pomocniki.size(); j++)
        {
            float dist = glm::distance(pomocniki[i].pozicija, pomocniki[j].pozicija);
            if (dist < 1.0f)
            {
                glm::vec2 smer = glm::normalize(pomocniki[i].pozicija - pomocniki[j].pozicija);
                pomocniki[i].pozicija += smer * (1.0f - dist) / 2.0f;
                pomocniki[j].pozicija -= smer * (1.0f - dist) / 2.0f;
            }
        }
    }
}
