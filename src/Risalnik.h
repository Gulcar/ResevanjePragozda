#pragma once

#include <glm/glm.hpp>

class Tekstura
{
public:
    //Tekstura() { m_render_id = -1; m_sirina = 0; m_visina = 0; }
    Tekstura(const char* filepath, bool nearest = true);
    Tekstura(const uint8_t* pixli, int sirina, int visina, bool nearest = true);
    ~Tekstura();

    uint32_t render_id() const { return m_render_id; };

private:
    uint32_t m_render_id;
    int m_sirina, m_visina;
};

namespace risalnik
{

void ustvari_okno(const char* naslov, int sirina, int visina);
bool je_okno_odprto();

void zacni_frame();
void koncaj_frame();

void set_camera_pos(glm::vec2 pozicija);

void narisi_rect(glm::vec3 pozicija, glm::vec2 velikost, glm::vec4 barva);
void narisi_sprite(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, glm::vec4 barva = glm::vec4(1.0f));
void narisi_crto(glm::vec3 a, glm::vec3 b, glm::vec4 barva = glm::vec4(1.0f));

void terminate();

}

