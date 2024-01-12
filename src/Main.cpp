#include <fcntl.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define ERROR_EXIT(...) { printf("ERROR: "); printf(__VA_ARGS__); printf("\n"); std::exit(1); }

int main()
{
    std::cout << "pozdravljen svet!\n";

    if (!glfwInit())
        ERROR_EXIT("failed to init glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "ResevanjePragozda", nullptr, nullptr);
    if (!window) ERROR_EXIT("glfw create window failed");

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, 1280, 720);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}

