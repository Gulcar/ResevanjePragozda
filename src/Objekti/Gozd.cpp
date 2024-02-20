#include "Gozd.h"
#include "../Ostalo.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

void Drevo::posodobi(float delta_time)
{
    if (cas_ognja > 0.0f)
        cas_ognja += delta_time;
}

void Drevo::narisi(const Tekstura* togenj)
{
    glm::vec3 crnina = glm::vec3((20.0f - cas_ognja) / 20.0f);

    risalnik::narisi_sprite(sprite, glm::vec3(pozicija, (-pozicija.y + 1.5f) / 10000.0f), glm::vec2(3.0f, 6.0f), false, glm::vec4(crnina, 1.0f));

    int stevilo_ogenckov = (int)std::ceil(cas_ognja / 1.25f);
    for (int i = 0; i < stevilo_ogenckov; i++)
    {
        int x = i + (int)(cas_ognja / 0.1f);
        Sprite spr = togenj->ustvari_sprite(x % 4, 0, 16);

        glm::vec2 poz = pozicija;
        poz.y += i * 0.2f - 2.6f;

        float poz_za_x[] = { 0.2f, -0.2f, 0.0f };
        poz.x += poz_za_x[i % 3];

        risalnik::narisi_sprite(spr, glm::vec3(poz, (-pozicija.y + 1.6f) / 10000.0f), glm::vec2(0.75f));
    }
}

Gozd::Gozd(const Tekstura* teks, const Tekstura* ogenj, glm::vec2 obmocje, float radij_sredine, bool znotraj)
{
    tekstura = teks;
    togenj = ogenj;

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

void Gozd::posodobi(float delta_time)
{
    for (int i = 0; i < drevesa.size(); i++)
    {
        drevesa[i].posodobi(delta_time);

        if (drevesa[i].cas_ognja > 20.0f)
        {
            std::swap(drevesa[i], drevesa.back());
            drevesa.pop_back();
            i--;
        }
    }
}

void Gozd::narisi()
{
    for (auto& drevo : drevesa)
        drevo.narisi(togenj);
}

glm::vec2 Gozd::najblizje_drevo(glm::vec2 poz) const
{
    glm::vec2 najblizje = glm::vec2(0.0f);
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

void Gozd::zaneti_ogenj(glm::vec2 poz)
{
    for (int i = 0; i < drevesa.size(); i++)
    {
        if (glm::distance2(drevesa[i].pozicija, poz) < 1.0f)
        {
            if (drevesa[i].cas_ognja == 0.0f)
                drevesa[i].cas_ognja = 0.0001f;
            break;
        }
    }
}
