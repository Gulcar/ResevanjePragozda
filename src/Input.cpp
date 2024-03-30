#include "Input.h"
#include <GLFW/glfw3.h>
#include "Risalnik.h"
#include <cstring>

namespace input
{
    static bool m_keys_just_pressed[GLFW_KEY_LAST + 1];
    static bool m_keys_just_released[GLFW_KEY_LAST + 1];

    static bool m_mouse_just_pressed[GLFW_MOUSE_BUTTON_LAST + 1];
    static bool m_mouse_just_released[GLFW_MOUSE_BUTTON_LAST + 1];

    static GLFWwindow* m_window;

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            m_keys_just_pressed[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_keys_just_released[key] = true;
        }
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            m_mouse_just_pressed[button] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_mouse_just_released[button] = true;
        }
    }

    void init()
    {
        m_window = risalnik::vrni_okno();
        glfwSetKeyCallback(m_window, key_callback);
        glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    }

    void pocisti()
    {
        memset(m_keys_just_pressed, 0, sizeof(m_keys_just_pressed));
        memset(m_keys_just_released, 0, sizeof(m_keys_just_released));

        memset(m_mouse_just_pressed, 0, sizeof(m_mouse_just_pressed));
        memset(m_mouse_just_released, 0, sizeof(m_mouse_just_released));
    }

    bool tipka_drzana(int key)
    {
        return glfwGetKey(m_window, key);
    }

    bool tipka_pritisnjena(int key)
    {
        return m_keys_just_pressed[key];
    }

    bool tipka_spuscena(int key)
    {
        return m_keys_just_released[key];
    }

    int os_tipk(int neg, int poz)
    {
        return glfwGetKey(m_window, poz) - glfwGetKey(m_window, neg);
    }

    bool miska_drzana(int button)
    {
        return glfwGetMouseButton(m_window, button);
    }

    bool miska_pritisnjena(int button)
    {
        return m_mouse_just_pressed[button];
    }

    bool miska_spuscena(int button)
    {
        return m_mouse_just_released[button];
    }

    glm::vec2 pozicija_miske()
    {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        return glm::vec2((float)x, (float)y);
    }

    glm::vec2 pozicija_miske_v_svetu()
    {
        glm::vec2 poz = pozicija_miske();
        glm::vec2 vidno = risalnik::velikost_vidnega();
        glm::vec2 vel_okna = risalnik::velikost_okna();

        poz.x = (poz.x - vel_okna.x / 2.0f) / vel_okna.x;
        poz.x *= vidno.x;

        poz.y = (poz.y - vel_okna.y / 2.0f) / vel_okna.y;
        poz.y *= -vidno.y;
        
        poz += risalnik::dobi_pozicijo_kamere();
        return poz;
    }
}
