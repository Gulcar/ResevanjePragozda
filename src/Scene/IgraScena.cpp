#include "IgraScena.h"
#include "TestScena.h"
#include "../Input.h"
#include "../Ostalo.h"
#include "../Objekti/MiniMap.h"
#include <iostream>

void IgraScena::zacetek()
{
    risalnik::nastavi_visino_perspektive(30.0f);

    //m_spawner.nastavi_wave(0, 0, 0, 5, 0.1f);

    m_spawner.nastavi_wave(5, 0, 0, 0, 1.0f);
    m_spawner.nastavi_wave(7, 2, 1, 0, 1.0f);
    m_spawner.nastavi_wave(10, 5, 4, 1, 1.0f);
    m_spawner.nastavi_wave(20, 10, 7, 3, 1.0f);
    m_spawner.nastavi_wave(20, 15, 10, 6, 1.0f);
    m_spawner.nastavi_wave(20, 20, 20, 10, 1.0f);
    m_spawner.nastavi_wave(60, 60, 60, 60, 0.3f);

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
    if (input::tipka_pritisnjena(GLFW_KEY_P))
        scena::zamenjaj_na(std::make_unique<TestScena>());
    if (input::tipka_pritisnjena(GLFW_KEY_R))
        scena::zamenjaj_na(std::make_unique<IgraScena>());

    // TODO: odstrani zoom
    if (input::tipka_pritisnjena(GLFW_KEY_M))
        risalnik::nastavi_visino_perspektive(30.0f * 3.0f);
    if (input::tipka_spuscena(GLFW_KEY_M))
        risalnik::nastavi_visino_perspektive(30.0f);

    m_igralec.posodobi(delta_time, m_spawner.zlobnezi, m_gozd_notranji);
    m_spawner.posodobi(delta_time, &m_igralec, m_gozd_notranji);

    m_gozd_notranji.posodobi(delta_time);

    for (auto& pomocnik : m_pomocniki)
        pomocnik.posodobi(delta_time, m_spawner.zlobnezi);

    Pomocnik::daj_narazen(m_pomocniki);
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
