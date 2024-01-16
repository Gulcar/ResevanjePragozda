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
    Tekstura m_vegovec = Tekstura("assets/vegovec.png");
    Tekstura m_gozd = Tekstura("assets/gozd.png");
    Tekstura m_domorodci = Tekstura("assets/domorodci.png");

    Igralec m_igralec = Igralec(&m_vegovec);
    TileMap m_tile_map = TileMap(&m_gozd, 44, 44);

    std::vector<Sprite> m_drevesa;
};
