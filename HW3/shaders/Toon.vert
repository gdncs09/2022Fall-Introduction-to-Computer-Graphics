#version 330 core

// TODO:
// Implement Toon shading

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 M;
uniform mat4 view;
uniform mat4 prespective;

uniform vec3 lightPos;

out vec2 texCoord;
out vec3 N;
out vec3 L;

void main()
{
	gl_Position = prespective*view*M*vec4(aPos, 1.0);
	vec3 worldPos = vec3(M * vec4(aPos, 1.0));
	vec3 normal = mat3(transpose(inverse(M)))*aNormal;
	texCoord = aTexCoord;

	N = normalize(normal);
	L = normalize(lightPos-worldPos);
}