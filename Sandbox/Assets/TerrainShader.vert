#version 330 core

in vec3 in_Position;
in vec2 in_TexCoord;

out vec3 pass_Position;
out vec2 pass_TexCoord;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    gl_Position = u_projection * u_view * vec4(in_Position.xyz, 1.0f);
    
    pass_Position = in_Position;
    pass_TexCoord = in_TexCoord;
}
