#pragma once

#include "../Animacija.h"
#include <queue>
#include <glm/vec2.hpp>

class Igralec;
class Gozd;
class Tekstura;

enum class ZlobnezTip
{
    Pozigalec,
    Gozdar,
    Plamenometalec,
    Buldozer
};

class Zlobnez
{
public:
    Zlobnez(const Tekstura* tekstura, glm::vec2 pozicija, ZlobnezTip tip);

    void posodobi(float delta_time, Gozd& gozd);
    void narisi();

    ZlobnezTip tip;

    Animacija animacije[3];
    int trenutna_anim;
    bool flip_x;

    int stevilo_napadov;

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

    void naredi_zlobneza(ZlobnezTip tip);
    void nastavi_wave(int st_vzigalnik, int st_sekira, int st_plamenometalec, int st_buldozer, float cas_spawna);

    std::vector<Zlobnez> zlobnezi;

    struct Wave
    {
        int st_vzigalnik;
        int st_sekira;
        int st_plamenometalec;
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
