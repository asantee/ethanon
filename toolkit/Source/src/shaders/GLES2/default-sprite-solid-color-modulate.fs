#version 330 core
out vec4 outFragColor;

in vec4 outColor;
in vec2 outTexCoord;

uniform sampler2D diffuse;
uniform sampler2D secondary;
uniform vec4 solidColor;

void main()
{
	vec4 blendedColor = texture(diffuse, outTexCoord) * texture(secondary, outTexCoord) * outColor;
	outFragColor = mix(blendedColor, vec4(solidColor.xyz, blendedColor.w), solidColor.w);
}
