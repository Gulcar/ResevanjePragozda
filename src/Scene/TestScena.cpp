#include "TestScena.h"
#include "IgraScena.h"
#include "../Input.h"

void TestScena::posodobi(float delta_time)
{
    if (input::miska_spuscena(GLFW_MOUSE_BUTTON_RIGHT))
        scena::zamenjaj_na(std::make_unique<IgraScena>());
}

void TestScena::narisi()
{
    risalnik::narisi_sprite(m_rozica, glm::vec3(100.0f, 0.0f, 0.0f), glm::vec2(100.0f, 100.0f));
    risalnik::narisi_sprite(m_grm, glm::vec3(200.0f, 0.0f, 0.0f), glm::vec2(100.0f, 100.0f));
    risalnik::narisi_sprite(m_trava, glm::vec3(300.0f, 0.0f, 0.0f), glm::vec2(100.0f, 100.0f));
    risalnik::narisi_sprite(m_stor, glm::vec3(400.0f, 0.0f, 0.0f), glm::vec2(100.0f, 100.0f));
    risalnik::narisi_rect(glm::vec3(0.0f, -100.0f, 0.0f), glm::vec2(300.0f, 100.0f), glm::vec4(0.1f, 0.1f, 0.9f, 1.0f));
    risalnik::narisi_rect(glm::vec3(300.0f, 200.0f, 0.0f), glm::vec2(50.0f, 50.0f), glm::vec4(0.9f, 0.1f, 0.1f, 1.0f));
}
