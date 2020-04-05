#version 330 core

in vec3 pass_UV;
in vec3 pass_normal;
in vec3 pass_Color;

uniform sampler2DArray u_TexArray;

out vec4 out_FragColor;

void main()
{	
	float normX = abs(pass_normal.x);
	float normY = abs(pass_normal.y);
	float normZ = abs(pass_normal.z);
	if(normX > 0.9 && normX < 1.1)
		out_FragColor = texture(u_TexArray, pass_UV) * vec4(vec3(0.4), 1);
	if(normY > 0.9 && normY < 1.1)
		out_FragColor = texture(u_TexArray, pass_UV) * vec4(vec3(1), 1);
	if(normZ > 0.9 && normZ < 1.1)
		out_FragColor = texture(u_TexArray, pass_UV) * vec4(vec3(0.7), 1);

	out_FragColor.rgb *= pass_Color;
}
