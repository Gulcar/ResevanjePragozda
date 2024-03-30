#include "Igralec.h"
#include "Zlobnez.h"
#include "Gozd.h"
#include "../Input.h"
#include "../Risalnik.h"
#include "../Text.h"
#include "../Ostalo.h"
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

Igralec::Igralec(const Tekstura* vegovec, const Tekstura* voda)
    : tvegovec(vegovec), tvoda(voda)
{
    animacije[0] = Animacija(2, 0, 1, 1.0f); // primiru
    animacije[1] = Animacija(0, 1, 8, 0.100f); // hoja
    animacije[2] = Animacija(4, 2, 4, 0.100f); // hoja simple
    trenutna_anim = 0;

    voda_anim = Animacija(0, 0, 4, 0.070f, true, 80);
}

void Igralec::posodobi(float delta_time, std::vector<Zlobnez>& zlobnezi, Gozd& gozd)
{
    animacije[trenutna_anim].posodobi(delta_time);
    voda_anim.posodobi(delta_time);

    glm::vec2 premik;
    premik.x = input::os_tipk(GLFW_KEY_A, GLFW_KEY_D);
    premik.y = input::os_tipk(GLFW_KEY_S, GLFW_KEY_W);

    if (premik.x != 0.0f || premik.y != 0.0f)
    {
        premik = glm::normalize(premik);
        pozicija += premik * HITROST_IGRALCA * delta_time;

        trenutna_anim = 2;
        if (premik.x < 0.0f) flip_h = true;
        else if (premik.x > 0.0f) flip_h = false;
    }
    else
    {
        trenutna_anim = 0;
    }

    pozicija.x = glm::clamp(pozicija.x, -41.0f, 41.0f);
    pozicija.y = glm::clamp(pozicija.y, -33.5f, 33.5f);

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
                zlobnezi[i].smer_smrti = !flip_h;
            }
        }

        for (auto& drevo : gozd.drevesa)
        {
            if (drevo.cas_ognja != 0.0f && se_prekrivata(pozicija_vode, velikost, drevo.pozicija, glm::vec2(1.0f, 3.1f)))
            {
                drevo.cas_ognja -= 4.0f * delta_time;
                if (drevo.cas_ognja < 0.0f)
                    drevo.cas_ognja = 0.0f;
            }
        }
    }
}

void Igralec::narisi()
{
    animacije[trenutna_anim].narisi(*tvegovec, glm::vec3(pozicija, -pozicija.y / 10000.0f), glm::vec2(3.0f), flip_h);

    text::narisi_centrirano(ime, glm::vec3(pozicija.x, pozicija.y + 1.2f, -(pozicija.y + 1.2f) / 10000.0f), 0.6f);

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
