#version 330 core

in vec2 texCoord;
in vec3 N;
in vec3 V;

uniform sampler2D ourTexture;

out vec4 fragColor;

void main()
{
	float cosDelta = dot(N, V)/(length(N)*length(V));
	vec3 color = vec3(1, 1, 1);

	if (cosDelta <= 0.1)
		color = vec3(0, 0, 1);
	else if (cosDelta <= 0.2)
		color = vec3(0, 0, 0.3);
	else if (cosDelta <= 0.4)
		color = vec3(0, 0, 0.1);
	else
		color = vec3(0, 0, 0);
	fragColor = vec4(color, 1);
}