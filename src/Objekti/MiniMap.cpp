#include "MiniMap.h"
#include "../Risalnik.h"

namespace minimap
{
    glm::vec2 mini_map_vel;
    glm::vec2 mini_map_poz;

    void narisi_ozadje()
    {
        mini_map_vel = glm::vec2(14.0f, 10.0f) / 1.3f;
        mini_map_poz = risalnik::dobi_pozicijo_kamere() + risalnik::velikost_vidnega() / 2.0f - mini_map_vel / 2.0f - glm::vec2(1.0f);
        risalnik::narisi_rect(glm::vec3(mini_map_poz, 0.8f), mini_map_vel, glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));
    }

    void narisi_zlobneza(glm::vec2 poz)
    {
        narisi_kvadratek(poz, glm::vec2(0.2f), glm::vec4(0.807f, 0.003f, 0.082f, 1.0f));
    }

    void narisi_velikega_zlobneza(glm::vec2 poz)
    {
        narisi_kvadratek(poz, glm::vec2(0.35f), glm::vec4(0.807f, 0.003f, 0.082f, 1.0f));
    }

    void narisi_igralca(glm::vec2 poz)
    {
        narisi_kvadratek(poz, glm::vec2(0.2f), glm::vec4(0.415f, 0.945f, 0.188f, 1.0f));
    }

    void narisi_pomagaca(glm::vec2 poz)
    {
        narisi_kvadratek(poz, glm::vec2(0.2f), glm::vec4(0.372f, 0.803f, 0.894f, 1.0f));
    }

    void narisi_ogenj(glm::vec2 poz)
    {
        narisi_kvadratek(poz, glm::vec2(0.2f), glm::vec4(1.0f, 0.682f, 0.0f, 1.0f));
    }

    void narisi_drevo(glm::vec2 poz)
    {
        narisi_kvadratek(poz, glm::vec2(0.2f), glm::vec4(0.2f, 0.38f, 0.082f, 1.0f));
    }

    void narisi_pomembno_drevo(glm::vec2 poz)
    {
        narisi_kvadratek(poz, glm::vec2(0.2f), glm::vec4(0.251f, 0.478f, 0.098f, 1.0f));
    }

    void narisi_kvadratek(glm::vec2 poz, glm::vec2 vel, glm::vec4 barva)
    {
        glm::vec2 mini_poz = ugotovi_mini_lokacijo(poz);
        risalnik::narisi_rect(glm::vec3(mini_poz, 0.81f), vel, barva);
    }

    glm::vec2 ugotovi_mini_lokacijo(glm::vec2 poz)
    {
        float tx = (poz.x) / 140.0f;
        float ty = (poz.y) / 100.0f;
        return mini_map_poz + glm::vec2(mini_map_vel.x * tx, mini_map_vel.y * ty);
    }
}
