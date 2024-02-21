#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace minimap
{
    void narisi_ozadje();

    void narisi_zlobneza(glm::vec2 poz);
    void narisi_velikega_zlobneza(glm::vec2 poz);

    void narisi_igralca(glm::vec2 poz);
    void narisi_pomagaca(glm::vec2 poz);

    void narisi_ogenj(glm::vec2 poz);
    void narisi_drevo(glm::vec2 poz);
    void narisi_pomembno_drevo(glm::vec2 poz);

    void narisi_kvadratek(glm::vec2 poz, glm::vec2 vel, glm::vec4 barva);
    glm::vec2 ugotovi_mini_lokacijo(glm::vec2 poz);
}
