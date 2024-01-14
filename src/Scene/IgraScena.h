#pragma once

#include "Scena.h"
#include "../Risalnik.h"

class IgraScena : public Scena
{
public:
    void zacetek() override;
    void posodobi(float delta_time) override;
    void narisi() override;

private:
    Tekstura m_vegovec = Tekstura("assets/vegovec.png");
};
