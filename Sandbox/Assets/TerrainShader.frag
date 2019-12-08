#version 330 core

in vec3 pass_Position;
out vec4 out_FragColor;

void main()
{
    out_FragColor = vec4(pass_Position.xyz + 0.5f, 1.0f);
} 
