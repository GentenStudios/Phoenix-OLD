#version 330 core

in vec3 pass_UV;

uniform samplerCube u_Tex;

out vec4 out_FragColor;

void main()
{
	out_FragColor = texture(u_Tex, pass_UV);
}
