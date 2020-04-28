#version 330 core

in vec3 position;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * vec4(position, 1.0);
}