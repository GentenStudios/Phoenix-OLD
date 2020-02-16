#version 330 core

in vec2 pass_UV;

uniform sampler2D u_Tex;
uniform float u_TexAlpha;

out vec4 out_FragColor;

void main()
{
	vec4 tex = texture(u_Tex, pass_UV);
	out_FragColor = vec4(tex.rgb, u_TexAlpha * tex.a);
}