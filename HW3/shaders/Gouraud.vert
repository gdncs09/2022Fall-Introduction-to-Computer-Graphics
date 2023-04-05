#version 330 core

// TODO:
// Implement Gouraud shading

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 M;
uniform mat4 view;
uniform mat4 prespective;

uniform vec3 cameraPos;
uniform vec3 lightPos;

//Model Material
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float gloss;

//Light
uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;

out vec2 texCoord;
out vec3 ambient;
out vec3 diffuse;
out vec3 specular;

void main()
{
	gl_Position = prespective*view*M*vec4(aPos, 1.0);

	vec3 worldPos = vec3(M * vec4(aPos, 1.0));
	vec3 normal = mat3(transpose(inverse(M)))*aNormal;
	
	texCoord = aTexCoord;
	vec3 N = normalize(normal);
	vec3 L = normalize(lightPos-worldPos);
	vec3 V = normalize(cameraPos-worldPos);
	vec3 R = reflect(-L,N);

	ambient = La * Ka;
	diffuse = Ld * Kd * max(dot(L, N),0.0);
	specular = Ls * Ks * pow(float(max(dot(V, R),0.0)), gloss);
}