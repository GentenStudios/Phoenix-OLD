#version 330 core

in vec3 pass_UV;
in vec3 pass_normal;
flat in uint pass_color;
in vec3 pass_pos;

uniform sampler2DArray u_TexArray;
uniform float u_AmbientStrength;
uniform vec3 u_LightPos;

out vec4 out_FragColor;

void main()
{
	int color_i = int(pass_color);
	vec4 objectColor = vec4(
		float((color_i & 4278190080) >> 24) / 255.0,
		float((color_i & 16711680)   >> 16) / 255.0,
		float((color_i & 65280)      >> 8 ) / 255.0,
		float((color_i & 255)             ) / 255.0
	);

	const vec3 lightColor = vec3(1.0, 1.0, 1.0);

	// -- ambient lighting --
	vec3 ambient = u_AmbientStrength * lightColor;

	// -- diffuse lighting --

	// assuming we don't need normalize(pass_normal) because
	// vectors are pre normalized
	vec3 norm = normalize(pass_normal);
	vec3 lightDir = normalize(u_LightPos - pass_pos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// -- output color --

	vec4 result = vec4((ambient + diffuse), 1.0) * objectColor;
	out_FragColor = result * texture(u_TexArray, pass_UV);
}
