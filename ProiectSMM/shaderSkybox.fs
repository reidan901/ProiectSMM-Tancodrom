#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform float mixValue=0.5;
uniform samplerCube skybox;
uniform samplerCube skybox2;

void main()
{    
    FragColor = mix(texture(skybox, TexCoords), texture(skybox2,TexCoords),mixValue);
}