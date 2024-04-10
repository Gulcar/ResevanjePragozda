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

    int m_meni = 0;

    std::string m_vpisano_ime = "igralec";

    std::vector<std::pair<std::string, int>> m_rezultati;

    bool m_obstaja_save;
};
