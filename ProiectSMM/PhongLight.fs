#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float Ka;
uniform float Kd;
uniform float n;
uniform float Ks;

void main()
{
    vec3 norm=normalize(Normal);
    vec3 lightDir=normalize(lightPos-FragPos);
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,norm);
    vec3 ambient=Ka*lightColor;
    float diff=Kd*max(dot(norm,lightDir),0.0);
    float spec=Ks*pow(max(dot(viewDir,reflectDir),0.0),n);
    vec3 result=(ambient+diff+spec)*objectColor;
    FragColor = vec4(result, 1.0);
}