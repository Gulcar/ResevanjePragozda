#include "Objekti.h"
#include "Risalnik.h"
#include "Input.h"
#include "Ostalo.h"
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

Animacija::Animacija(int zac_x, int zac_y, int st_framov, float cas_frama, bool loop, int tile_px)
{
    m_zacetek = { zac_x, zac_y };
    m_trenuten_cas = 0.0f;
    m_st_framov = st_framov;
    m_cas_frama = cas_frama;
    m_loop = loop;
    m_tile_px = tile_px;
}

void Animacija::posodobi(float delta_time)
{
    m_trenuten_cas += delta_time;

    if (m_loop && m_trenuten_cas > m_st_framov * m_cas_frama)
    {
        m_trenuten_cas -= m_st_framov * m_cas_frama;
    }
}

void Animacija::narisi(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h, glm::vec4 barva)
{
    int frame = (int)(m_trenuten_cas / m_cas_frama);
    if (frame >= m_st_framov)
        frame = m_st_framov - 1;

    Sprite sprite = tekstura.ustvari_sprite(m_zacetek.x + frame, m_zacetek.y, m_tile_px);
    risalnik::narisi_sprite(sprite, pozicija, velikost, flip_h, barva);
}

void Igralec::posodobi(float delta_time)
{
    trenutna_animacija->posodobi(delta_time);

    glm::vec2 premik;
    premik.x = input::os_tipk(GLFW_KEY_A, GLFW_KEY_D);
    premik.y = input::os_tipk(GLFW_KEY_S, GLFW_KEY_W);

    if (premik.x != 0.0f || premik.y != 0.0f)
    {
        premik = glm::normalize(premik);
        pozicija += premik * hitrost * delta_time;

        trenutna_animacija = &hoja_simple;
        if (premik.x < 0.0f) flip_h = true;
        else if (premik.x > 0.0f) flip_h = false;
    }
    else
    {
        trenutna_animacija = &primiru;
    }

    glm::vec2 kamera = risalnik::dobi_pozicijo_kamere();
    kamera = glm::lerp(kamera, pozicija + premik * 0.6f, 6.0f * delta_time);
    kamera = glm::round(kamera * 400.0f) / 400.0f;
    risalnik::nastavi_pozicijo_kamere(kamera);
}

void Igralec::narisi()
{
    trenutna_animacija->narisi(*tekstura, glm::vec3(pozicija, -pozicija.y / 10000.0f), glm::vec2(3.0f), flip_h);
}

TileMap::TileMap(const Tekstura* teks, int sirina, int visina)
    : tekstura(teks)
{
    for (int y = 1; y <= 3; y++)
    {
        for (int x = 1; x <= 2; x++)
            mozni_tili.push_back(teks->ustvari_sprite(x, y, 16));
    }

    this->sirina = sirina;
    this->visina = visina;

    tili.resize(sirina * visina);

    for (int y = 0; y < visina; y++)
    {
        for (int x = 0; x < sirina; x++)
        {
            tili[x + y * sirina] = rand() % mozni_tili.size();
        }
    }
}

void TileMap::narisi()
{
    for (int y = 0; y < visina; y++)
    {
        for (int x = 0; x < sirina; x++)
        {
            const Sprite& sprite = mozni_tili[tili[x + y * sirina]];
            glm::vec3 poz = glm::vec3(center.x + x - sirina / 2.0f, center.y + y - visina / 2.0f, -0.5f);
            risalnik::narisi_sprite(sprite, poz, glm::vec2(1.0001f));
        }
    }
}

void Drevo::narisi()
{
    risalnik::narisi_sprite(sprite, glm::vec3(pozicija, (-pozicija.y + 1.5f) / 10000.0f), glm::vec2(3.0f, 6.0f));
}

