#include "Text.h"
#include "Ostalo.h"
#include "Risalnik.h"
#include "Input.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <fstream>
#include <GLFW/glfw3.h>

namespace text
{
    stbtt_bakedchar m_char_data[95];
    Tekstura* m_bitmap = nullptr;

    constexpr size_t VELIKOST_BITMAPA = 1024;
    constexpr float VELIKOST_PISAVE = 128.0f;

    std::string m_vpis;

    static void char_callback(GLFWwindow* window, unsigned int codepoint)
    {
        if (codepoint >= ' ' && codepoint <= 'z')
            m_vpis.push_back(codepoint);
    }

    void init(const char* font_file)
    {
        std::ifstream file(font_file, std::ios::binary);
        if (file.is_open() == false)
        {
            std::string path = std::string("../") + font_file;
            file = std::ifstream(path, std::ios::binary);
        }
        if (file.is_open() == false)
        {
            std::string path = std::string("../../") + font_file;
            file = std::ifstream(path, std::ios::binary);
        }
        if (file.is_open() == false)
            ERROR_EXIT("ni mogoce odpreti font datoteke %s", font_file);

        file.seekg(0, std::ios::end);
        int fsize = file.tellg();
        file.seekg(0, std::ios::beg);

        unsigned char* ttf_data = new unsigned char[fsize];
        file.read((char*)ttf_data, fsize);
        if (file.fail())
            ERROR_EXIT("branje iz font datoteke ni bilo uspesno %s", font_file);

        unsigned char* pixels = new unsigned char[VELIKOST_BITMAPA * VELIKOST_BITMAPA];

        int res = stbtt_BakeFontBitmap(ttf_data, 0, VELIKOST_PISAVE, pixels, VELIKOST_BITMAPA, VELIKOST_BITMAPA, ' ', 95, m_char_data);

        m_bitmap = new Tekstura(pixels, VELIKOST_BITMAPA, VELIKOST_BITMAPA, true, 1);

        //stbtt_GetBakedQuad()

        delete[] ttf_data;
        delete[] pixels;


        glfwSetCharCallback(risalnik::vrni_okno(), char_callback);
    }

    void izbrisi()
    {
        delete m_bitmap;
    }

    void narisi(std::string_view besedilo, glm::vec2 poz, float vel)
    {
        float x = 0.0f;
        float y = 0.0f;

        for (int i = 0; i < besedilo.size(); i++)
        {
            stbtt_aligned_quad quad;
            stbtt_GetBakedQuad(m_char_data, VELIKOST_BITMAPA, VELIKOST_BITMAPA, besedilo[i] - ' ', &x, &y, &quad, false);

            Sprite spr = m_bitmap->ustvari_sprite(glm::vec2(quad.s0, quad.t1), glm::vec2(quad.s1, quad.t0));

            glm::vec2 poz_crke = poz + glm::vec2(quad.x0 + quad.x1, -(quad.y0 + quad.y1)) / 2.0f / VELIKOST_PISAVE * vel;
            glm::vec2 vel_crke = glm::vec2(quad.x1 - quad.x0, quad.y1 - quad.y0) / VELIKOST_PISAVE * vel;

            risalnik::narisi_sprite(spr, glm::vec3(poz_crke, 0.7f), vel_crke);
        }

        //risalnik::narisi_rect(glm::vec3(poz, 0.8f), glm::vec2(0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    void narisi_centrirano(std::string_view besedilo, glm::vec2 poz, float vel)
    {
        glm::vec2 dim = dimenzije_besedila(besedilo, vel);
        //risalnik::narisi_rect(glm::vec3(poz, 0.8f), glm::vec2(0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        poz.x -= dim.x / 2.0f;
        poz.y -= vel / 2.0f;
        narisi(besedilo, poz, vel);
    }

    void narisi_vpis(std::string* vpis, glm::vec2 poz, float vel)
    {
        for (int i = 0; i < m_vpis.size(); i++)
        {
            vpis->push_back(m_vpis[i]);
        }
        m_vpis.clear();

        if (vpis->size() > 60)
            vpis->resize(60);

        if (input::tipka_pritisnjena(GLFW_KEY_BACKSPACE) && vpis->size() > 0)
            vpis->pop_back();
        if (input::tipka_pritisnjena(GLFW_KEY_BACKSPACE) && input::tipka_drzana(GLFW_KEY_LEFT_CONTROL))
            vpis->clear();

        glm::vec2 dim = dimenzije_besedila(*vpis, vel);
        dim.x += 0.3f;
        risalnik::narisi_rect(glm::vec3(poz.x, poz.y - 0.06f, 0.6f), dim, glm::vec4(0.0f, 0.0f, 0.0f, 0.15f));
        narisi_centrirano(*vpis, poz, vel);
    }

    void pocisti_char_vpis()
    {
        m_vpis.clear();
    }

    bool narisi_gumb(std::string_view besedilo, glm::vec2 poz, float vel)
    {
        glm::vec2 miska = input::pozicija_miske_v_svetu();
        glm::vec2 dim = dimenzije_besedila(besedilo, vel);
        dim.x += 0.3f * vel / 0.35f;
        bool znotraj = je_znotraj(glm::vec2(poz.x, poz.y - 0.06f * vel / 0.35f), dim, miska);
        bool pritisnjena = input::miska_pritisnjena(GLFW_MOUSE_BUTTON_LEFT);
        if (znotraj && !pritisnjena)
        {
            risalnik::narisi_rect(glm::vec3(poz.x, poz.y - 0.06f * vel / 0.35f, 0.6f), dim, glm::vec4(0.0f, 0.0f, 0.0f, 0.3f));
        }
        if (znotraj && pritisnjena)
        {
            risalnik::narisi_rect(glm::vec3(poz.x, poz.y - 0.06f * vel / 0.35f, 0.6f), dim, glm::vec4(1.0f, 1.0f, 1.0f, 0.3f));
        }
        narisi_centrirano(besedilo, poz, vel);
        return znotraj && pritisnjena;
    }

    glm::vec2 dimenzije_besedila(std::string_view besedilo, float vel)
    {
        float x = 0.0f;
        float y = 0.0f;

        for (int i = 0; i < besedilo.size(); i++)
        {
            stbtt_aligned_quad quad;
            stbtt_GetBakedQuad(m_char_data, VELIKOST_BITMAPA, VELIKOST_BITMAPA, besedilo[i] - ' ', &x, &y, &quad, false);
        }

        return glm::vec2(x / VELIKOST_PISAVE * vel, vel);
    }
}
