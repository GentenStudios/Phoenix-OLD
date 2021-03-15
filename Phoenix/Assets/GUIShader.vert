#version 330 core

layout (location = 0) in vec2 a_Vertex;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_UV;

uniform mat4 u_OrthoProj;

out vec4 pass_Color;
out vec2 pass_UV;

void main()
{
	gl_Position = vec4(a_Vertex, 0.f, 1.f);
	pass_Color = a_Color;
	pass_UV = a_UV;
}
