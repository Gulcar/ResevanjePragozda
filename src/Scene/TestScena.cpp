#include "TestScena.h"
#include "IgraScena.h"
#include "../Input.h"

void TestScena::zacetek()
{
    risalnik::nastavi_visino_perspektive(5.0f);
    risalnik::nastavi_pozicijo_kamere(glm::vec2(2.0f, 0.5f));
}

void TestScena::posodobi(float delta_time)
{
    /*
    if (input::tipka_pritisnjena(GLFW_KEY_P))
        scena::zamenjaj_na(std::make_unique<IgraScena>());
    */
}

void TestScena::narisi()
{
    risalnik::narisi_sprite(m_rozica, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    risalnik::narisi_sprite(m_grm, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    risalnik::narisi_sprite(m_trava, glm::vec3(3.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    risalnik::narisi_sprite(m_stor, glm::vec3(4.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    risalnik::narisi_rect(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(3.0f, 1.0f), glm::vec4(0.1f, 0.1f, 0.9f, 1.0f));
    risalnik::narisi_rect(glm::vec3(3.0f, 2.0f, 0.0f), glm::vec2(0.5f, 0.5f), glm::vec4(0.9f, 0.1f, 0.1f, 1.0f));
}
