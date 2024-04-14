#include "Risalnik.h"
#include "Ostalo.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
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
    if (!pixels)
    {
        char path[128] = "../";
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
    printf("tekstura %d %s ustvarjena\n", m_render_id, filepath);
}

Tekstura::Tekstura(const uint8_t* pixli, int sirina, int visina, bool nearest, int st_kanalov)
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

    GLenum format;
    switch (st_kanalov)
    {
    case 1:
    {
        format = GL_RED;
        GLint swizzle_mask[] = { GL_RED, GL_RED, GL_RED, GL_RED };
        glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
        break;
    }
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    default: assert(false && "napacen format za teksturo");
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_sirina, m_visina, 0, format, GL_UNSIGNED_BYTE, pixli);
    printf("tekstura %d ustvarjena iz pomnilnika\n", m_render_id);
}

Sprite Tekstura::ustvari_sprite(int tile_x, int tile_y, int velikost_tilov, int st_tilov_x, int st_tilov_y) const
{
    Sprite sprite;
    sprite.m_tekstura = this;

    tile_y = m_visina / velikost_tilov - tile_y - 1;
    
    glm::vec2 levo_zgoraj = glm::vec2(tile_x * velikost_tilov, (tile_y - st_tilov_y + 1) * velikost_tilov);
    glm::vec2 desno_spodaj = glm::vec2((tile_x + st_tilov_x) * velikost_tilov, (tile_y + 1) * velikost_tilov);

    sprite.m_min_uv = levo_zgoraj / glm::vec2(m_sirina, m_visina);
    sprite.m_max_uv = desno_spodaj / glm::vec2(m_sirina, m_visina);

    return sprite;
}

Sprite Tekstura::ustvari_sprite(glm::vec2 min_uv, glm::vec2 max_uv) const
{
    Sprite sprite;
    sprite.m_tekstura = this;
    sprite.m_min_uv = min_uv;
    sprite.m_max_uv = max_uv;
    return sprite;
}

Tekstura::~Tekstura()
{
    if (m_render_id != -1)
    {
        glDeleteTextures(1, &m_render_id);
        printf("izbrisana tekstura %d\n", m_render_id);
    }
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
static float m_zoom_height = 720.0f;

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
    float visina = m_zoom_height;
    float sirina = visina * m_window_width / (float)m_window_height;
    glm::mat4 proj = glm::ortho(-sirina / 2.0f, sirina / 2.0f, -visina / 2.0f, visina / 2.0f);

    m_view_proj = glm::translate(proj, glm::vec3(-m_camera_pos.x, -m_camera_pos.y, 0.0f));

    glUseProgram(m_shader_prog);
    int loc = glGetUniformLocation(m_shader_prog, "u_view_proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m_view_proj[0][0]);
}

void nastavi_pozicijo_kamere(glm::vec2 pozicija)
{
    m_camera_pos = pozicija;
    posodobi_view_proj();
}

glm::vec2 dobi_pozicijo_kamere()
{
    return m_camera_pos;
}

void nastavi_visino_perspektive(float h)
{
    m_zoom_height = h;
    posodobi_view_proj();
}

glm::vec2 velikost_vidnega()
{
    return glm::vec2(
        m_zoom_height * m_window_width / (float)m_window_height,
        m_zoom_height
    );
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

    glfwSwapInterval(1);

    glViewport(0, 0, sirina, visina);
    glfwSetFramebufferSizeCallback(m_window, on_window_resize);
    m_window_width = sirina;
    m_window_height = visina;

    ustvari_bufferje();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

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
            vec4 tex_color;
            switch (v_tekstura)
            {
            case 0: tex_color = texture(u_teksture[0], v_uv); break;
            case 1: tex_color = texture(u_teksture[1], v_uv); break;
            case 2: tex_color = texture(u_teksture[2], v_uv); break;
            case 3: tex_color = texture(u_teksture[3], v_uv); break;
            case 4: tex_color = texture(u_teksture[4], v_uv); break;
            case 5: tex_color = texture(u_teksture[5], v_uv); break;
            case 6: tex_color = texture(u_teksture[6], v_uv); break;
            case 7: tex_color = texture(u_teksture[7], v_uv); break;
            }
            v_frag_color = tex_color * v_barva;
            //v_frag_color = vec4(v_uv, 0.0, 1.0);
            if (v_frag_color.a < 0.1)
                discard;
        }
    )");

    nastavi_pozicijo_kamere(glm::vec2(0.0f, 0.0f));

    glUseProgram(m_shader_prog);
    int slots[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    glUniform1iv(glGetUniformLocation(m_shader_prog, "u_teksture"), 8, slots);
}

bool je_okno_odprto()
{
    return !glfwWindowShouldClose(m_window);
}

