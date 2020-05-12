#version 330 core

in vec4 pass_Color;
in vec2 pass_UV;

uniform sampler2D u_Tex;

out vec4 out_FragColor;

void main()
{
	//out_FragColor = texture(u_Tex, pass_UV) * vec4(pass_Color / 255.f);
	out_FragColor = pass_Color;
}
