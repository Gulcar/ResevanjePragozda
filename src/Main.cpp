#include "Risalnik.h"
#include <iostream>
#include "scene/Scena.h"
#include "scene/IgraScena.h"
#include "scene/TestScena.h"

int main()
{
    std::cout << "pozdravljen svet!\n";

    risalnik::ustvari_okno("Resevanje Pragozda", 1280, 720);

    risalnik::set_camera_pos(glm::vec2(300.0f, 100.0f));

    scena::zamenjaj_na(std::make_unique<IgraScena>());
    scena::zamenjaj_na(std::make_unique<TestScena>());

    while (risalnik::je_okno_odprto())
    {
        scena::posodobi(0.1f);
    }

    risalnik::terminate();
}

