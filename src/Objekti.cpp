#include "Objekti.h"
#include "Risalnik.h"
#include "Input.h"
#include "Ostalo.h"
#include "glm/geometric.hpp"
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

bool Animacija::je_koncana()
{
    return m_trenuten_cas >= m_st_framov * m_cas_frama;
}

void Animacija::reset()
{
    m_trenuten_cas = 0.0f;
}

Igralec::Igralec(const Tekstura* vegovec, const Tekstura* voda)
    : tvegovec(vegovec), tvoda(voda)
{
    animacije[0] = Animacija(2, 0, 1, 1.0f); // primiru
    animacije[1] = Animacija(0, 1, 8, 0.100f); // hoja
    animacije[2] = Animacija(4, 2, 4, 0.100f); // hoja simple
    trenutna_anim = 0;

    voda_anim = Animacija(0, 0, 4, 0.070f, true, 80);
}

void Igralec::posodobi(float delta_time, std::vector<Zlobnez>& zlobnezi)
{
    animacije[trenutna_anim].posodobi(delta_time);
    voda_anim.posodobi(delta_time);

    glm::vec2 premik;
    premik.x = input::os_tipk(GLFW_KEY_A, GLFW_KEY_D);
    premik.y = input::os_tipk(GLFW_KEY_S, GLFW_KEY_W);

    if (premik.x != 0.0f || premik.y != 0.0f)
    {
        premik = glm::normalize(premik);
        pozicija += premik * hitrost_igralca * delta_time;

        trenutna_anim = 2;
        if (premik.x < 0.0f) flip_h = true;
        else if (premik.x > 0.0f) flip_h = false;
    }
    else
    {
        trenutna_anim = 0;
    }

    pozicija.x = std::clamp(pozicija.x, -50.0f, 50.0f);
    pozicija.y = std::clamp(pozicija.y, -37.0f, 37.0f);

    glm::vec2 kamera = risalnik::dobi_pozicijo_kamere();
    kamera = glm::lerp(kamera, pozicija + premik * 0.6f, 6.0f * delta_time);
    kamera = glm::round(kamera * 400.0f) / 400.0f;
    risalnik::nastavi_pozicijo_kamere(kamera);

    napada = input::tipka_drzana(GLFW_KEY_SPACE);
    if (napada)
    {
        glm::vec2 pozicija_vode = pozicija;
        pozicija_vode.x += flip_h ? -3.5f : 3.5f;
        pozicija_vode.y -= 0.3f;
        
        glm::vec2 velikost = { 5.0f, 3.0f };

        for (int i = 0; i < zlobnezi.size(); i++)
        {
            if (se_prekrivata(pozicija_vode, velikost, zlobnezi[i].pozicija, zlobnezi[i].velikost))
            {
                zlobnezi[i].zdravje -= 100.0f * delta_time;
            }
        }
    }
}

void Igralec::narisi()
{
    animacije[trenutna_anim].narisi(*tvegovec, glm::vec3(pozicija, -pozicija.y / 10000.0f), glm::vec2(3.0f), flip_h);

    if (napada)
    {
        glm::vec2 pozicija_vode = pozicija;
        pozicija_vode.x += flip_h ? -3.5f : 3.5f;
        pozicija_vode.y -= 0.3f;
        voda_anim.narisi(*tvoda, glm::vec3(pozicija_vode, -pozicija.y / 10000.0f), glm::vec2(6.0f), flip_h);

        if (narisi_trkalnike())
            risalnik::narisi_rect(glm::vec3(pozicija_vode, -pozicija.y / 10000.0f), glm::vec2(5.0f, 3.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.3f));
    }
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
            risalnik::narisi_sprite(sprite, poz, glm::vec2(1.01f));
        }
    }
}

void Drevo::narisi()
{
    risalnik::narisi_sprite(sprite, glm::vec3(pozicija, (-pozicija.y + 1.5f) / 10000.0f), glm::vec2(3.0f, 6.0f));
}

Gozd::Gozd(const Tekstura* teks, glm::vec2 obmocje, float radij_sredine, bool znotraj)
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
            if (znotraj && glm::length2(vec) > radij_sredine * radij_sredine) continue;
            if (!znotraj && glm::length2(vec) < radij_sredine * radij_sredine) continue;

            if (znotraj)
                drevesa.emplace_back(vec, nakljucno_iz(mozni_spriti));

            float noise = perlin_noise(x * 0.08f + seed_x, y * 0.08f + seed_y);
            noise -= glm::smoothstep(obmocje.x / 2.0f - 15.0f, obmocje.x / 2.0f, std::abs(x));
            noise -= glm::smoothstep(obmocje.y / 2.0f - 15.0f, obmocje.y / 2.0f, std::abs(y));

            if (noise > 0.1f)
                drevesa.emplace_back(vec, nakljucno_iz(mozni_spriti));
        }
    }
}

void Gozd::narisi()
{
    for (auto& drevo : drevesa)
        drevo.narisi();
}

glm::vec2 Gozd::najblizje_drevo(glm::vec2 poz) const
{
    glm::vec2 najblizje;
    float min_razd = FLT_MAX;
    for (const auto& drevo : drevesa)
    {
        float razd = glm::distance2(drevo.pozicija, poz);
        if (razd < min_razd)
        {
            min_razd = razd;
            najblizje = drevo.pozicija;
        }
    }
    return najblizje;
}

