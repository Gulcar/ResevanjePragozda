#pragma once

#include "Scena.h"
#include "../Risalnik.h"

class TestScena : public Scena
{
public:
    void posodobi(float delta_time) override;

private:
    Tekstura m_rastline = Tekstura("assets/rastline.png");
    Sprite m_grm = m_rastline.ustvari_sprite(0, 0);
    Sprite m_rozica = m_rastline.ustvari_sprite(1, 0);
    Sprite m_trava = m_rastline.ustvari_sprite(0, 1);
    Sprite m_stor = m_rastline.ustvari_sprite(1, 1);
};
