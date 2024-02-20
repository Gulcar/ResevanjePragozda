#pragma once

#include <vector>
#include <glm/vec2.hpp>

class Tekstura;
class Sprite;

class TileMap
{
public:
    TileMap(const Tekstura* teks, int sirina, int visina);
    void narisi();

    glm::vec2 center = { 0.0f, 0.0f };

    int sirina;
    int visina;

    const Tekstura* tekstura;
    std::vector<Sprite> mozni_tili;
    std::vector<uint8_t> tili;
};
