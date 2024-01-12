#include "Risalnik.h"
#include "Ostalo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace risalnik
{

static GLFWwindow* m_window;
static uint32_t m_vao, m_vbo, m_ebo;
static uint32_t m_shader_prog;

struct Vertex
{
    glm::vec3 poz;
    glm::vec2 uv;
};
static std::vector<Vertex> m_batch_verts;
constexpr int BATCH_QUADS = 1000;
constexpr int BATCH_VERTICES = BATCH_QUADS * 4;
constexpr int BATCH_INDICIES = BATCH_QUADS * 6;

static glm::mat4 m_view_proj;
static glm::vec2 m_camera_pos;

static int m_window_width;
static int m_window_height;

static void ustvari_bufferje()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    m_batch_verts.reserve(BATCH_VERTICES);

    uint32_t indicies[BATCH_INDICIES];
    for (int i = 0; i < BATCH_QUADS; i++)
    {
        indicies[i * 6 + 0] = i * 4 + 0;
        indicies[i * 6 + 1] = i * 4 + 1;
        indicies[i * 6 + 2] = i * 4 + 2;
        indicies[i * 6 + 3] = i * 4 + 2;
        indicies[i * 6 + 4] = i * 4 + 3;
        indicies[i * 6 + 5] = i * 4 + 0;
    }

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, BATCH_VERTICES * sizeof(Vertex), nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, poz));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

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

static void posodobi_view_proj()
{
    constexpr float visina = 720.0f;
    float sirina = visina * m_window_width / (float)m_window_height;
    glm::mat4 proj = glm::ortho(-sirina / 2.0f, sirina / 2.0f, -visina / 2.0f, visina / 2.0f);

    m_view_proj = glm::translate(proj, glm::vec3(-m_camera_pos.x, -m_camera_pos.y, 0.0f));

    glUseProgram(m_shader_prog);
    int loc = glGetUniformLocation(m_shader_prog, "u_view_proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m_view_proj[0][0]);
}

void set_camera_pos(glm::vec2 pozicija)
{
    m_camera_pos = pozicija;
    posodobi_view_proj();
}

static void on_window_resize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    m_window_width = width;
    m_window_height = height;
    posodobi_view_proj();
}

void ustvari_okno(const char* naslov, int sirina, int visina)
{
    if (!glfwInit())
        ERROR_EXIT("failed to init glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(sirina, visina, naslov, nullptr, nullptr);
    if (!m_window) ERROR_EXIT("glfw create window failed");

    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, sirina, visina);
    glfwSetFramebufferSizeCallback(m_window, on_window_resize);
    m_window_width = sirina;
    m_window_height = visina;

    ustvari_bufferje();

    m_shader_prog = ustvari_shader(R"(
        #version 330 core
        layout (location = 0) in vec3 a_pos;
        layout (location = 1) in vec2 a_uv;

        uniform mat4 u_view_proj;
        out vec2 v_uv;

        void main()
        {
            gl_Position = u_view_proj * vec4(a_pos, 1.0);
            v_uv = a_uv;
        }
    )", R"(
        #version 330 core
        out vec4 v_frag_color;
        in vec2 v_uv;
        void main()
        {
            v_frag_color = vec4(v_uv, 0.1, 1.0);
        }
    )");

    set_camera_pos(glm::vec2(0.0f, 0.0f));
}

bool je_okno_odprto()
{
    return !glfwWindowShouldClose(m_window);
}

static void flush_batch()
{
    glUseProgram(m_shader_prog);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_batch_verts.size() * sizeof(Vertex), m_batch_verts.data());

    glDrawElements(GL_TRIANGLES, m_batch_verts.size() / 4 * 6, GL_UNSIGNED_INT, 0);

    m_batch_verts.resize(0);
}

void narisi_rect(glm::vec3 pozicija, glm::vec2 velikost, glm::vec4 barva)
{
    if (m_batch_verts.size() + 4 > BATCH_VERTICES)
        flush_batch();

    float half_w = velikost.x / 2.0f;
    float half_h = velikost.y / 2.0f;

    m_batch_verts.push_back(Vertex {
        pozicija + glm::vec3(-half_w, -half_h, 0.0f),
        glm::vec2(0.0f, 0.0f),
    });
    m_batch_verts.push_back(Vertex {
        pozicija + glm::vec3(half_w, -half_h, 0.0f),
        glm::vec2(1.0f, 0.0f),
    });
    m_batch_verts.push_back(Vertex {
        pozicija + glm::vec3(half_w, half_h, 0.0f),
        glm::vec2(1.0f, 1.0f),
    });
    m_batch_verts.push_back(Vertex {
        pozicija + glm::vec3(-half_w, half_h, 0.0f),
        glm::vec2(0.0f, 1.0f),
    });
}

void narisi_crto(glm::vec3 a, glm::vec3 b, glm::vec4 barva)
{
    // TODO
}

void zacni_frame()
{
    glfwPollEvents();

    glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void koncaj_frame()
{
    flush_batch();
    glfwSwapBuffers(m_window);
}

void terminate()
{
    glfwTerminate();
}

}

