#pragma once

#include <glm/glm.hpp>

namespace risalnik
{

void ustvari_okno();
bool je_okno_odprto();

void zacni_frame();
void koncaj_frame();

void narisi_rect(glm::vec3 pozicija, glm::vec2 velikost, glm::vec4 barva = glm::vec4(1.0f));
void narisi_crto(glm::vec3 a, glm::vec3 b, glm::vec4 barva = glm::vec4(1.0f));

void terminate();

}

