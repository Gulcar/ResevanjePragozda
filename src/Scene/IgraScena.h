#pragma once

#include "Scena.h"
#include "../Risalnik.h"
#include "../Objekti.h"

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
    Tekstura m_tzlobnezi = Tekstura("assets/zlobnezi.png");

    Igralec m_igralec = Igralec(&m_tvegovec);

    TileMap m_tile_map = TileMap(&m_tgozd, 140, 100);
    Gozd m_gozd_zunanji = Gozd(&m_tgozd, glm::vec2(180.0f, 140.0f), 40.0f, false);
    Gozd m_gozd_notranji = Gozd(&m_tgozd, glm::vec2(30.0f, 30.0f), 10.0f, true);

    ZlobnezSpawner m_spawner = ZlobnezSpawner(&m_tzlobnezi, glm::vec2(140.0f, 100.0f));
};
