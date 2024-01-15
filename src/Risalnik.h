#pragma once

#include <glm/glm.hpp>

// del teksture
class Sprite
{
public:
    const class Tekstura* tekstura() const { return m_tekstura; }
    glm::vec2 min_uv() const { return m_min_uv; }
    glm::vec2 max_uv() const { return m_max_uv; }

private:
    const class Tekstura* m_tekstura;
    glm::vec2 m_min_uv, m_max_uv;

    friend class Tekstura;
};

class Tekstura
{
public:
    //Tekstura() { m_render_id = -1; m_sirina = 0; m_visina = 0; }
    Tekstura(const char* filepath, bool nearest = true);
    Tekstura(const uint8_t* pixli, int sirina, int visina, bool nearest = true);
    ~Tekstura();

    Sprite ustvari_sprite(int tile_x, int tile_y, int velikost_tilov = 32) const;

    uint32_t render_id() const { return m_render_id; };
    int sirina() const { return m_sirina; }
    int visina() const { return m_visina; }

private:
    uint32_t m_render_id;
    int m_sirina, m_visina;
};

struct GLFWwindow;

namespace risalnik
{

void ustvari_okno(const char* naslov, int sirina, int visina);
bool je_okno_odprto();
GLFWwindow* vrni_okno();

// vrne delta time
float zacni_frame();
void koncaj_frame();

void set_camera_pos(glm::vec2 pozicija);
// nastavi koliko enot bo vidno na y
void set_camera_zoom_height(float h);

void narisi_rect(glm::vec3 pozicija, glm::vec2 velikost, glm::vec4 barva);
void narisi_teksturo(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h = false, glm::vec4 barva = glm::vec4(1.0f));
void narisi_sprite(const Sprite& sprite, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h = false, glm::vec4 barva = glm::vec4(1.0f));

void narisi_crto(glm::vec3 a, glm::vec3 b, glm::vec4 barva = glm::vec4(1.0f));

void terminate();

}

