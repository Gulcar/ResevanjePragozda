#include "Risalnik.h"
#include <iostream>

int main()
{
    std::cout << "pozdravljen svet!\n";

    risalnik::ustvari_okno();

    while (risalnik::je_okno_odprto())
    {
        risalnik::zacni_frame();

        risalnik::koncaj_frame();
    }

    risalnik::terminate();
}

