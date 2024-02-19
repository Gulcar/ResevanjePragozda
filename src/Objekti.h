#pragma once

#include "Risalnik.h"
#include <vector>
#include <glm/glm.hpp>
#include <queue>

constexpr float hitrost_igralca = 5.6f;
constexpr float hitrost_zlobnezev = 2.7f;
constexpr float hitrost_pomagacev = 4.0f;

class Animacija
{
public:
    Animacija() {}
    Animacija(int zac_x, int zac_y, int st_framov, float cas_frama, bool loop = true, int tile_px = 32);

    void posodobi(float delta_time);
    void narisi(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h = false, glm::vec4 barva = glm::vec4(1.0f));

    bool je_koncana();
    void reset();

    int vrni_tile_px();

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

class TileMap
{
public:
    TileMap(const Tekstura* teks, int sirina, int visina);
    void narisi();

    glm::vec2 center = { 0.0f, 0.0f };

    int sirina;
    int visina;

    const Tekstura* tekstura;
    std::vector<Sprite> mozni_tili;
    std::vector<uint8_t> tili;
};

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

    const Tekstura* tekstura;
    const Tekstura* togenj;
    std::vector<Drevo> drevesa;
};

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

class Pomocnik
{
public:
    Pomocnik(const Tekstura* tekstura, int sprite, glm::vec2 pozicija, float razdalja_napada);

    void posodobi(float delta_time, std::vector<Zlobnez>& zlobnezi);
    void narisi();

    static void daj_narazen(std::vector<Pomocnik>& pomocniki);

    const Tekstura* tekstura;
    glm::vec2 pozicija;

    Animacija animacije[3];
    int trenutna_anim;
    bool flip_h;

    float cas_napada = 0.0f;
    float razdalja_napada;
};

