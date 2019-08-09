precision lowp float;


varying vec4 outColor;
varying vec2 outTexCoord;

uniform sampler2D diffuse;
uniform sampler2D secondary;
uniform vec4 solidColor;

void main()
{
	vec4 blendedColor = texture2D(diffuse, outTexCoord) * texture2D(secondary, outTexCoord) * outColor;
	gl_FragColor = mix(blendedColor, vec4(solidColor.xyz, blendedColor.w), solidColor.w);
}
