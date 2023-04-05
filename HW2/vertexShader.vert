#version 330 core
// TO DO:
// Implement vertex shader
// note: remember to set gl_Position

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 S;

out vec2 texCoord;

void main()
{
	gl_Position = P*V*M*vec4(aPos*S, 1.0);
	texCoord = aTexCoord;
}