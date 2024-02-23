#include "Text.h"
#include "Ostalo.h"
#include "Risalnik.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <fstream>

namespace text
{
    stbtt_bakedchar m_char_data[95];
    Tekstura* m_bitmap = nullptr;

    constexpr size_t VELIKOST_BITMAPA = 1024;
    constexpr float VELIKOST_PISAVE = 128.0f;

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
        float x = 0.0f;
        float y = 0.0f;

        for (int i = 0; i < besedilo.size(); i++)
        {
            stbtt_aligned_quad quad;
            stbtt_GetBakedQuad(m_char_data, VELIKOST_BITMAPA, VELIKOST_BITMAPA, besedilo[i] - ' ', &x, &y, &quad, false);
        }
        //risalnik::narisi_rect(glm::vec3(poz, 0.8f), glm::vec2(0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        poz.x -= x / 2.0f / VELIKOST_PISAVE * vel;
        poz.y -= vel / 2.0f;
        narisi(besedilo, poz, vel);
    }
}
