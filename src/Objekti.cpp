#include "Objekti.h"
#include "Risalnik.h"
#include "Input.h"

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

        trenutna_animacija = &hoja;
        if (premik.x < 0.0f) flip_h = true;
        else if (premik.x > 0.0f) flip_h = false;
    }
    else
    {
        trenutna_animacija = &primiru;
    }
}

void Igralec::narisi()
{
    trenutna_animacija->narisi(tekstura, glm::vec3(pozicija, 0.0f), glm::vec2(200.0f), flip_h);
}
