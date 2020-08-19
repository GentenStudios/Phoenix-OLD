#version 330 core

in vec3 a_Pos;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * vec4(a_Pos, 1.0);
}