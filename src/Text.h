#pragma once

#include <string_view>
#include <glm/vec2.hpp>

namespace text
{
    void init(const char* font_file);
    void izbrisi();

    void narisi(std::string_view besedilo, glm::vec2 poz, float vel);
    void narisi_centrirano(std::string_view besedilo, glm::vec2 poz, float vel);
}
