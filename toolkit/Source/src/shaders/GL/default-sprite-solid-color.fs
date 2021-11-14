#version 330 core
out vec4 outFragColor;

in vec4 outColor;
in vec2 outTexCoord;

uniform sampler2D diffuse;
uniform vec4 solidColor;
uniform vec4 highlight;

void main()
{
	vec4 blendedColor = texture(diffuse, outTexCoord) * highlight;
	outFragColor = mix(blendedColor, vec4(solidColor.xyz, blendedColor.w), solidColor.w);
}
