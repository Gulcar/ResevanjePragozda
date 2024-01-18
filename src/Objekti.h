#pragma once

#include "Risalnik.h"
#include <vector>
#include <glm/glm.hpp>
#include <queue>

class Animacija
{
public:
    Animacija(int zac_x, int zac_y, int st_framov, float cas_frama, bool loop = true, int tile_px = 32);

    void posodobi(float delta_time);
    void narisi(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h = false, glm::vec4 barva = glm::vec4(1.0f));

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
    Igralec(const Tekstura* tekstura)
        : tekstura(tekstura) {}
    void posodobi(float delta_time);
    void narisi();

    glm::vec2 pozicija = { 0.0f, 0.0f };

    static constexpr float hitrost = 5.6f;

    bool flip_h = false;

    Animacija primiru = Animacija(1, 0, 1, 1.0f);
    Animacija hoja = Animacija(0, 1, 8, 0.100f);
    Animacija hoja_simple = Animacija(0, 2, 4, 0.100f);
    Animacija* trenutna_animacija = &hoja;

    const Tekstura* tekstura;
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
    Gozd(const Tekstura* teks, glm::vec2 obmocje);

    void narisi();

    const Tekstura* tekstura;
    std::vector<Drevo> drevesa;
};

class Zlobnez
{
public:
    Zlobnez(const Sprite& sprite, glm::vec2 pozicija)
        : sprite(sprite), pozicija(pozicija) {}

    void posodobi(float delta_time);
    void narisi();

    // TODO: animacije

    Sprite sprite;
    glm::vec2 pozicija;
};

class ZlobnezSpawner
{
public:
    ZlobnezSpawner(const Tekstura* tekstura, glm::vec2 obmocje)
        : tekstura(tekstura), obmocje(obmocje) {}

    void posodobi(float delta_time, Igralec* igralec);
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

