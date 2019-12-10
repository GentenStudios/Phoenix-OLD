#version 330 core

in vec3 pass_Position;
in vec2 pass_TexCoord;
out vec4 out_FragColor;
uniform sampler2D u_Sampler;

void main()
{
    out_FragColor = texture(u_Sampler, pass_TexCoord);
} 
