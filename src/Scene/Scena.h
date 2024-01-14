#pragma once

#include <memory>
#include <vector>
#include <glm/vec2.hpp>

class Objekt
{
public:
    virtual ~Objekt() {}
    virtual void posodobi(float delta_time) = 0;
    virtual void narisi() = 0;

    glm::vec2 pozicija = { 0.0f, 0.0f };
};

class Scena
{
public:
    virtual ~Scena() {}
    virtual void zacetek() {}
    virtual void posodobi(float delta_time);
    virtual void narisi();

    std::vector<std::unique_ptr<Objekt>> objekti;
};

namespace scena
{
    void zamenjaj_na(std::unique_ptr<Scena> scena);
    void posodobi(float delta_time);
    void narisi();
}
