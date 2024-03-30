#pragma once

#include <string_view>
#include <glm/vec2.hpp>

namespace text
{
    void init(const char* font_file);
    void izbrisi();

    void narisi(std::string_view besedilo, glm::vec2 poz, float vel);
    void narisi_centrirano(std::string_view besedilo, glm::vec2 poz, float vel);

    void narisi_vpis(std::string* vpis, glm::vec2 poz, float vel);
    void pocisti_char_vpis();
    bool narisi_gumb(std::string_view besedilo, glm::vec2 poz, float vel);

    glm::vec2 dimenzije_besedila(std::string_view besedilo, float vel);
}
