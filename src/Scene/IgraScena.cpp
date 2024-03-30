#include "IgraScena.h"
#include "MeniScena.h"
#include "../Input.h"
#include "../Text.h"
#include "../Ostalo.h"
#include "../Objekti/MiniMap.h"
#include <iostream>
#include <string>

IgraScena::IgraScena(const std::string& ime_igralca, int level)
{
    m_igralec.ime = ime_igralca;
    m_level = "LEVEL " + std::to_string(level);

    if (level == 1)
    {
        /*
        m_spawner.nastavi_wave(5, 0, 0, 0, 1.0f);
        m_spawner.nastavi_wave(7, 2, 1, 0, 1.0f);
        m_spawner.nastavi_wave(8, 4, 3, 1, 1.0f);
        */
        std::cout << "izbran level 1\n";
    }
    else if (level == 2)
    {
        m_spawner.nastavi_wave(10, 0, 0, 0, 1.0f);
        m_spawner.nastavi_wave(14, 4, 4, 0, 1.0f);
        m_spawner.nastavi_wave(10, 10, 10, 0, 1.0f);
        m_spawner.nastavi_wave(10, 10, 10, 3, 1.0f);
        std::cout << "izbran level 2\n";
    }
    else if (level == 3)
    {
        m_spawner.nastavi_wave(10, 0, 0, 0, 0.5f);
        m_spawner.nastavi_wave(14, 4, 4, 0, 0.5f);
        m_spawner.nastavi_wave(10, 10, 10, 0, 0.5f);
        m_spawner.nastavi_wave(10, 10, 10, 3, 0.5f);
        m_spawner.nastavi_wave(10, 20, 20, 6, 0.5f);
        m_spawner.nastavi_wave(10, 30, 30, 10, 0.5f);
        std::cout << "izbran level 3\n";
    }
}

