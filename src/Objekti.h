#pragma once

#include "Risalnik.h"
#include <vector>
#include <glm/glm.hpp>
#include <queue>

class Animacija
{
public:
    Animacija() {}
    Animacija(int zac_x, int zac_y, int st_framov, float cas_frama, bool loop = true, int tile_px = 32);

    void posodobi(float delta_time);
    void narisi(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h = false, glm::vec4 barva = glm::vec4(1.0f));
    bool je_koncana();

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
    void posodobi(float delta_time, std::vector<class Zlobnez>& zlobnezi);
    void narisi();

    glm::vec2 pozicija = { 0.0f, 0.0f };

    static constexpr float hitrost = 5.6f;

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

    void narisi();

    glm::vec2 pozicija;
    Sprite sprite;
};

class Gozd
{
public:
    Gozd(const Tekstura* teks, glm::vec2 obmocje, float radij_sredine, bool znotraj);

    void narisi();
    glm::vec2 najblizje_drevo(glm::vec2 poz) const;

    const Tekstura* tekstura;
    std::vector<Drevo> drevesa;
};

class Zlobnez
{
public:
    Zlobnez(const Tekstura* tekstura, glm::vec2 pozicija, glm::vec2 velikost, float zdravje);

    void posodobi(float delta_time, const Gozd& gozd);
    void narisi();

    Animacija animacije[2];
    int trenutna_anim;

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

    void posodobi(float delta_time, Igralec* igralec, const Gozd& gozd);
    void narisi();

    void naredi_zlobneza();
    void nastavi_wave(int st_zlobnezov, float cas_spawna);

    std::vector<Zlobnez> zlobnezi;

    struct Wave
    {
        int st_zlobnezov;
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

    const Tekstura* tekstura;
    glm::vec2 pozicija;

    Animacija animacije[3];
    int trenutna_anim;
    bool flip_h;

    float cas_napada = 0.0f;
    float razdalja_napada;
};

