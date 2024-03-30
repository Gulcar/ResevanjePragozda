#include "MeniScena.h"
#include "IgraScena.h"
#include "../Input.h"
#include "../Text.h"

void MeniScena::zacetek()
{
    risalnik::nastavi_visino_perspektive(5.0f);
    risalnik::nastavi_pozicijo_kamere(glm::vec2(0.0f, 0.0f));

    text::pocisti_char_vpis();
}

void MeniScena::posodobi(float delta_time)
{
}

void MeniScena::narisi()
{
    m_tile_map.narisi();

    float y = 1.75f;
    text::narisi_centrirano("Resevanje Pragozda", glm::vec2(0.0f, y), 0.6f);

    y -= 1.0f;
    text::narisi_vpis(&m_vpisano_ime, glm::vec2(0.0f, y), 0.35f);

    y -= 1.0f;
    if (text::narisi_gumb("Igraj", glm::vec2(0.0f, y), 0.35f))
    {
        scena::zamenjaj_na(std::make_unique<IgraScena>());
    }

    y -= 0.5f;
    text::narisi_gumb("Replay", glm::vec2(0.0f, y), 0.35f);

    y -= 0.5f;
    text::narisi_gumb("Nastavitve", glm::vec2(0.0f, y), 0.35f);

    y -= 0.5f;
    if (text::narisi_gumb("Izhod", glm::vec2(0.0f, y), 0.35f))
    {
        risalnik::zapri_okno();
    }
}
