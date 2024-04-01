#include "IgraScena.h"
#include "MeniScena.h"
#include "../Input.h"
#include "../Text.h"
#include "../Ostalo.h"
#include "../Objekti/MiniMap.h"
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

IgraScena::IgraScena(const std::string& ime_igralca, int level)
{
    m_igralec.ime = ime_igralca;
    m_level = level;

    if (level == 1)
    {
        /* TODO: odkomentiraj
        m_spawner.nastavi_wave(5, 0, 0, 0, 1.0f);
        m_spawner.nastavi_wave(7, 2, 1, 0, 1.0f);
        m_spawner.nastavi_wave(8, 4, 3, 1, 1.0f);
        */
        std::cout << "izbran level 1\n";
    }
    else if (level == 2)
    {
        m_spawner.nastavi_wave(10, 0, 0, 0, 1.0f);
        m_spawner.nastavi_wave(14, 4, 4, 0, 1.0f);
        m_spawner.nastavi_wave(10, 10, 10, 0, 1.0f);
        m_spawner.nastavi_wave(10, 10, 10, 3, 1.0f);
        std::cout << "izbran level 2\n";
    }
    else if (level == 3)
    {
        m_spawner.nastavi_wave(10, 0, 0, 0, 0.5f);
        m_spawner.nastavi_wave(14, 4, 4, 0, 0.5f);
        m_spawner.nastavi_wave(10, 10, 10, 0, 0.5f);
        m_spawner.nastavi_wave(10, 10, 10, 3, 0.5f);
        m_spawner.nastavi_wave(10, 20, 20, 6, 0.5f);
        m_spawner.nastavi_wave(10, 30, 30, 10, 0.5f);
        std::cout << "izbran level 3\n";
    }

    m_igralec.pozicija = { 0.0f, -15.0f };

    m_pomocniki.emplace_back(&m_tdomorodci, 0, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    m_pomocniki.emplace_back(&m_tdomorodci, 1, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    m_pomocniki.emplace_back(&m_tdomorodci, 2, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    m_pomocniki.emplace_back(&m_tdomorodci, 3, vec2_iz_kota(randf() * PI * 2) * 15.0f, 5.0f);
    m_pomocniki.emplace_back(&m_tdomorodci, 4, vec2_iz_kota(randf() * PI * 2) * 15.0f, 5.0f);
}

IgraScena::IgraScena(const char* save_file)
{
    std::ifstream file(save_file, std::ios::binary);
    if (file.fail())
        ERROR_EXIT("ni uspelo odpreti datoteke '%s' za branje", save_file);

    file.read((char*)&m_level, sizeof(m_level));
    file.read((char*)&m_cas, sizeof(m_cas));

    m_igralec.nalozi(file);

    m_gozd_zunanji.nalozi(file, false);
    m_gozd_notranji.nalozi(file, true);

    m_spawner.nalozi(file);

    m_pomocniki.emplace_back(&m_tdomorodci, 0, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    m_pomocniki.emplace_back(&m_tdomorodci, 1, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    m_pomocniki.emplace_back(&m_tdomorodci, 2, vec2_iz_kota(randf() * PI * 2) * 15.0f, 1.5f);
    m_pomocniki.emplace_back(&m_tdomorodci, 3, vec2_iz_kota(randf() * PI * 2) * 15.0f, 5.0f);
    m_pomocniki.emplace_back(&m_tdomorodci, 4, vec2_iz_kota(randf() * PI * 2) * 15.0f, 5.0f);

    for (int i = 0; i < 5; i++)
    {
        m_pomocniki[i].nalozi(file);
    }

    if (file.fail() || file.eof())
        ERROR_EXIT("shranjena igra ni bila uspesno prebrana");

    char c;
    file.read(&c, sizeof(c));
    if (file.eof() == false)
        ERROR_EXIT("shranjena igra ni bila uspesno prebrana (predolga)");

    file.close();
    printf("igra nalozena iz datoteke '%s'", save_file);
}

void IgraScena::zacetek()
{
    risalnik::nastavi_visino_perspektive(30.0f);

    std::cout << "stevilo zunanjih dreves: " << m_gozd_zunanji.drevesa.size() << "\n";
    std::cout << "stevilo notranjih dreves: " << m_gozd_notranji.drevesa.size() << "\n";
}

void IgraScena::posodobi(float delta_time)
{
    if (input::tipka_pritisnjena(GLFW_KEY_ESCAPE))
        m_pavza = !m_pavza;

    if (m_cas_prev_delta <= 0.0f)
    {
        m_prev_delta = delta_time;
        m_cas_prev_delta = 0.5f;
    }
    m_cas_prev_delta -= delta_time;

    if (m_pavza)
        return;

    m_cas += delta_time;

    /*
    if (input::tipka_pritisnjena(GLFW_KEY_P))
        scena::zamenjaj_na(std::make_unique<TestScena>());
    if (input::tipka_pritisnjena(GLFW_KEY_R))
        scena::zamenjaj_na(std::make_unique<IgraScena>());


    // TODO: odstrani zoom
    if (input::tipka_pritisnjena(GLFW_KEY_M))
        risalnik::nastavi_visino_perspektive(30.0f * 3.0f);
    if (input::tipka_spuscena(GLFW_KEY_M))
        risalnik::nastavi_visino_perspektive(30.0f);
    */

    m_igralec.posodobi(delta_time, m_spawner.zlobnezi, m_gozd_notranji);
    m_spawner.posodobi(delta_time, &m_igralec, m_gozd_notranji);

    m_gozd_notranji.posodobi(delta_time);

    for (auto& pomocnik : m_pomocniki)
        pomocnik.posodobi(delta_time, m_spawner.zlobnezi);

    Pomocnik::daj_narazen(m_pomocniki);

    if (!m_zmaga && m_spawner.je_konec_wavov() && m_gozd_notranji.vsi_ognji_pogaseni())
    {
        m_zmaga = true;
        if (m_level == 0) m_st_tock = 0;
        else if (m_level == 1) m_st_tock = 500;
        else if (m_level == 2) m_st_tock = 1000;
        m_st_tock += (int)std::max(600.0f - m_cas, 0.0f) + m_gozd_notranji.drevesa.size() * 7;
        shrani_rezultat();
    }

    if (m_zmaga && input::tipka_pritisnjena(GLFW_KEY_ENTER))
    {
        scena::zamenjaj_na(std::make_unique<MeniScena>());
    }
}

void IgraScena::narisi()
{
    m_tile_map.narisi();
    m_gozd_zunanji.narisi();
    m_gozd_notranji.narisi();

    m_spawner.narisi();
    m_igralec.narisi();

    for (auto& pomocnik : m_pomocniki)
        pomocnik.narisi();

    //risalnik::narisi_teksturo(m_tdomorodci, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(3.0f * 5.0f, 3.0f * 3.0f));

    //text::narisi("Pozdravljen svet", glm::vec2(15.0f, -1.0f), 1.0f);
    //text::narisi_centrirano("Pozdravljen svet", glm::vec2(15.0f, -5.0f), 1.0f);

    glm::vec2 vidno = risalnik::velikost_vidnega();
    glm::vec2 poz_levo_gor = glm::vec2(-vidno.x / 2.0f + 0.4f, vidno.y / 2.0f - 1.2f) + risalnik::dobi_pozicijo_kamere();
    text::narisi("LEVEL " + std::to_string(m_level), poz_levo_gor, 1.0f);
    poz_levo_gor.y -= 1.0f;
    text::narisi(std::to_string(m_spawner.st_wava) + ". VAL SOVRAZNIKOV", poz_levo_gor, 1.0f);

    poz_levo_gor.y -= 1.0f;
    char buf[48];
    snprintf(buf, sizeof(buf), "%.0ffps %.2fms", 1.0f / m_prev_delta, m_prev_delta * 1000.0f);
    text::narisi(buf, poz_levo_gor, 1.0f);

    if (m_zmaga)
    {
        glm::vec2 poz = risalnik::dobi_pozicijo_kamere();
        risalnik::narisi_rect(glm::vec3(poz, 0.5f), risalnik::velikost_vidnega(), glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        text::narisi_centrirano("ZMAGA", glm::vec2(poz.x, poz.y + 3.0f), 3.0f);
        snprintf(buf, sizeof(buf), "stevilo tock: %d", m_st_tock);
        text::narisi_centrirano(buf, glm::vec2(poz.x, poz.y), 2.0f);
        text::narisi_centrirano("pritisni enter", glm::vec2(poz.x, poz.y - 2.0f), 1.5f);
    }

    if (m_pavza)
    {
        glm::vec2 poz = risalnik::dobi_pozicijo_kamere();
        risalnik::narisi_rect(glm::vec3(poz, 0.51f), risalnik::velikost_vidnega(), glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        poz.y += 4.0f;
        text::narisi_centrirano("PAVZA", poz, 4.0f);

        poz.y -= 3.4f;
        if (text::narisi_gumb("Fullscreen", poz, 2.0f))
        {
            risalnik::toggle_fullscreen();
        }

        poz.y -= 2.2f;
        if (text::narisi_gumb("Shrani", poz, 2.0f))
        {
            shrani_igro();
        }

        poz.y -= 2.2f;
        if (text::narisi_gumb("Nazaj", poz, 2.0f))
        {
            scena::zamenjaj_na(std::make_unique<MeniScena>());
        }
    }

    minimap::narisi_ozadje();
    minimap::narisi_igralca(m_igralec.pozicija);

    for (const auto& pomagac : m_pomocniki)
        minimap::narisi_pomagaca(pomagac.pozicija);

    for (const auto& zlobnez : m_spawner.zlobnezi)
    {
        if (zlobnez.tip == ZlobnezTip::Buldozer)
            minimap::narisi_velikega_zlobneza(zlobnez.pozicija);
        else
            minimap::narisi_zlobneza(zlobnez.pozicija);
    }

    for (const auto& drevo : m_gozd_notranji.drevesa)
    {
        if (drevo.cas_ognja > 0.0f)
            minimap::narisi_ogenj(drevo.pozicija);
        minimap::narisi_pomembno_drevo(drevo.pozicija);
    }

    for (const auto& drevo : m_gozd_zunanji.drevesa)
    {
        if (drevo.pozicija.x > -70.5f && drevo.pozicija.x < 70.5f &&
            drevo.pozicija.y > -50.5f && drevo.pozicija.y < 50.5f)
        {
            minimap::narisi_drevo(drevo.pozicija);
        }
    }
}

void IgraScena::shrani_rezultat()
{
    using Rezultat = std::pair<std::string, int>;
    std::vector<Rezultat> rezultati;

    std::ifstream ifile("rezultati.txt");
    if (ifile.fail() == false)
    {
        std::string ime; int st_tock;
        while (ifile >> ime >> st_tock)
        {
            rezultati.push_back({ ime, st_tock });
        }
    }

    ifile.close();

    rezultati.push_back({ m_igralec.ime, m_st_tock });
    std::sort(rezultati.begin(), rezultati.end(), [](const Rezultat& a, const Rezultat& b) {
        return a.second > b.second;
    });
    if (rezultati.size() > 5)
    {
        rezultati.resize(5);
    }

    std::ofstream ofile("rezultati.txt");
    if (ofile.fail())
        ERROR_EXIT("ni mogoce odpreti rezultati.txt za pisanje");

    for (int i = 0; i < rezultati.size(); i++)
    {
        ofile << rezultati[i].first << "    " << rezultati[i].second << "\n";
    }

    ofile.close();
}

void IgraScena::shrani_igro()
{
    std::ofstream file("shranjena_igra.bin", std::ios::binary);
    if (file.fail())
        ERROR_EXIT("ni mogoce odpreti shranjena_igra.bin za pisanje");
    
    file.write((const char*)&m_level, sizeof(m_level));
    file.write((const char*)&m_cas, sizeof(m_cas));

    m_igralec.shrani(file);

    m_gozd_zunanji.shrani(file, false);
    m_gozd_notranji.shrani(file, true);

    m_spawner.shrani(file);
    
    for (int i = 0; i < 5; i++)
    {
        m_pomocniki.at(i).shrani(file);
    }

    file.close();
    printf("shranjeno\n");
}