void zapri_okno()
{
    glfwSetWindowShouldClose(m_window, true);
}

struct GLFWwindow* vrni_okno()
{
    return m_window;
}

glm::vec2 velikost_okna()
{
    return glm::vec2(m_window_width, m_window_height);
}

void toggle_fullscreen()
{
    static bool fullscreen = false;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (!fullscreen)
    { 
        glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        glfwSetWindowMonitor(m_window, NULL, 100, 100, mode->width / 2, mode->height / 2, mode->refreshRate);
    }

    fullscreen = !fullscreen;
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

static void vstavi_quad(glm::vec3 pozicija, glm::vec2 velikost, glm::vec2 min_uv, glm::vec2 max_uv, glm::vec4 barva, int tex_index, float rotacija)
{
    float half_w = velikost.x / 2.0f;
    float half_h = velikost.y / 2.0f;

    if (rotacija == 0.0f)
    {
        m_batch_verts.push_back(Vertex{
            pozicija + glm::vec3(-half_w, -half_h, 0.0f),
            min_uv,
            barva,
            tex_index,
        });
        m_batch_verts.push_back(Vertex{
            pozicija + glm::vec3(half_w, -half_h, 0.0f),
            glm::vec2(max_uv.x, min_uv.y),
            barva,
            tex_index,
        });
        m_batch_verts.push_back(Vertex{
            pozicija + glm::vec3(half_w, half_h, 0.0f),
            max_uv,
            barva,
            tex_index,
        });
        m_batch_verts.push_back(Vertex{
            pozicija + glm::vec3(-half_w, half_h, 0.0f),
            glm::vec2(min_uv.x, max_uv.y),
            barva,
            tex_index,
        });
    }
    else
    {
        glm::mat3 rotation_mat = glm::rotate(glm::mat3(1.0f), rotacija);

        m_batch_verts.push_back(Vertex{
            pozicija + rotation_mat * glm::vec3(-half_w, -half_h, 0.0f),
            min_uv,
            barva,
            tex_index,
        });
        m_batch_verts.push_back(Vertex{
            pozicija + rotation_mat * glm::vec3(half_w, -half_h, 0.0f),
            glm::vec2(max_uv.x, min_uv.y),
            barva,
            tex_index,
        });
        m_batch_verts.push_back(Vertex{
            pozicija + rotation_mat * glm::vec3(half_w, half_h, 0.0f),
            max_uv,
            barva,
            tex_index,
        });
        m_batch_verts.push_back(Vertex{
            pozicija + rotation_mat * glm::vec3(-half_w, half_h, 0.0f),
            glm::vec2(min_uv.x, max_uv.y),
            barva,
            tex_index,
        });
    }
}

void narisi_rect(glm::vec3 pozicija, glm::vec2 velikost, glm::vec4 barva, float rotacija)
{
    narisi_teksturo(*m_bel_pixel, pozicija, velikost, false, barva, rotacija);
}

void narisi_teksturo(const Tekstura& tekstura, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h, glm::vec4 barva, float rotacija)
{
    if (m_batch_verts.size() + 4 > BATCH_VERTICES)
        flush_batch();

    int index_teksture = dobi_slot_teksture(tekstura);

    glm::vec2 min_uv = { 0.0f, 0.0f };
    glm::vec2 max_uv = { 1.0f, 1.0f };
    if (flip_h)
        std::swap(min_uv.x, max_uv.x);

    vstavi_quad(pozicija, velikost, min_uv, max_uv, barva, index_teksture, rotacija);
}

void narisi_sprite(const Sprite& sprite, glm::vec3 pozicija, glm::vec2 velikost, bool flip_h, glm::vec4 barva, float rotacija)
{
    if (m_batch_verts.size() + 4 > BATCH_VERTICES)
        flush_batch();

    int index_teksture = dobi_slot_teksture(*sprite.tekstura());

    glm::vec2 min_uv = sprite.min_uv();
    glm::vec2 max_uv = sprite.max_uv();
    if (flip_h)
        std::swap(min_uv.x, max_uv.x);

    vstavi_quad(pozicija, velikost, min_uv, max_uv, barva, index_teksture, rotacija);
}

float zacni_frame()
{
    glfwPollEvents();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float prejsni_cas = glfwGetTime();
    float nov_cas = glfwGetTime();
    float delta_time = nov_cas - prejsni_cas;
    prejsni_cas = nov_cas;

    if (delta_time > 1.0f / 30.0f)
        printf("WARNING: frame time %.2fms\n", delta_time * 1000.0f);
    if (delta_time > 1.0f / 15.0f)
        delta_time = 1.0f / 15.0f;
    return delta_time;
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

