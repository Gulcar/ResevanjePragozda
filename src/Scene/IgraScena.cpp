#include "IgraScena.h"
#include "../Input.h"
#include "TestScena.h"
#include "../Objekti.h"
#include "../Ostalo.h"

void IgraScena::zacetek()
{
    risalnik::nastavi_visino_perspektive(15.0f * 7.0f);
}

void IgraScena::posodobi(float delta_time)
{
    if (input::tipka_pritisnjena(GLFW_KEY_P))
        scena::zamenjaj_na(std::make_unique<TestScena>());

    m_igralec.posodobi(delta_time);
}

void IgraScena::narisi()
{
    m_tile_map.narisi();
    m_gozd.narisi();
    m_igralec.narisi();

    risalnik::narisi_teksturo(m_tdomorodci, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(3.0f * 5.0f, 3.0f * 3.0f));
}
