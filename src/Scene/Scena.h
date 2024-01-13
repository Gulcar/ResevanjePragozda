#pragma once

#include <memory>

class Scena
{
public:
    virtual ~Scena() {}
    virtual void posodobi(float delta_time) = 0;
};

namespace scena
{
    void zamenjaj_na(std::unique_ptr<Scena> scena);
    void posodobi(float delta_time);
}
