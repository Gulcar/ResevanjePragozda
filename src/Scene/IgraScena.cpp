#include "IgraScena.h"
#include "../Input.h"
#include "TestScena.h"
#include "../Objekti.h"

void IgraScena::zacetek()
{
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
    m_igralec.narisi();
}
