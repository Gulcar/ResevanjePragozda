#pragma once

#include "Scena.h"
#include "../Risalnik.h"
#include "../Objekti.h"
#include <vector>

class IgraScena : public Scena
{
public:
    void zacetek() override;
    void posodobi(float delta_time) override;
    void narisi() override;

private:
    Tekstura m_tvegovec = Tekstura("assets/vegovec.png");
    Tekstura m_tgozd = Tekstura("assets/gozd.png");
    Tekstura m_tdomorodci = Tekstura("assets/domorodci.png");

    Igralec m_igralec = Igralec(&m_tvegovec);

    TileMap m_tile_map = TileMap(&m_tgozd, 140, 100);
    Gozd m_gozd = Gozd(&m_tgozd, glm::vec2(140.0f, 100.0f));

    ZlobnezSpawner m_spawner = ZlobnezSpawner(&m_tdomorodci, glm::vec2(140.0f, 100.0f));
};
