#ifndef SAND_HPP
#define SAND_HPP

constexpr const char *sand_vert { R"(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(position.x, position.y, position.z, 1.0);
}
)" };

constexpr const char *sand_frag { R"(
#version 330 core

out vec4 fragment_color;

void main()
{
    fragment_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)" };

#endif // SAND_HPP
