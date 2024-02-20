#pragma once

#include "../Animacija.h"
#include <queue>
#include <glm/vec2.hpp>

class Igralec;
class Gozd;
class Tekstura;

class Zlobnez
{
public:
    Zlobnez(const Tekstura* tekstura, glm::vec2 pozicija, glm::vec2 velikost, float zdravje, int sprite);

    void posodobi(float delta_time, Gozd& gozd);
    void narisi();

    Animacija animacije[3];
    int trenutna_anim;
    int stevilo_napadov;

    bool flip_x = false;

    const Tekstura* tekstura;

    float zdravje;
    glm::vec2 pozicija;
    glm::vec2 velikost;

    enum class Stanje {
        ProtiCentru,
        ProtiDrevesu,
        UnicujeDrevo,
        Bezi
    } stanje;

    glm::vec2 do_drevesa;
};

class ZlobnezSpawner
{
public:
    ZlobnezSpawner(const Tekstura* tekstura, glm::vec2 obmocje)
        : tekstura(tekstura), obmocje(obmocje) {}

    void posodobi(float delta_time, Igralec* igralec, Gozd& gozd);
    void narisi();

    void naredi_zlobneza(int sprite);
    void nastavi_wave(int st_vzigalnik, int st_sekira, int st_motorka, int st_buldozer, float cas_spawna);

    std::vector<Zlobnez> zlobnezi;

    struct Wave
    {
        int st_vzigalnik;
        int st_sekira;
        int st_motorka;
        int st_buldozer;
        float cas_spawna;
        float cas_zadnjega_spawna;
    };
    std::queue<Wave> waves;
    bool cakanje_wava = false;

    float cas = 0.0f;
    const Tekstura* tekstura;
    glm::vec2 obmocje;
};