Zlobnez::Zlobnez(const Tekstura* tekstura, glm::vec2 pozicija, glm::vec2 velikost, float zdravje, int sprite)
{
    this->tekstura = tekstura;
    this->zdravje = zdravje;
    this->pozicija = pozicija;
    this->velikost = velikost;

    animacije[0] = Animacija(0, sprite, 1, 1.000f, false); // pri miru
    animacije[1] = Animacija(0, sprite, 4, 0.100f); // hoja
    
    switch (sprite) // napad animacije
    {
    case 0: animacije[2] = Animacija(0, sprite, 4, 0.070f, false); break;
    case 1: animacije[2] = Animacija(4, sprite, 3, 0.070f, false); break;
    case 2: animacije[2] = Animacija(4, sprite, 2, 0.070f, false); break;
    case 3:
        animacije[1] = Animacija(0, sprite, 1, 1.000f); // buldozer
        animacije[2] = Animacija(0, sprite, 1, 0.100f, false); break;
    }

    trenutna_anim = 1;
    stanje = Stanje::ProtiCentru;
    stevilo_napadov = 0;
}

void Zlobnez::posodobi(float delta_time, const Gozd& gozd)
{
    animacije[trenutna_anim].posodobi(delta_time);
    flip_x = pozicija.x > 0.0;

    if (stanje == Stanje::ProtiCentru)
    {
        pozicija -= glm::normalize(pozicija) * hitrost_zlobnezev * delta_time;

        if (glm::length2(pozicija) < 150.0f)
        {
            stanje = Stanje::ProtiDrevesu;
            do_drevesa = gozd.najblizje_drevo(pozicija);
        }
    }
    else if (stanje == Stanje::ProtiDrevesu)
    {
        pozicija += glm::normalize(do_drevesa - pozicija) * hitrost_zlobnezev * delta_time;

        if (glm::distance2(do_drevesa, pozicija) < 1.0f)
        {
            do_drevesa = gozd.najblizje_drevo(pozicija);
            if (glm::distance2(do_drevesa, pozicija) < 1.0f)
            {
                stanje = Stanje::UnicujeDrevo;
                trenutna_anim = 2;
            }
        }
    }
    else if (stanje == Stanje::UnicujeDrevo)
    {
        if (animacije[2].je_koncana())
        {
            animacije[2].reset();
            animacije[0].reset();
            trenutna_anim = 0;
            stevilo_napadov += 1;

            if (stevilo_napadov >= 5)
            {
                stanje = Stanje::Bezi;
                trenutna_anim = 0;
                // TODO: ogenj
            }
        }
        else if (animacije[0].je_koncana())
        {
            trenutna_anim = 2;
        }
    }
    else if (stanje == Stanje::Bezi)
    {
        pozicija += glm::normalize(pozicija) * hitrost_zlobnezev * delta_time;
        flip_x = !flip_x;
        if (glm::length2(pozicija) > 90.0f * 90.0f)
            zdravje = 0.0f;
    }
}

void Zlobnez::narisi()
{
    animacije[trenutna_anim].narisi(*tekstura, glm::vec3(pozicija, -pozicija.y / 10000.0f), glm::vec2(3.0f), flip_x);

    if (narisi_trkalnike())
    {
        risalnik::narisi_rect(glm::vec3(pozicija, -pozicija.y / 10000.0f), velikost, glm::vec4(1.0f, 0.0f, 0.0f, 0.3f));
    }

    glm::vec2 zdravje_poz = pozicija;
    zdravje_poz.y += 1.2f;
    constexpr float max_vel = 1.8f;
    float vel = zdravje / 100.0f * max_vel;
    risalnik::narisi_rect(glm::vec3(zdravje_poz, -(zdravje_poz.y+0.2f) / 10000.0f), glm::vec2(max_vel, 0.12f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    zdravje_poz.x -= (max_vel - vel) / 2.0f;
    risalnik::narisi_rect(glm::vec3(zdravje_poz, -zdravje_poz.y / 10000.0f), glm::vec2(vel, 0.12f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

void ZlobnezSpawner::posodobi(float delta_time, Igralec* igralec, const Gozd& gozd)
{
    cas += delta_time;

    for (int i = 0; i < zlobnezi.size(); i++)
    {
        zlobnezi[i].posodobi(delta_time, gozd);

        if (zlobnezi[i].zdravje <= 0.0f)
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

            int r = rand() % 10;
            if (r < 4 && wave.st_vzigalnik > 0)
            {
                wave.st_vzigalnik -= 1;
                naredi_zlobneza(0);
            }
            else if (r < 7 && wave.st_sekira > 0)
            {
                wave.st_sekira -= 1;
                naredi_zlobneza(1);
            }
            else if (r < 9 && wave.st_motorka > 0)
            {
                wave.st_motorka -= 1;
                naredi_zlobneza(2);
            }
            else if (wave.st_buldozer > 0)
            {
                wave.st_buldozer -= 1;
                naredi_zlobneza(3);
            }

            if (wave.st_vzigalnik == 0 && wave.st_sekira == 0 && wave.st_motorka == 0 && wave.st_buldozer == 0)
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

void ZlobnezSpawner::naredi_zlobneza(int sprite)
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

    zlobnezi.emplace_back(tekstura, pozicija, glm::vec2(1.5f, 2.5f), 100.0f, sprite);
}

void ZlobnezSpawner::nastavi_wave(int st_vzigalnik, int st_sekira, int st_motorka, int st_buldozer, float cas_spawna)
{
    waves.push(Wave {
        st_vzigalnik,
        st_sekira,
        st_motorka,
        st_buldozer,
        cas_spawna,
        0.0f
    });
}

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
        pozicija += glm::normalize(najblizji->pozicija - pozicija) * hitrost_pomagacev * delta_time;
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
