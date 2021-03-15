#version 330 core

layout (location = 0) in vec3 a_Pos;

uniform mat4 u_projection;
uniform mat4 u_view;

out vec3 pass_UV;

void main()
{
    vec4 pos = u_projection * u_view * vec4(a_Pos, 1.0);
    gl_Position = pos.xyww;

    pass_UV = a_Pos;
}
