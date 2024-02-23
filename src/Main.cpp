#include "Ostalo.h"
#include "Risalnik.h"
#include "Input.h"
#include "Text.h"
#include "Scene/Scena.h"
#include "Scene/IgraScena.h"
#include <iostream>
#include <cstring>

void nastavi_cli_argumente(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    std::cout << "pozdravljen svet!\n";
    srand(time(nullptr));

    nastavi_cli_argumente(argc, argv);

    risalnik::ustvari_okno("Resevanje Pragozda", 1280, 720);
    input::init();
    text::init("assets/monogram-extended.ttf");

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
    text::izbrisi();
    risalnik::terminate();
}

void nastavi_cli_argumente(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--trkalniki") == 0)
            set_risanje_trkalnikov(true);
        else
            ERROR_EXIT("ne poznam argumenta: %s", argv[i]);
    }
}

