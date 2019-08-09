precision lowp float;


varying vec4 outColor;
varying vec2 outTexCoord;

uniform sampler2D diffuse;
uniform sampler2D secondary;

void main()
{
	gl_FragColor = (texture2D(diffuse, outTexCoord) * outColor) + vec4(texture2D(secondary, outTexCoord).xyz, 0.0);
}
