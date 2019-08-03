#version 330 core
out vec4 outFragColor;

in vec4 outColor;
in vec2 outTexCoord;

uniform sampler2D diffuse;

void main()
{
	outFragColor = texture(diffuse, outTexCoord) * outColor;
}
