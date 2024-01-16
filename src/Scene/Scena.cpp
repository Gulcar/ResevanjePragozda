#include "Scena.h"

namespace scena
{
    static std::unique_ptr<Scena> m_trenutna_scena;
    static std::unique_ptr<Scena> m_zamenjaj_na;

    void zamenjaj_na(std::unique_ptr<Scena> scena)
    {
        m_zamenjaj_na = std::move(scena);
    }

    void posodobi(float delta_time)
    {
        if (m_zamenjaj_na)
        {
            m_trenutna_scena = std::move(m_zamenjaj_na);
            m_trenutna_scena->zacetek();
        }

        if (m_trenutna_scena)
            m_trenutna_scena->posodobi(delta_time);
    }

    void narisi()
    {
        if (m_trenutna_scena)
            m_trenutna_scena->narisi();
    }

    void izbrisi()
    {
        m_trenutna_scena.reset();
    }
}
