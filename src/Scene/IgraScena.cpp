#include "IgraScena.h"
#include "../Input.h"
#include "TestScena.h"

void IgraScena::posodobi(float delta_time)
{
    if (input::tipka_pritisnjena(GLFW_KEY_SPACE))
        scena::zamenjaj_na(std::make_unique<TestScena>());

    risalnik::narisi_teksturo(m_vegovec, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(250.0f));
}
