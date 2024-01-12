#include "Risalnik.h"
#include <iostream>

int main()
{
    std::cout << "pozdravljen svet!\n";

    risalnik::ustvari_okno("Resevanje Pragozda", 1280, 720);

    risalnik::set_camera_pos(glm::vec2(300.0f, 100.0f));

    Tekstura vegovec("assets/vegovec.png");

    while (risalnik::je_okno_odprto())
    {
        risalnik::zacni_frame();

        risalnik::narisi_sprite(vegovec, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(100.0f, 100.0f));
        risalnik::narisi_rect(glm::vec3(0.0f, -100.0f, 0.0f), glm::vec2(300.0f, 100.0f), glm::vec4(0.1f, 0.1f, 0.9f, 1.0f));
        risalnik::narisi_rect(glm::vec3(300.0f, 200.0f, 0.0f), glm::vec2(50.0f, 50.0f), glm::vec4(0.9f, 0.1f, 0.1f, 1.0f));

        risalnik::koncaj_frame();
    }

    risalnik::terminate();
}

