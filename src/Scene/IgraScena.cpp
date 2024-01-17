#include "IgraScena.h"
#include "../Input.h"
#include "TestScena.h"
#include "../Objekti.h"
#include "../Ostalo.h"

void IgraScena::zacetek()
{
    m_drevesa.push_back(m_gozd.ustvari_sprite(0, 4, 16, 2, 4));
    m_drevesa.push_back(m_gozd.ustvari_sprite(2, 4, 16, 2, 4));
    m_drevesa.push_back(m_gozd.ustvari_sprite(4, 4, 16, 2, 4));
    m_drevesa.push_back(m_gozd.ustvari_sprite(6, 4, 16, 2, 4));
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

    risalnik::narisi_teksturo(m_domorodci, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(3.0f * 5.0f, 3.0f * 3.0f));

    for (int i = 0; i < 10; i++)
        risalnik::narisi_sprite(m_drevesa[i % m_drevesa.size()], glm::vec3(8.0f + i * 2.0f, 0.0f, 1.5f / 10000.0f), glm::vec2(3.0f, 6.0f));

    for (int y = 0; y < 200; y++)
    {
        for (int x = 0; x < 200; x++)
        {
            float perlin = perlin_noise(x * 0.1f, y * 0.1f);
            perlin = (perlin + 1.0f) / 2.0f;
            risalnik::narisi_rect(glm::vec3(x * 0.1f, y * 0.1f, 0.9f), glm::vec2(0.1001f), glm::vec4(perlin, perlin, perlin, 1.0f));
        }
    }
}
