#pragma once

#include <memory>
#include <glm/vec2.hpp>

class Scena
{
public:
    virtual ~Scena() {}
    virtual void zacetek() {}
    virtual void posodobi(float delta_time) = 0;
    virtual void narisi() = 0;
};

namespace scena
{
    void zamenjaj_na(std::unique_ptr<Scena> scena);

    void posodobi(float delta_time);
    void narisi();

    void izbrisi();
}
