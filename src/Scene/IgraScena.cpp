#include "IgraScena.h"
#include "../Input.h"
#include "TestScena.h"
#include "../Objekti.h"

void IgraScena::zacetek()
{
    objekti.push_back(std::make_unique<Igralec>(m_vegovec));
}

void IgraScena::posodobi(float delta_time)
{
    if (input::tipka_pritisnjena(GLFW_KEY_SPACE))
        scena::zamenjaj_na(std::make_unique<TestScena>());

    Scena::posodobi(delta_time);
}

void IgraScena::narisi()
{
    //risalnik::narisi_teksturo(m_vegovec, glm::vec3(200.0f, 0.0f, 0.0f), glm::vec2(m_vegovec.sirina(), m_vegovec.visina()) * 4.0f);

    Scena::narisi();
}
