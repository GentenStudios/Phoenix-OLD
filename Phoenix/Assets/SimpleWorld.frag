#version 330 core

in vec3 pass_UV;

uniform sampler2DArray u_TexArray;

out vec4 out_FragColor;

void main()
{
	out_FragColor = texture(u_TexArray, pass_UV);
}