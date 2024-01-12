#include "Risalnik.h"
#include "Ostalo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace risalnik
{

static GLFWwindow* m_window;
static uint32_t m_vao, m_vbo, m_ebo;
static uint32_t m_shader_prog;

static void ustvari_bufferje()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    float vertices[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f,
    };

    uint32_t indicies[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    glBindVertexArray(0);
}

static void preveri_shader(uint32_t shader, const char* tip)
{
    int compile;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile);
    if (!compile)
    {
        int log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        char* log = new char[log_length];
        glGetShaderInfoLog(shader, log_length, 0, log);

        ERROR_EXIT("failed to compile %s shader:\n%s", tip, log);
    }
}

static void preveri_program(uint32_t program)
{
    int ok;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        int log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        char* log = new char[log_length];
        glGetProgramInfoLog(program, log_length, 0, log);

        ERROR_EXIT("failed to link program:\n%s", log);
    }
}

static uint32_t ustvari_shader(const char* vert_src, const char* frag_src)
{
    uint32_t vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_src, 0);
    glCompileShader(vert);

    uint32_t frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_src, 0);
    glCompileShader(frag);

    preveri_shader(vert, "vertex");
    preveri_shader(frag, "fragment");

    uint32_t program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    preveri_program(program);

    glDetachShader(program, vert);
    glDetachShader(program, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    return program;
}

void on_window_resize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

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
    glfwSetFramebufferSizeCallback(m_window, on_window_resize);

    ustvari_bufferje();

    m_shader_prog = ustvari_shader(R"(
        #version 330 core
        layout (location = 0) in vec3 a_pos;

        void main()
        {
            gl_Position = vec4(a_pos, 1.0);
        }
    )", R"(
        #version 330 core
        out vec4 v_frag_color;
        void main()
        {
            v_frag_color = vec4(0.1, 0.9, 0.1, 1.0);
        }
    )");
}

bool je_okno_odprto()
{
    return !glfwWindowShouldClose(m_window);
}

void narisi_rect(glm::vec3 pozcija, glm::vec2 velikost, glm::vec4 barva)
{
    glUseProgram(m_shader_prog);
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void narisi_crto(glm::vec3 a, glm::vec3 b, glm::vec4 barva)
{

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

