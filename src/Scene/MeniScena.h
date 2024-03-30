#pragma once

#include "Scena.h"
#include "../Risalnik.h"
#include "../Objekti/TileMap.h"
#include <string>

class MeniScena : public Scena
{
public:
    void zacetek() override;
    void posodobi(float delta_time) override;
    void narisi() override;

private:
    Tekstura m_tgozd = Tekstura("assets/gozd.png");
    TileMap m_tile_map = TileMap(&m_tgozd, 16, 10);

    std::string m_vpisano_ime;
};