void IgraScena::zacetek()
{
    risalnik::nastavi_visino_perspektive(30.0f);

    //m_spawner.nastavi_wave(0, 0, 0, 5, 0.1f);

    m_igralec.pozicija = { 0.0f, -15.0f };

    std::cout << "stevilo zunanjih dreves: " << m_gozd_zunanji.drevesa.size() << "\n";
    std::cout << "stevilo notranjih dreves: " << m_gozd_notranji.drevesa.size() << "\n";

    m_pomocniki.emplace_back(&m_tdomorodci, 0, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    m_pomocniki.emplace_back(&m_tdomorodci, 1, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    m_pomocniki.emplace_back(&m_tdomorodci, 2, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    m_pomocniki.emplace_back(&m_tdomorodci, 3, vec2_iz_kota(randf() * PI * 2) * 15.0f, 5.0f);
    m_pomocniki.emplace_back(&m_tdomorodci, 4, vec2_iz_kota(randf() * PI * 2) * 15.0f, 5.0f);
}

void IgraScena::posodobi(float delta_time)
{
    if (m_cas_prev_delta <= 0.0f)
    {
        m_prev_delta = delta_time;
        m_cas_prev_delta = 0.5f;
    }
    m_cas_prev_delta -= delta_time;

    m_cas += delta_time;

    /*
    if (input::tipka_pritisnjena(GLFW_KEY_P))
        scena::zamenjaj_na(std::make_unique<TestScena>());
    if (input::tipka_pritisnjena(GLFW_KEY_R))
        scena::zamenjaj_na(std::make_unique<IgraScena>());


    // TODO: odstrani zoom
    if (input::tipka_pritisnjena(GLFW_KEY_M))
        risalnik::nastavi_visino_perspektive(30.0f * 3.0f);
    if (input::tipka_spuscena(GLFW_KEY_M))
        risalnik::nastavi_visino_perspektive(30.0f);
    */

    m_igralec.posodobi(delta_time, m_spawner.zlobnezi, m_gozd_notranji);
    m_spawner.posodobi(delta_time, &m_igralec, m_gozd_notranji);

    m_gozd_notranji.posodobi(delta_time);

    for (auto& pomocnik : m_pomocniki)
        pomocnik.posodobi(delta_time, m_spawner.zlobnezi);

    Pomocnik::daj_narazen(m_pomocniki);

    if (!m_zmaga && m_spawner.je_konec_wavov() && m_gozd_notranji.vsi_ognji_pogaseni())
    {
        m_zmaga = true;
        m_st_tock = (int)std::max(600.0f - m_cas, 0.0f) + m_gozd_notranji.drevesa.size() * 7;
    }

    if (m_zmaga && input::tipka_pritisnjena(GLFW_KEY_ENTER))
    {
        scena::zamenjaj_na(std::make_unique<MeniScena>());
    }
}

void IgraScena::narisi()
{
    m_tile_map.narisi();
    m_gozd_zunanji.narisi();
    m_gozd_notranji.narisi();

    m_spawner.narisi();
    m_igralec.narisi();

    for (auto& pomocnik : m_pomocniki)
        pomocnik.narisi();

    //risalnik::narisi_teksturo(m_tdomorodci, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(3.0f * 5.0f, 3.0f * 3.0f));

    //text::narisi("Pozdravljen svet", glm::vec2(15.0f, -1.0f), 1.0f);
    //text::narisi_centrirano("Pozdravljen svet", glm::vec2(15.0f, -5.0f), 1.0f);

    glm::vec2 vidno = risalnik::velikost_vidnega();
    glm::vec2 poz_levo_gor = glm::vec2(-vidno.x / 2.0f + 0.4f, vidno.y / 2.0f - 1.2f) + risalnik::dobi_pozicijo_kamere();
    text::narisi(m_level, poz_levo_gor, 1.0f);
    poz_levo_gor.y -= 1.0f;
    text::narisi(std::to_string(m_spawner.st_wava) + ". VAL SOVRAZNIKOV", poz_levo_gor, 1.0f);

    poz_levo_gor.y -= 1.0f;
    char buf[48];
    snprintf(buf, sizeof(buf), "%.0ffps %.2fms", 1.0f / m_prev_delta, m_prev_delta * 1000.0f);
    text::narisi(buf, poz_levo_gor, 1.0f);

    if (m_zmaga)
    {
        glm::vec2 poz = risalnik::dobi_pozicijo_kamere();
        risalnik::narisi_rect(glm::vec3(poz, 0.5f), risalnik::velikost_vidnega(), glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        text::narisi_centrirano("ZMAGA", glm::vec2(poz.x, poz.y + 3.0f), 3.0f);
        snprintf(buf, sizeof(buf), "stevilo tock: %d", m_st_tock);
        text::narisi_centrirano(buf, glm::vec2(poz.x, poz.y), 2.0f);
        text::narisi_centrirano("pritisni enter", glm::vec2(poz.x, poz.y - 2.0f), 1.5f);
    }

    minimap::narisi_ozadje();
    minimap::narisi_igralca(m_igralec.pozicija);

    for (const auto& pomagac : m_pomocniki)
        minimap::narisi_pomagaca(pomagac.pozicija);

    for (const auto& zlobnez : m_spawner.zlobnezi)
    {
        if (zlobnez.tip == ZlobnezTip::Buldozer)
            minimap::narisi_velikega_zlobneza(zlobnez.pozicija);
        else
            minimap::narisi_zlobneza(zlobnez.pozicija);
    }

    for (const auto& drevo : m_gozd_notranji.drevesa)
    {
        if (drevo.cas_ognja > 0.0f)
            minimap::narisi_ogenj(drevo.pozicija);
        minimap::narisi_pomembno_drevo(drevo.pozicija);
    }

    for (const auto& drevo : m_gozd_zunanji.drevesa)
    {
        if (drevo.pozicija.x > -70.5f && drevo.pozicija.x < 70.5f &&
            drevo.pozicija.y > -50.5f && drevo.pozicija.y < 50.5f)
        {
            minimap::narisi_drevo(drevo.pozicija);
        }
    }
}
