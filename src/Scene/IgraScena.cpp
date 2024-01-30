#include "IgraScena.h"
#include "../Input.h"
#include "TestScena.h"
#include "../Objekti.h"
#include <iostream>

void IgraScena::zacetek()
{
    risalnik::nastavi_visino_perspektive(20.0f);

    //m_spawner.nastavi_wave(3000, 0.001f);

    m_spawner.nastavi_wave(5, 1.0f);
    m_spawner.nastavi_wave(15, 1.0f);
    m_spawner.nastavi_wave(25, 1.0f);

    m_igralec.pozicija = { 0.0f, -15.0f };

    std::cout << "stevilo zunanjih dreves: " << m_gozd_zunanji.drevesa.size() << "\n";
    std::cout << "stevilo notranjih dreves: " << m_gozd_notranji.drevesa.size() << "\n";
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
    m_spawner.posodobi(delta_time, &m_igralec);
}

void IgraScena::narisi()
{
    m_tile_map.narisi();
    m_gozd_zunanji.narisi();
    m_gozd_notranji.narisi();

    m_spawner.narisi();
    m_igralec.narisi();

    //risalnik::narisi_teksturo(m_tdomorodci, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(3.0f * 5.0f, 3.0f * 3.0f));
}
