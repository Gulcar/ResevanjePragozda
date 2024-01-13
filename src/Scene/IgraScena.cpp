#include "IgraScena.h"

void IgraScena::posodobi(float delta_time)
{
    risalnik::zacni_frame();

    risalnik::narisi_teksturo(m_vegovec, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(250.0f));

    risalnik::koncaj_frame();
}
