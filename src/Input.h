#pragma once

#include <glm/vec2.hpp>
#include "InputDefines.h"

namespace input
{
    void init();
    void pocisti();

    bool tipka_drzana(int key);
    bool tipka_pritisnjena(int key);
    bool tipka_spuscena(int key);

    bool miska_drzana(int button);
    bool miska_pritisnjena(int button);
    bool miska_spuscena(int button);

    glm::vec2 pozicija_miske();
}
