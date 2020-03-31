#version 330 core

layout (location = 0) in vec3 a_Vertex;
layout (location = 1) in vec3 a_UV;
layout (location = 2) in vec3 a_Normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 pass_UV;
out vec3 pass_normal;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(a_Vertex, 1.f); // don't need model right now either.
	pass_UV = a_UV;
	pass_normal = a_Normal;
}
