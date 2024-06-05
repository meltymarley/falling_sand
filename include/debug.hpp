#ifndef LOGGING_HPP
#define LOGGING_HPP

#ifdef DEBUG_BUILD

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#define DEBUG_OPENGL_CONTEXT() glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

#define DEBUG_OPENGL_INFO()                                                    \
    std::cout << "Renderer:\t\t\t" << glGetString(GL_RENDERER)                 \
              << "\nSupported OpenGL version:\t" << glGetString(GL_VERSION)    \
              << "\n\n";

#define DEBUG_LOG(message) std::cout << "DEBUG: " << message << '\n';

#define DEBUG_WATCH(value)                                                     \
    std::cout << "DEBUG: " << #value << " = " << (value) << '\n';

inline unsigned glCheckError_(const char *file, int line)
{
    unsigned error_code {};
    while ((error_code = glGetError()) != GL_NO_ERROR)
    {
        const char *error;
        switch (error_code)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }

        std::cout << error << " | " << file << " (" << line << ")\n";
    }

    return error_code;
}

#define DEBUG_CHECK_OPENGL_ERROR() glCheckError(__FILE__, __LINE__);

inline void APIENTRY glDebugOutput(
    unsigned source,
    unsigned type,
    unsigned id,
    unsigned severity,
    int,
    const char *message,
    const void *)
{
    // Ignore non-significant error/warning codes.
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    {
        return;
    }

    std::cout << "------------------\n";
    std::cout << "Debug message (" << id << "): " << message << '\n';

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        std::cout << "Source: API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cout << "Source: Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cout << "Source: Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cout << "Source: Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cout << "Source: Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cout << "Source: Other";
        break;
    }
    std::cout << '\n';

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "Type: Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "Type: Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "Type: Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "Type: Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "Type: Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cout << "Type: Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cout << "Type: Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cout << "Type: Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "Type: Other";
        break;
    }
    std::cout << '\n';

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "Severity: high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "Severity: medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "Severity: low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << "Severity: notification";
        break;
    }
    std::cout << "\n\n";
}

#define DEBUG_OPENGL_OUTPUT() glDebugMessageCallback(glDebugOutput, nullptr);

#else
#define DEBUG_OPENGL_CONTEXT()
#define DEBUG_OPENGL_INFO()
#define DEBUG_LOG(message)
#define DEBUG_WATCH(value)
#define DEBUG_CHECK_OPENGL_ERROR()
#define DEBUG_OPENGL_OUTPUT()
#endif

#endif // LOGGING_HPP
