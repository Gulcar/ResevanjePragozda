#include "Zlobnez.h"
#include "Igralec.h"
#include "Gozd.h"
#include "../Risalnik.h"
#include "../Ostalo.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <fstream>

Zlobnez::Zlobnez(const Tekstura* tekstura, glm::vec2 pozicija, ZlobnezTip tip)
{
    this->tip = tip;
    this->tekstura = tekstura;
    this->pozicija = pozicija;
    this->cas_smrti = 0.0f;

    trenutna_anim = 1;
    stanje = Stanje::ProtiCentru;
    stevilo_napadov = 0;

    switch (tip)
    {
    case ZlobnezTip::Pozigalec:
        this->zdravje = 100.0f;
        this->velikost = glm::vec2(1.5f, 2.5f);
        animacije[0] = Animacija(0, 0, 1, 1.000f, false); // pri miru
        animacije[1] = Animacija(0, 0, 4, 0.100f); // hoja
        animacije[2] = Animacija(0, 0, 4, 0.070f, false); // napad
        break;

    case ZlobnezTip::Gozdar:
        this->zdravje = 100.0f;
        this->velikost = glm::vec2(1.5f, 2.5f);
        animacije[0] = Animacija(0, 1, 1, 1.000f, false); // pri miru
        animacije[1] = Animacija(0, 1, 4, 0.100f); // hoja
        animacije[2] = Animacija(4, 1, 3, 0.070f, false); // napad
        break;

    case ZlobnezTip::Plamenometalec:
        this->zdravje = 100.0f;
        this->velikost = glm::vec2(1.5f, 2.5f);
        animacije[0] = Animacija(0, 3, 1, 0.001f, false); // pri miru
        animacije[1] = Animacija(0, 3, 4, 0.100f); // hoja
        animacije[2] = Animacija(0, 3, 1, 0.450f, false); // napad
        break;


    case ZlobnezTip::Buldozer:
        this->zdravje = 600.0f;
        this->velikost = glm::vec2(4.0f, 2.8f);
        animacije[0] = Animacija(0, 2, 1, 0.200f, false, 64);
        animacije[1] = Animacija(0, 2, 1, 1.000f, true, 64);
        animacije[2] = Animacija(0, 2, 1, 0.100f, false, 64);
        break;

    default: assert(false && "napacen zlobnez tip");
    }
}

void Zlobnez::posodobi(float delta_time, Gozd& gozd)
{
    if (zdravje <= 0.0f)
    {
        cas_smrti += delta_time;
        return;
    }

    animacije[trenutna_anim].posodobi(delta_time);

    float hitrost_mult = (tip == ZlobnezTip::Buldozer) ? 0.5f : 1.0f;

    if (stanje == Stanje::ProtiCentru)
    {
        pozicija -= glm::normalize(pozicija) * HITROST_ZLOBNEZEV * hitrost_mult * delta_time;
        flip_x = pozicija.x > 0.0;

        if (glm::length2(pozicija) < 150.0f)
        {
            stanje = Stanje::ProtiDrevesu;
            do_drevesa = gozd.najblizje_drevo(pozicija);
        }
    }
    else if (stanje == Stanje::ProtiDrevesu)
    {
        glm::vec2 do_drevesa_zraven = do_drevesa;
        if (tip == ZlobnezTip::Plamenometalec)
            do_drevesa_zraven.x += (pozicija.x > do_drevesa.x) ? 2.0f : -2.0f;
        do_drevesa_zraven.y -= 0.5f;

        pozicija += glm::normalize(do_drevesa_zraven - pozicija) * HITROST_ZLOBNEZEV * hitrost_mult * delta_time;
        flip_x = pozicija.x > do_drevesa.x;

        if (glm::distance2(do_drevesa_zraven, pozicija) < 1.0f)
        {
            glm::vec2 najblizje = do_drevesa;
            do_drevesa = gozd.najblizje_drevo(do_drevesa);
            if (glm::distance2(najblizje, do_drevesa) < 0.5f)
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
                if (tip == ZlobnezTip::Buldozer || tip == ZlobnezTip::Gozdar)
                {
                    stanje = Stanje::ProtiDrevesu;
                    trenutna_anim = 1;
                    gozd.podri_drevo(do_drevesa);
                    do_drevesa = gozd.najblizje_drevo(pozicija);
                    stevilo_napadov = 0;
                }
                else
                {
                    stanje = Stanje::Bezi;
                    trenutna_anim = 1;
                    gozd.zaneti_ogenj(do_drevesa);
                }
            }
        }
        else if (animacije[0].je_koncana())
        {
            trenutna_anim = 2;
        }
    }
    else if (stanje == Stanje::Bezi)
    {
        pozicija += glm::normalize(pozicija) * HITROST_ZLOBNEZEV * hitrost_mult * delta_time;
        flip_x = pozicija.x < 0.0;
        if (pozicija.x > 70.0f || pozicija.x < -70.0f || pozicija.y > 50.0f || pozicija.y < -50.0f)
            zdravje = 0.0f;
    }
}

