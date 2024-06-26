#include "Ostalo.h"
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

static bool m_narisi_trkalnike = false;

static glm::vec2 dobi_gradient(int x, int y)
{
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;

    unsigned a = x, b = y;
    a *= 3284157443; b ^= a << s | a >> w - s;
    b *= 1911520717; a ^= b << s | b >> w - s;
    a *= 2048419325;

    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    return glm::vec2(
        std::cos(random),
        std::sin(random)
    );
}

float perlin_noise(float x, float y)
{
    int ix = (int)std::floor(x);
    int iy = (int)std::floor(y);

    float tx = x - ix;
    float ty = y - iy;

    glm::vec2 levo_zgoraj = dobi_gradient(ix, iy);
    glm::vec2 desno_zgoraj = dobi_gradient(ix + 1, iy);
    glm::vec2 levo_spodaj = dobi_gradient(ix, iy + 1);
    glm::vec2 desno_spodaj = dobi_gradient(ix + 1, iy + 1);

    float vrednost_lz = glm::dot(levo_zgoraj, glm::vec2(tx, ty));
    float vrednost_dz = glm::dot(desno_zgoraj, glm::vec2(tx - 1.0f, ty));
    float vrednost_ls = glm::dot(levo_spodaj, glm::vec2(tx, ty - 1.0f));
    float vrednost_ds = glm::dot(desno_spodaj, glm::vec2(tx - 1.0f, ty - 1.0f));

    float smooth_tx = glm::smoothstep(0.0f, 1.0f, tx);
    float smooth_ty = glm::smoothstep(0.0f, 1.0f, ty);

    float vrednost_ly = glm::lerp(vrednost_lz, vrednost_ls, smooth_ty);
    float vrednost_dy = glm::lerp(vrednost_dz, vrednost_ds, smooth_ty);

    float vrednost = glm::lerp(vrednost_ly, vrednost_dy, smooth_tx);
    return vrednost;
}

bool se_prekrivata(glm::vec2 poz_a, glm::vec2 vel_a, glm::vec2 poz_b, glm::vec2 vel_b)
{
    if (std::abs(poz_a.x - poz_b.x) < (vel_a.x + vel_b.x) / 2.0f &&
        std::abs(poz_a.y - poz_b.y) < (vel_a.y + vel_b.y) / 2.0f)
    {
        return true;
    }
    return false;
}

bool je_znotraj(glm::vec2 rect_poz, glm::vec2 rect_vel, glm::vec2 tocka)
{
    if (tocka.x >= rect_poz.x - rect_vel.x / 2.0f &&
        tocka.y >= rect_poz.y - rect_vel.y / 2.0f &&
        tocka.x <= rect_poz.x + rect_vel.x / 2.0f &&
        tocka.y <= rect_poz.y + rect_vel.y / 2.0f)
    {
        return true;
    }
    return false;
}

bool narisi_trkalnike()
{
    return m_narisi_trkalnike;
}

void set_risanje_trkalnikov(bool risanje)
{
    m_narisi_trkalnike = risanje;
}

float randf()
{
    return rand() / (float)RAND_MAX;
}

glm::vec2 vec2_iz_kota(float kot)
{
    return glm::vec2(
        glm::cos(kot),
        glm::sin(kot)
    );
}

