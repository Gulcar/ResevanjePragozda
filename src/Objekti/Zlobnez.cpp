#pragma once

#include "Zlobnez.h"
#include "Igralec.h"
#include "Gozd.h"
#include "../Risalnik.h"
#include "../Ostalo.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

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
    case 3: // buldozer
        animacije[0] = Animacija(0, 2, 1, 1.000f, false, 64);
        animacije[1] = Animacija(0, 2, 1, 1.000f, true, 64);
        animacije[2] = Animacija(0, 2, 1, 0.100f, false, 64);
        break;
    }

    trenutna_anim = 1;
    stanje = Stanje::ProtiCentru;
    stevilo_napadov = 0;
}

void Zlobnez::posodobi(float delta_time, Gozd& gozd)
{
    animacije[trenutna_anim].posodobi(delta_time);
    flip_x = pozicija.x > 0.0;

    if (stanje == Stanje::ProtiCentru)
    {
        pozicija -= glm::normalize(pozicija) * HITROST_ZLOBNEZEV * delta_time;

        if (glm::length2(pozicija) < 150.0f)
        {
            stanje = Stanje::ProtiDrevesu;
            do_drevesa = gozd.najblizje_drevo(pozicija);
        }
    }
    else if (stanje == Stanje::ProtiDrevesu)
    {
        pozicija += glm::normalize(do_drevesa - pozicija) * HITROST_ZLOBNEZEV * delta_time;

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
                trenutna_anim = 1;
                gozd.zaneti_ogenj(do_drevesa);
            }
        }
        else if (animacije[0].je_koncana())
        {
            trenutna_anim = 2;
        }
    }
    else if (stanje == Stanje::Bezi)
    {
        pozicija += glm::normalize(pozicija) * HITROST_ZLOBNEZEV * delta_time;
        flip_x = !flip_x;
        if (glm::length2(pozicija) > 90.0f * 90.0f)
            zdravje = 0.0f;
    }
}

void Zlobnez::narisi()
{
    bool je_buldozer = animacije[0].vrni_tile_px() == 64;

    glm::vec2 velikost_slikice = je_buldozer ? glm::vec2(6.0f) : glm::vec2(3.0f);
    float z_index = je_buldozer ? (-(pozicija.y - 0.7f) / 10000.0f) : (-pozicija.y / 10000.0f);
    animacije[trenutna_anim].narisi(*tekstura, glm::vec3(pozicija, z_index), velikost_slikice, flip_x);

    if (narisi_trkalnike())
    {
        risalnik::narisi_rect(glm::vec3(pozicija, 1.0f), velikost, glm::vec4(1.0f, 0.0f, 0.0f, 0.3f));
    }

    glm::vec2 zdravje_poz = pozicija;
    zdravje_poz.y += je_buldozer ? 2.2f : 1.2f;
    float max_vel = je_buldozer ? 4.0f : 1.8f;
    float max_zdravje = je_buldozer ? 500.0f : 100.0f;

    float vel = zdravje / max_zdravje * max_vel;
    risalnik::narisi_rect(glm::vec3(zdravje_poz, -(zdravje_poz.y + 0.2f) / 10000.0f), glm::vec2(max_vel, 0.12f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    zdravje_poz.x -= (max_vel - vel) / 2.0f;
    risalnik::narisi_rect(glm::vec3(zdravje_poz, -zdravje_poz.y / 10000.0f), glm::vec2(vel, 0.12f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

void ZlobnezSpawner::posodobi(float delta_time, Igralec* igralec, Gozd& gozd)
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
        if (waves.size() > 0)
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

    glm::vec2 vel_trkalnika = (sprite == 3) ? glm::vec2(4.0f, 2.8f) : glm::vec2(1.5f, 2.5f);
    float zdravje = (sprite == 3) ? 500.0f : 100.0f;
    zlobnezi.emplace_back(tekstura, pozicija, vel_trkalnika, zdravje, sprite);
}

void ZlobnezSpawner::nastavi_wave(int st_vzigalnik, int st_sekira, int st_motorka, int st_buldozer, float cas_spawna)
{
    waves.push(Wave{
        st_vzigalnik,
        st_sekira,
        st_motorka,
        st_buldozer,
        cas_spawna,
        0.0f
        });
}
