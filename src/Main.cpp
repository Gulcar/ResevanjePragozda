#include "Risalnik.h"
#include "Input.h"
#include <iostream>
#include "Scene/Scena.h"
#include "Scene/IgraScena.h"

int main()
{
    std::cout << "pozdravljen svet!\n";

    risalnik::ustvari_okno("Resevanje Pragozda", 1280, 720);
    input::init();

    risalnik::nastavi_pozicijo_kamere(glm::vec2(0.0f, 0.0f));
    risalnik::nastavi_visino_perspektive(15.0f);

    scena::zamenjaj_na(std::make_unique<IgraScena>());

    while (risalnik::je_okno_odprto())
    {
        float delta_time = risalnik::zacni_frame();

        scena::posodobi(delta_time);
        scena::narisi();

        risalnik::koncaj_frame();
        input::pocisti();
    }

    scena::izbrisi();
    risalnik::terminate();
}

