#include "IgraScena.h"
#include "TestScena.h"
#include "../Input.h"
#include "../Objekti.h"
#include "../Ostalo.h"
#include <iostream>

void IgraScena::zacetek()
{
    risalnik::nastavi_visino_perspektive(20.0f);

    //m_spawner.nastavi_wave(3000, 0.001f);

    m_spawner.nastavi_wave(5, 0, 0, 0, 1.0f);
    m_spawner.nastavi_wave(7, 2, 1, 0, 1.0f);
    m_spawner.nastavi_wave(10, 5, 4, 1, 1.0f);

    m_igralec.pozicija = { 0.0f, -15.0f };

    std::cout << "stevilo zunanjih dreves: " << m_gozd_zunanji.drevesa.size() << "\n";
    std::cout << "stevilo notranjih dreves: " << m_gozd_notranji.drevesa.size() << "\n";

    pomocniki.emplace_back(&m_tdomorodci, 0, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    pomocniki.emplace_back(&m_tdomorodci, 1, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    pomocniki.emplace_back(&m_tdomorodci, 2, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    pomocniki.emplace_back(&m_tdomorodci, 3, vec2_iz_kota(randf() * PI * 2) * 15.0f, 5.0f);
    pomocniki.emplace_back(&m_tdomorodci, 4, vec2_iz_kota(randf() * PI * 2) * 15.0f, 5.0f);
}

void IgraScena::posodobi(float delta_time)
{
    if (input::tipka_pritisnjena(GLFW_KEY_P))
        scena::zamenjaj_na(std::make_unique<TestScena>());
    if (input::tipka_pritisnjena(GLFW_KEY_R))
        scena::zamenjaj_na(std::make_unique<IgraScena>());

    if (input::tipka_pritisnjena(GLFW_KEY_M))
        risalnik::nastavi_visino_perspektive(20.0f * 4.0f);
    if (input::tipka_spuscena(GLFW_KEY_M))
        risalnik::nastavi_visino_perspektive(20.0f);

    m_igralec.posodobi(delta_time, m_spawner.zlobnezi);
    m_spawner.posodobi(delta_time, &m_igralec, m_gozd_notranji);

    for (auto& pomocnik : pomocniki)
        pomocnik.posodobi(delta_time, m_spawner.zlobnezi);

    Pomocnik::daj_narazen(pomocniki);
}

void IgraScena::narisi()
{
    m_tile_map.narisi();
    m_gozd_zunanji.narisi();
    m_gozd_notranji.narisi();

    m_spawner.narisi();
    m_igralec.narisi();

    for (auto& pomocnik : pomocniki)
        pomocnik.narisi();

    //risalnik::narisi_teksturo(m_tdomorodci, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(3.0f * 5.0f, 3.0f * 3.0f));
}
