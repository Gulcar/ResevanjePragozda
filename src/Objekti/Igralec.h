#pragma once

#include "../Animacija.h"
#include "../Risalnik.h"
#include <string>

constexpr float CAS_PREMIKA = 0.050f;

struct Premik
{
    glm::vec2 pozicija;
    bool flip_h;
};

class Igralec
{
public:
    Igralec(const Tekstura* vegovec, const Tekstura* voda);
    void posodobi(float delta_time, std::vector<class Zlobnez>& zlobnezi, class Gozd& gozd);
    void narisi();

    void shrani(std::ofstream& file);
    void nalozi(std::ifstream& file);

    void shrani_premike();
    void nalozi_premike();
    void predvajaj_premik(float delta_time);

    glm::vec2 pozicija = { 0.0f, 0.0f };
    std::string ime;

    bool flip_h = false;
    bool napada = false;

    Animacija animacije[3];
    int trenutna_anim;

    Animacija voda_anim;

    const Tekstura* tvegovec;
    const Tekstura* tvoda;

    int trenuten_premik = 0;
    std::vector<Premik> vsi_premiki;
    float cas_shranjevanja_premikov = 0.0f;
};
