#version 330 core

in vec4 pass_Color;
in vec2 pass_UV;

uniform sampler2D u_Tex;

out vec4 out_FragColor;

void main()
{
	out_FragColor = vec4(pass_Color.rgb / 255.f, pass_Color.a);
}