Gozd::Gozd(const Tekstura* teks, glm::vec2 obmocje)
{
    tekstura = teks;

    Sprite mozni_spriti[] = {
        teks->ustvari_sprite(0, 4, 16, 2, 4),
        teks->ustvari_sprite(2, 4, 16, 2, 4),
        teks->ustvari_sprite(4, 4, 16, 2, 4),
        teks->ustvari_sprite(6, 4, 16, 2, 4)
    };

    float seed_x = rand() / (float)RAND_MAX * 10000.0f;
    float seed_y = rand() / (float)RAND_MAX * 10000.0f;

    for (float y = -obmocje.y / 2.0f; y < obmocje.y / 2.0f; y += 2.0f)
    {
        for (float x = -obmocje.x / 2.0f; x < obmocje.x / 2.0f; x += 2.0f)
        {
            glm::vec2 vec = { x, y };
            float noise = perlin_noise(x * 0.08f + seed_x, y * 0.08f + seed_y);
            noise -= glm::smoothstep(obmocje.x / 2.0f - 15.0f, obmocje.x / 2.0f, std::abs(x));
            noise -= glm::smoothstep(obmocje.y / 2.0f - 15.0f, obmocje.y / 2.0f, std::abs(y));

            if (glm::length2(vec) > 100.0f && noise > 0.1f)
                drevesa.emplace_back(vec, nakljucno_iz(mozni_spriti));
        }
    }
}

void Gozd::narisi()
{
    for (auto& drevo : drevesa)
        drevo.narisi();
}

void Zlobnez::posodobi(float delta_time)
{
    pozicija -= glm::normalize(pozicija) * 3.0f * delta_time;
}

void Zlobnez::narisi()
{
    risalnik::narisi_sprite(sprite, glm::vec3(pozicija, -pozicija.y / 10000.0f), glm::vec2(3.0f));
}

void ZlobnezSpawner::posodobi(float delta_time, Igralec* igralec)
{
    cas += delta_time;

    for (int i = 0; i < zlobnezi.size(); i++)
    {
        zlobnezi[i].posodobi(delta_time);

        if (glm::distance2(zlobnezi[i].pozicija, igralec->pozicija) < 8.0f)
        {
            std::swap(zlobnezi[i], zlobnezi.back());
            zlobnezi.pop_back();
            i--;
        }
    }

    if (cakanje_wava && zlobnezi.size() == 0)
    {
        cakanje_wava = false;
        waves.front().cas_zadnjega_spawna = cas;
    }

    if (waves.size() > 0 && cakanje_wava == false)
    {
        Wave& wave = waves.front();
        if (cas > wave.cas_zadnjega_spawna + wave.cas_spawna)
        {
            wave.cas_zadnjega_spawna += wave.cas_spawna;
            wave.st_zlobnezov -= 1;
            naredi_zlobneza();

            if (wave.st_zlobnezov == 0)
            {
                std::cout << "wave spawning koncan\n";
                waves.pop();
                cakanje_wava = true;
            }
        }
    }
}

void ZlobnezSpawner::narisi()
{
    for (auto& zlobnez : zlobnezi)
        zlobnez.narisi();
}

void ZlobnezSpawner::naredi_zlobneza()
{
    glm::vec2 pozicija;
    float rob = (rand() / (float)RAND_MAX) * (2 * (obmocje.x + obmocje.y));

    float levo = -obmocje.x / 2.0f;
    float desno = obmocje.x / 2.0f;
    float gor = obmocje.y / 2.0f;
    float dol = -obmocje.y / 2.0f;

    if (rob < obmocje.x)
        pozicija = { levo + rob, dol };
    else if (rob < obmocje.x + obmocje.y)
    {
        rob -= obmocje.x;
        pozicija = { desno, dol + rob };
    }
    else if (rob < 2 * obmocje.x + obmocje.y)
    {
        rob -= obmocje.x;
        rob -= obmocje.y;
        pozicija = { desno - rob, gor };
    }
    else
    {
        rob -= 2 * obmocje.x;
        rob -= obmocje.y;
        pozicija = { levo, gor - rob };
    }

    zlobnezi.emplace_back(tekstura->ustvari_sprite(0, 1), pozicija);
}

void ZlobnezSpawner::nastavi_wave(int st_zlobnezov, float cas_spawna)
{
    waves.push(Wave {
        st_zlobnezov,
        cas_spawna,
        0.0f
    });
}