void Zlobnez::narisi()
{
    bool je_buldozer = tip == ZlobnezTip::Buldozer;

    glm::vec2 velikost_slikice = je_buldozer ? glm::vec2(6.0f) : glm::vec2(3.0f);
    float z_index = je_buldozer ? (-(pozicija.y - 0.7f) / 10000.0f) : (-pozicija.y / 10000.0f);
    glm::vec2 poz_risanja = pozicija;
    if (je_buldozer)
    {
        poz_risanja.x += (randf() - 0.5f) * 0.07f;
        poz_risanja.y += (randf() - 0.5f) * 0.07f;
    }
    
    if (zdravje > 0.0f)
    {
        animacije[trenutna_anim].narisi(*tekstura, glm::vec3(poz_risanja, z_index), velikost_slikice, flip_x);
    }
    else
    {
        float rotacija = smer_smrti ? -PI / 2.0f : PI / 2.0f;
        poz_risanja.x += smer_smrti ? 1.2f : -1.2f;
        poz_risanja.y -= 1.2f;
        animacije[0].narisi(*tekstura, glm::vec3(poz_risanja, z_index), velikost_slikice, smer_smrti, glm::vec4(1.0f), rotacija);
        return;
    }

    if (tip == ZlobnezTip::Plamenometalec && trenutna_anim == 2)
    {
        int plamen_frame = (int)(animacije[trenutna_anim].vrni_cas() / 0.1f) % 4;
        Sprite plamen = tekstura->ustvari_sprite(4 + plamen_frame, 3);
        glm::vec2 poz_plamena = pozicija;
        poz_plamena.x += flip_x ? -2.5f : 2.5f;
        poz_plamena.y -= 0.5f;
        risalnik::narisi_sprite(plamen, glm::vec3(poz_plamena, z_index - 0.000001f), glm::vec2(3.0f), flip_x);
    }

    if (narisi_trkalnike())
    {
        risalnik::narisi_rect(glm::vec3(pozicija, 1.0f), velikost, glm::vec4(1.0f, 0.0f, 0.0f, 0.3f));
    }

    glm::vec2 zdravje_poz = pozicija;
    zdravje_poz.y += je_buldozer ? 2.2f : 1.2f;
    float max_vel = je_buldozer ? 4.0f : 1.8f;
    float max_zdravje = je_buldozer ? 600.0f : 100.0f;

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

        if (zlobnezi[i].cas_smrti > 0.400f)
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
        {
            st_wava += 1;
            waves.front().cas_zadnjega_spawna = cas;
        }
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
                naredi_zlobneza(ZlobnezTip::Pozigalec);
            }
            else if (r < 7 && wave.st_sekira > 0)
            {
                wave.st_sekira -= 1;
                naredi_zlobneza(ZlobnezTip::Gozdar);
            }
            else if (r < 9 && wave.st_plamenometalec > 0)
            {
                wave.st_plamenometalec -= 1;
                naredi_zlobneza(ZlobnezTip::Plamenometalec);
            }
            else if (wave.st_buldozer > 0)
            {
                wave.st_buldozer -= 1;
                naredi_zlobneza(ZlobnezTip::Buldozer);
            }

            if (wave.st_vzigalnik == 0 && wave.st_sekira == 0 && wave.st_plamenometalec == 0 && wave.st_buldozer == 0)
            {
                std::cout << "wave spawning koncan\n";
                waves.pop_front();
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

void ZlobnezSpawner::naredi_zlobneza(ZlobnezTip tip)
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

    zlobnezi.emplace_back(tekstura, pozicija, tip);
}

