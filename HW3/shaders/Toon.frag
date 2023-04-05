#version 330 core

in vec2 texCoord;
in vec3 N;
in vec3 L;

uniform sampler2D ourTexture;

uniform vec3 Kd;

out vec4 fragColor;

void main()
{
	vec3 objectColor = texture2D(ourTexture, texCoord).rgb;
	float cosDelta = dot(N, L)/(length(N)*length(L));
	vec3 intensity = vec3(1.0, 1.0, 1.0);
	if (cosDelta < 0)
		intensity = vec3(0.1, 0.1, 0.1);
	else if (cosDelta > 0.9)
		intensity = vec3(1, 1, 1);
	else
		intensity = vec3(0.5, 0.5, 0.5);
	vec3 color = Kd * objectColor * intensity; 
	fragColor = vec4(color, 1.0);
}