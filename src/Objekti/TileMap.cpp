#include "TileMap.h"
#include "../Risalnik.h"

TileMap::TileMap(const Tekstura* teks, int sirina, int visina)
    : tekstura(teks)
{
    for (int y = 1; y <= 3; y++)
    {
        for (int x = 1; x <= 2; x++)
            mozni_tili.push_back(teks->ustvari_sprite(x, y, 16));
    }

    this->sirina = sirina;
    this->visina = visina;

    tili.resize(sirina * visina);

    for (int y = 0; y < visina; y++)
    {
        for (int x = 0; x < sirina; x++)
        {
            tili[x + y * sirina] = rand() % mozni_tili.size();
        }
    }
}

void TileMap::narisi()
{
    for (int y = 0; y < visina; y++)
    {
        for (int x = 0; x < sirina; x++)
        {
            const Sprite& sprite = mozni_tili[tili[x + y * sirina]];
            glm::vec3 poz = glm::vec3(center.x + x - sirina / 2.0f, center.y + y - visina / 2.0f, -0.5f);
            risalnik::narisi_sprite(sprite, poz, glm::vec2(1.01f));
        }
    }
}
