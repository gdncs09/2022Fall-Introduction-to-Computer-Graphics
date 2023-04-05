#version 330 core

in vec2 texCoord;
in vec3 ambient;
in vec3 diffuse;
in vec3 specular;

uniform sampler2D ourTexture;

out vec4 fragColor;

void main()
{
	vec3 objectColor = texture2D(ourTexture, texCoord).rgb;
	vec3 color = (ambient + diffuse)*objectColor + specular;
	fragColor = vec4(color,1.0);
}