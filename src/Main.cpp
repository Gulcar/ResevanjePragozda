#include "Risalnik.h"
#include "Input.h"
#include <iostream>
#include "Scene/Scena.h"
#include "Scene/IgraScena.h"

int main()
{
    std::cout << "pozdravljen svet!\n";
    srand(time(nullptr));

    risalnik::ustvari_okno("Resevanje Pragozda", 1280, 720);
    input::init();

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

