#version 330 core

in vec2 pass_UV;

uniform sampler2DArray u_TexArray;
uniform float u_Layer;

out vec4 out_FragColor;

void main()
{
	out_FragColor = texture(u_TexArray, vec3(pass_UV, u_Layer));
}
