#version 330 core

in vec3 pass_UV;
in vec3 pass_Normal;

uniform sampler2DArray u_TexArray;
uniform float u_AmbientStrength;
uniform vec3 u_LightDir;
uniform float u_Brightness;

out vec4 out_FragColor;

void main()
{
	const vec3 lightColor = vec3(1.0, 1.0, 1.0);

	// -- ambient lighting --
	vec3 ambient = u_AmbientStrength * lightColor;

	// -- diffuse lighting --

	// assuming we don't need normalize(pass_normal) because
	// vectors are pre normalized
	vec3 norm = pass_Normal;
	vec3 lightDir = normalize(-u_LightDir);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// -- output color --

	vec4 lightingResult = (vec4(ambient, 1.0) + vec4(diffuse, 1.0)) * u_Brightness;
	out_FragColor = lightingResult * texture(u_TexArray, pass_UV);
}
