#version 330 core

layout (location = 0) in vec2 a_Vertex;
layout (location = 1) in vec2 a_UV;

out vec2 pass_UV;

void main()
{
	gl_Position = vec4(a_Vertex, 0.f, 1.f);

	pass_UV = a_UV;
}
