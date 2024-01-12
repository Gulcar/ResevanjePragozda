#include "Risalnik.h"
#include "Ostalo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

Tekstura::Tekstura(const char* filepath, bool nearest)
{
    glGenTextures(1, &m_render_id);
    glBindTexture(GL_TEXTURE_2D, m_render_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (nearest)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        // manjka podpora za mipmape ampak jih verjetno ne bom rabil
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    stbi_set_flip_vertically_on_load(true);

    int st_kanalov;
    uint8_t* pixels = stbi_load(filepath, &m_sirina, &m_visina, &st_kanalov, 0);
    if (!pixels)
    {
        char path[128] = "../../";
        strcat(path, filepath);
        pixels = stbi_load(path, &m_sirina, &m_visina, &st_kanalov, 0);
    }
    if (!pixels) ERROR_EXIT("ni uspelo naloziti teksture %s", filepath);

    GLenum format;
    if (st_kanalov == 3) format = GL_RGB;
    else if (st_kanalov == 4) format = GL_RGBA;
    else ERROR_EXIT("cuden format teksture %s %d", filepath, st_kanalov);

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_sirina, m_visina, 0, format, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);
}

Tekstura::Tekstura(const uint8_t* pixli, int sirina, int visina, bool nearest)
{
    m_sirina = sirina;
    m_visina = visina;

    glGenTextures(1, &m_render_id);
    glBindTexture(GL_TEXTURE_2D, m_render_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (nearest)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        // manjka podpora za mipmape ampak jih verjetno ne bom rabil
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_sirina, m_visina, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixli);
}

Tekstura::~Tekstura()
{
    if (m_render_id != -1)
        glDeleteTextures(1, &m_render_id);
}

namespace risalnik
{

static GLFWwindow* m_window;
static uint32_t m_vao, m_vbo, m_ebo;
static uint32_t m_shader_prog;

struct Vertex
{
    glm::vec3 poz;
    glm::vec2 uv;
    glm::vec4 barva;
    int tekstura;
};
static std::vector<Vertex> m_batch_verts;
constexpr int BATCH_QUADS = 1000;
constexpr int BATCH_VERTICES = BATCH_QUADS * 4;
constexpr int BATCH_INDICIES = BATCH_QUADS * 6;

static glm::mat4 m_view_proj;
static glm::vec2 m_camera_pos;

static int m_window_width;
static int m_window_height;

static Tekstura* m_bel_pixel;
constexpr int MAX_TEXTURE_SLOTS = 8;
static std::vector<uint32_t> m_texture_slots;

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
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, barva));
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, tekstura));

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    uint32_t pixel = 0xffffffff;
    m_bel_pixel = new Tekstura((uint8_t*)&pixel, 1, 1);

    m_shader_prog = ustvari_shader(R"(
        #version 330 core
        layout (location = 0) in vec3 a_pos;
        layout (location = 1) in vec2 a_uv;
        layout (location = 2) in vec4 a_barva;
        layout (location = 3) in int a_tekstura;

        uniform mat4 u_view_proj;
        out vec2 v_uv;
        out vec4 v_barva;
        flat out int v_tekstura;

        void main()
        {
            gl_Position = u_view_proj * vec4(a_pos, 1.0);
            v_uv = a_uv;
            v_barva = a_barva;
            v_tekstura = a_tekstura;
        }
    )", R"(
        #version 330 core
        out vec4 v_frag_color;
        in vec2 v_uv;
        in vec4 v_barva;
        flat in int v_tekstura;
        uniform sampler2D u_teksture[8];

        void main()
        {
            v_frag_color = texture(u_teksture[v_tekstura], v_uv) * v_barva;
        }
    )");

    set_camera_pos(glm::vec2(0.0f, 0.0f));

    glUseProgram(m_shader_prog);
    int slots[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    glUniform1iv(glGetUniformLocation(m_shader_prog, "u_teksture"), 8, slots);
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

    for (int i = 0; i < m_texture_slots.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_texture_slots[i]);
    }

    glDrawElements(GL_TRIANGLES, m_batch_verts.size() * 6 / 4, GL_UNSIGNED_INT, 0);

    m_batch_verts.resize(0);
    m_texture_slots.resize(0);
}

static int dobi_slot_teksture(const Tekstura& teks)
{
    for (int i = 0; i < m_texture_slots.size(); i++)
    {
        if (m_texture_slots[i] == teks.render_id())
            return i;
    }

    if (m_texture_slots.size() + 1 > MAX_TEXTURE_SLOTS)
        flush_batch();

    m_texture_slots.push_back(teks.render_id());
    return m_texture_slots.size() - 1;
}

void narisi_rect(glm::vec3 pozicija, glm::vec2 velikost, glm::vec4 barva)
{
    narisi_sprite(*m_bel_pixel, pozicija, velikost, barva);
}

void narisi_sprite(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, glm::vec4 barva)
{
    if (m_batch_verts.size() + 4 > BATCH_VERTICES)
        flush_batch();

    int index_teksture = dobi_slot_teksture(tekstura);

    float half_w = velikost.x / 2.0f;
    float half_h = velikost.y / 2.0f;

    m_batch_verts.push_back(Vertex{
        pozicija + glm::vec3(-half_w, -half_h, 0.0f),
        glm::vec2(0.0f, 0.0f),
        barva,
        index_teksture,
    });
    m_batch_verts.push_back(Vertex{
        pozicija + glm::vec3(half_w, -half_h, 0.0f),
        glm::vec2(1.0f, 0.0f),
        barva,
        index_teksture,
    });
    m_batch_verts.push_back(Vertex{
        pozicija + glm::vec3(half_w, half_h, 0.0f),
        glm::vec2(1.0f, 1.0f),
        barva,
        index_teksture,
    });
    m_batch_verts.push_back(Vertex{
        pozicija + glm::vec3(-half_w, half_h, 0.0f),
        glm::vec2(0.0f, 1.0f),
        barva,
        index_teksture,
    });
}

void narisi_crto(glm::vec3 a, glm::vec3 b, glm::vec4 barva)
{
    // TODO
}

void zacni_frame()
{
    glfwPollEvents();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