void ZlobnezSpawner::nastavi_wave(int st_vzigalnik, int st_sekira, int st_plamenometalec, int st_buldozer, float cas_spawna)
{
    waves.push_back(Wave{
        st_vzigalnik,
        st_sekira,
        st_plamenometalec,
        st_buldozer,
        cas_spawna,
        0.0f
        });
}

bool ZlobnezSpawner::je_konec_wavov()
{
    return waves.size() == 0 && zlobnezi.size() == 0;
}

void ZlobnezSpawner::shrani(std::ofstream& file)
{
    file.write((const char*)&st_wava, sizeof(st_wava));
    file.write((const char*)&cakanje_wava, sizeof(cakanje_wava));
    file.write((const char*)&cas, sizeof(cas));
    file.write((const char*)&obmocje, sizeof(obmocje));

    int size = waves.size();
    file.write((const char*)&size, sizeof(size));
    for (int i = 0; i < waves.size(); i++)
    {
        file.write((const char*)&waves[i], sizeof(waves[i]));
    }

    size = zlobnezi.size();
    file.write((const char*)&size, sizeof(size));
    for (int i = 0; i < zlobnezi.size(); i++)
    {
        file.write((const char*)&zlobnezi[i].tip, sizeof(zlobnezi[i].tip));
        file.write((const char*)&zlobnezi[i].pozicija, sizeof(zlobnezi[i].pozicija));

        file.write((const char*)&zlobnezi[i].trenutna_anim, sizeof(zlobnezi[i].trenutna_anim));
        file.write((const char*)&zlobnezi[i].flip_x, sizeof(zlobnezi[i].flip_x));
        file.write((const char*)&zlobnezi[i].stevilo_napadov, sizeof(zlobnezi[i].stevilo_napadov));
        file.write((const char*)&zlobnezi[i].zdravje, sizeof(zlobnezi[i].zdravje));
        file.write((const char*)&zlobnezi[i].cas_smrti, sizeof(zlobnezi[i].cas_smrti));
        file.write((const char*)&zlobnezi[i].smer_smrti, sizeof(zlobnezi[i].smer_smrti));
        file.write((const char*)&zlobnezi[i].velikost, sizeof(zlobnezi[i].velikost));
        file.write((const char*)&zlobnezi[i].stanje, sizeof(zlobnezi[i].stanje));
        file.write((const char*)&zlobnezi[i].do_drevesa, sizeof(zlobnezi[i].do_drevesa));
    }
}

void ZlobnezSpawner::nalozi(std::ifstream& file)
{
    file.read((char*)&st_wava, sizeof(st_wava));
    file.read((char*)&cakanje_wava, sizeof(cakanje_wava));
    file.read((char*)&cas, sizeof(cas));
    file.read((char*)&obmocje, sizeof(obmocje));

    int size;
    file.read((char*)&size, sizeof(size));
    waves.resize(size);

    for (int i = 0; i < size; i++)
    {
        file.read((char*)&waves[i], sizeof(waves[i]));
    }

    file.read((char*)&size, sizeof(size));
    zlobnezi.clear();
    zlobnezi.reserve(size);

    for (int i = 0; i < size; i++)
    {
        ZlobnezTip tip;
        glm::vec2 pozicija;
        file.read((char*)&tip, sizeof(tip));
        file.read((char*)&pozicija, sizeof(pozicija));

        zlobnezi.emplace_back(tekstura, pozicija, tip);

        file.read((char*)&zlobnezi[i].trenutna_anim, sizeof(zlobnezi[i].trenutna_anim));
        file.read((char*)&zlobnezi[i].flip_x, sizeof(zlobnezi[i].flip_x));
        file.read((char*)&zlobnezi[i].stevilo_napadov, sizeof(zlobnezi[i].stevilo_napadov));
        file.read((char*)&zlobnezi[i].zdravje, sizeof(zlobnezi[i].zdravje));
        file.read((char*)&zlobnezi[i].cas_smrti, sizeof(zlobnezi[i].cas_smrti));
        file.read((char*)&zlobnezi[i].smer_smrti, sizeof(zlobnezi[i].smer_smrti));
        file.read((char*)&zlobnezi[i].velikost, sizeof(zlobnezi[i].velikost));
        file.read((char*)&zlobnezi[i].stanje, sizeof(zlobnezi[i].stanje));
        file.read((char*)&zlobnezi[i].do_drevesa, sizeof(zlobnezi[i].do_drevesa));
    }
}
