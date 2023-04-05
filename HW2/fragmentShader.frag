#version 330 core
// TO DO:
// Implement fragment shader

in vec2 texCoord;

uniform sampler2D ourTexture;
uniform vec3 catColor;
uniform vec3 boxColor;

out vec4 fragColor;

void main()
{
	vec4 sample = texture2D(ourTexture, texCoord);
	float intensity = (sample.r + sample.g + sample.b + sample.a)*0.5;
	if (intensity > 1)
	{
		fragColor = vec4(sample.r*boxColor.x, sample.g*boxColor.y, sample.b*boxColor.z, 1);
	}
	else
	{
		fragColor = vec4(sample.r*catColor.x, sample.g*catColor.y, sample.b*catColor.z, 1);
	}		
}