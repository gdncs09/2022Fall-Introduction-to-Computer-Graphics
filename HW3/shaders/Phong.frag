#version 330 core

in vec2 texCoord;
in vec3 N;
in vec3 L;
in vec3 V;
in vec3 R;

uniform sampler2D ourTexture;

//Model Material
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float gloss;

//Light
uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;

out vec4 fragColor;

void main()
{
	vec3 objectColor = texture2D(ourTexture, texCoord).rgb;
	vec3 ambient = La * Ka;
	vec3 diffuse = Ld * Kd * max(dot(L, N),0.0);
	vec3 specular = Ls * Ks * pow(float(max(dot(V, R),0.0)), gloss);
	vec3 color = (ambient + diffuse)*objectColor + specular;
	fragColor = vec4(color,1.0);
}