#include "Risalnik.h"
#include <iostream>

int main()
{
    std::cout << "pozdravljen svet!\n";

    risalnik::ustvari_okno();

    while (risalnik::je_okno_odprto())
    {
        risalnik::zacni_frame();

        risalnik::narisi_rect(glm::vec3(0.0f, -0.2f, 0.0f), glm::vec2(0.9f, 0.2f));
        risalnik::narisi_rect(glm::vec3(0.0f), glm::vec2(0.1f, 0.2f));
        risalnik::narisi_rect(glm::vec3(0.4f, 0.4f, 0.0f), glm::vec2(0.1f, 0.2f));

        risalnik::koncaj_frame();
    }

    risalnik::terminate();
}

