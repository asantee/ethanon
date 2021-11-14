precision lowp float;


varying vec4 outColor;
varying vec2 outTexCoord;

uniform sampler2D diffuse;
uniform vec4 solidColor;
uniform vec4 highlight;

void main()
{
	vec4 blendedColor = texture2D(diffuse, outTexCoord) * highlight;
	gl_FragColor = mix(blendedColor, vec4(solidColor.xyz, blendedColor.w), solidColor.w);
}
