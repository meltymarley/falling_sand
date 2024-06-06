#include <span>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <unordered_map>
#include <array>

#include "debug.hpp"

#include "glsl/sand.hpp"

// Reads vertex & fragment shaders, builds shader program, then returns its ID.
auto buildShaderProgram(const std::string &vertex, const std::string &fragment)
    -> unsigned
{
    const char *vertex_code { vertex.c_str() };
    const char *fragment_code { fragment.c_str() };

    unsigned vertex_shader { glCreateShader(GL_VERTEX_SHADER) };
    glShaderSource(vertex_shader, 1, &vertex_code, nullptr);
    glCompileShader(vertex_shader);

    unsigned fragment_shader { glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragment_shader, 1, &fragment_code, nullptr);
    glCompileShader(fragment_shader);

    unsigned shader_program { glCreateProgram() };
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

// Initializes and binds VBO, VAO, and EBO for a square sprite.
void createSquareSpriteBuffers()
{
    constexpr std::size_t vertices_count { 12 };
    constexpr std::array<float, vertices_count> square_vertices {
        0.0f, -1.0f, 0.0f, // Top-left
        1.0f, -1.0f, 0.0f, // Top-right
        1.0f, 0.0f,  0.0f, // Bottom-right
        0.0f, 0.0f,  0.0f  // Bottom-left
    };

    constexpr std::size_t indices_count { 6 };
    constexpr std::array<unsigned, indices_count> square_indices {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    unsigned VBO {};
    unsigned VAO {};
    unsigned EBO {};
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(square_vertices),
        square_vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(square_indices),
        square_indices.data(),
        GL_STATIC_DRAW);

    constexpr int floats_per_vertex { 3 };
    constexpr unsigned stride { floats_per_vertex * sizeof(float) };
    glVertexAttribPointer(
        0,
        floats_per_vertex,
        GL_FLOAT,
        GL_FALSE,
        stride,
        (void *)nullptr);

    glEnableVertexAttribArray(0);
}

static constexpr std::size_t INPUT_KEYS_COUNT { 349 };
struct Settings
{
    float width;
    float height;
    float sprite_width;
    int columns;
    int rows;
    std::unordered_map<std::string, unsigned> shaders;
    std::span<bool, INPUT_KEYS_COUNT> input;
    std::span<bool, INPUT_KEYS_COUNT> input_processed;
};

void keyCallback(GLFWwindow *window, int key, int, int action, int)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    Settings *settings { static_cast<Settings *>(
        glfwGetWindowUserPointer(window)) };

    if (action == GLFW_PRESS)
    {
        settings->input[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        settings->input[key]           = false;
        settings->input_processed[key] = false;
    }
}

void mouseCallback(GLFWwindow *window, int button, int action, int)
{
    Settings *settings { static_cast<Settings *>(
        glfwGetWindowUserPointer(window)) };

    if (action == GLFW_PRESS)
    {
        settings->input[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        settings->input[button]           = false;
        settings->input_processed[button] = false;
    }
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    Settings *settings { static_cast<Settings *>(
        glfwGetWindowUserPointer(window)) };

    settings->width  = static_cast<float>(width);
    settings->height = static_cast<float>(height);

    glViewport(0, 0, width, height);
}

// Initializes GLFW, creates and returns a pointer to the GLFW window.
auto initializeGlfw(Settings &settings, const char *title) -> GLFWwindow *
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!\n";
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Enables GLFW debug context if DEBUG_BUILD is set.
    DEBUG_OPENGL_CONTEXT();

    GLFWwindow *window { glfwCreateWindow(
        static_cast<int>(settings.width),
        static_cast<int>(settings.height),
        title,
        nullptr,
        nullptr) };
    if (!window)
    {
        std::cerr << "Failed to create the GLFW window!\n";
        std::exit(-1);
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // V-Sync: ON

    glfwSetWindowUserPointer(window, &settings);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    return window;
}

// Checks if glad was successfully initialized and sets glDebugMessageCallback.
void initializeGlad()
{
    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize GLAD!\n";
        std::exit(-1);
    }

    // Outputs renderer name and supported OpenGL version if DEBUG_BUILD is set.
    DEBUG_OPENGL_INFO();

    // Enables GLFW debug context messages if DEBUG_BUILD is set.
    int flags {};
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        DEBUG_OPENGL_OUTPUT();
        glDebugMessageControl(
            GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            nullptr,
            GL_TRUE);
    }
}

// Loads shaders and creates OpenGL vertex buffers.
void initializeData(Settings *const settings)
{
    createSquareSpriteBuffers();

    settings->shaders["sand"] = buildShaderProgram(sand_vert, sand_frag);
}

// Draws a square sprite at specified position.
void drawSprite(Settings &settings, const char *sprite, float x, float y)
{
    constexpr float scale_factor { 2.0f };
    constexpr float offset { 1.0f };

    float x_top_left { (scale_factor * x) / settings.width - offset };
    float y_top_left { offset - (scale_factor * y) / settings.height };
    float width { (scale_factor * settings.sprite_width) / settings.width };
    float height { (scale_factor * settings.sprite_width) / settings.height };

    glm::mat4 model { glm::mat4(1.0f) };
    model = glm::translate(model, glm::vec3(x_top_left, y_top_left, 1.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    glUseProgram(settings.shaders[sprite]);

    glUniformMatrix4fv(
        glGetUniformLocation(settings.shaders[sprite], "model"),
        1,
        GL_FALSE,
        glm::value_ptr(model));

    constexpr int indices_count { 6 };
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
}

void handleInput(Settings &settings) {}

void updateAndRender(Settings &settings)
{
    handleInput(settings);

    drawSprite(settings, "sand", 0, 0);
    drawSprite(settings, "sand", settings.sprite_width, settings.sprite_width);
}

// TODO: Input with mouse & keyboard.
auto main() -> int
{
    constexpr float screen_width { 1280 };
    constexpr float screen_height { 720 };
    constexpr float sprite_width { 10 };

    // The size of these arrays is big enough to hold both mouse buttons state
    // and keyboard keys state.
    // glfw.org/docs/3.3/group__buttons.html
    // glfw.org/docs/3.3/group__keys.html
    std::array<bool, INPUT_KEYS_COUNT> input {};
    std::array<bool, INPUT_KEYS_COUNT> input_processed {};

    Settings settings {
        .width        = screen_width,
        .height       = screen_height,
        .sprite_width = sprite_width,
        .columns = static_cast<int>(settings.width / settings.sprite_width),
        .rows    = static_cast<int>(settings.height / settings.sprite_width),
        .shaders {},
        .input { input },
        .input_processed { input_processed }
    };

    GLFWwindow *window { initializeGlfw(settings, "Falling sand") };

    initializeGlad();

    initializeData(&settings);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        updateAndRender(settings);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
