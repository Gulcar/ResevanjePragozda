#include "Risalnik.h"
#include "Ostalo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace risalnik
{

static GLFWwindow* m_window;

void ustvari_okno()
{
    if (!glfwInit())
        ERROR_EXIT("failed to init glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1280, 720, "ResevanjePragozda", nullptr, nullptr);
    if (!m_window) ERROR_EXIT("glfw create window failed");

    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, 1280, 720);
}

bool je_okno_odprto()
{
    return !glfwWindowShouldClose(m_window);
}

void zacni_frame()
{
    glfwPollEvents();

    glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void koncaj_frame()
{
    glfwSwapBuffers(m_window);
}

void terminate()
{
    glfwTerminate();
}

}

