#include "MeniScena.h"
#include "IgraScena.h"
#include "../Input.h"
#include "../Text.h"
#include <fstream>

void MeniScena::zacetek()
{
    risalnik::nastavi_visino_perspektive(5.0f);
    risalnik::nastavi_pozicijo_kamere(glm::vec2(0.0f, 0.0f));

    text::pocisti_char_vpis();

    std::ifstream ifile("rezultati.txt");
    if (ifile.fail() == false)
    {
        std::string ime; int st_tock;
        while (ifile >> ime >> st_tock)
        {
            m_rezultati.push_back({ ime, st_tock });
        }
    }
    ifile.close();
}

void MeniScena::posodobi(float delta_time)
{
}

void MeniScena::narisi()
{
    m_tile_map.narisi();

    if (m_meni == 0)
    {
        float y = 1.75f;
        text::narisi_centrirano("Resevanje Pragozda", glm::vec2(0.0f, y), 0.6f);

        y -= 0.75f;
        text::narisi_vpis(&m_vpisano_ime, glm::vec2(0.0f, y), 0.35f);

        y -= 0.5f;
        if (text::narisi_gumb("Igraj", glm::vec2(0.0f, y), 0.35f))
        {
            //scena::zamenjaj_na(std::make_unique<IgraScena>());
            m_meni = 1;
        }

        y -= 0.5f;
        if (text::narisi_gumb("Replay", glm::vec2(0.0f, y), 0.35f))
        {
            // TODO
        }

        y -= 0.5f;
        if (text::narisi_gumb("Fullscreen", glm::vec2(0.0f, y), 0.35f))
        {
            risalnik::toggle_fullscreen();
        }

        if (m_rezultati.size() > 0)
            text::narisi("Najboljsi Rezultati", glm::vec2(-4.0f, 0.5f), 0.3f);
        for (int i = 0; i < m_rezultati.size(); i++)
        {
            char buf[64];
            snprintf(buf, sizeof(buf), "%d. %s - %d", i + 1, m_rezultati[i].first.c_str(), m_rezultati[i].second);
            text::narisi(buf, glm::vec2(-4.0f, -(i + 1) * 0.3f + 0.5f), 0.3f);
        }

        if (text::narisi_gumb("Izhod", glm::vec2(0.0f, -1.8f), 0.35f))
        {
            risalnik::zapri_okno();
        }
    }
    else if (m_meni == 1)
    {
        float y = 1.0f;
        for (int i = 1; i <= 3; i++)
        {
            if (text::narisi_gumb("Level " + std::to_string(i), glm::vec2(0.0f, y), 0.35f))
            {
                scena::zamenjaj_na(std::make_unique<IgraScena>(m_vpisano_ime, i));
            }
            y -= 0.5f;
        }

        if (text::narisi_gumb("Nalozi Shranjeno", glm::vec2(0.0f, y), 0.35f))
        {
            // TODO: load
            scena::zamenjaj_na(std::make_unique<IgraScena>("shranjena_igra.bin"));
        }

        if (text::narisi_gumb("Nazaj", glm::vec2(0.0f, -1.8f), 0.35f))
        {
            m_meni = 0;
        }
    }
}
