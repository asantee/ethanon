precision mediump float;


varying vec4 outColor;
varying vec2 outTexCoord;

uniform sampler2D diffuse;
uniform sampler2D secondary;
uniform vec4 solidColor;

void main()
{
	vec4 blendedColor = (texture2D(diffuse, outTexCoord) * outColor) + vec4(texture2D(secondary, outTexCoord).xyz, 0.0);
	gl_FragColor = mix(blendedColor, vec4(solidColor.xyz, blendedColor.w), solidColor.w);
}
