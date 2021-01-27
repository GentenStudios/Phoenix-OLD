#version 330 core

layout (location = 0) in vec3 a_Vertex;
layout (location = 1) in vec2 a_UV;

uniform float u_TexW;
uniform float u_TexH;
uniform float u_ScreenW;
uniform float u_ScreenH;

out vec2 pass_UV;

void main()
{
	vec2 pos = vec2(a_Vertex.x * u_TexW, a_Vertex.y * u_TexH);
	pos /= vec2(u_ScreenW, u_ScreenH);
	gl_Position = vec4(pos, 0.f, 1.f);
	pass_UV = a_UV;
}
