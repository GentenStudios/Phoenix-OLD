#version 330 core

in vec3 pass_Position;
in vec3 pass_TexCoord;
out vec4 out_FragColor;
uniform sampler2D u_Sampler;
uniform sampler2DArray u_Textures;

void main()
{
    out_FragColor = texture(u_Textures, pass_TexCoord);
} 
