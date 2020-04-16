precision lowp float;


varying vec4 outColor;
varying vec2 outTexCoord;

uniform sampler2D diffuse;
uniform vec4 highlight;

void main()
{
	gl_FragColor = texture2D(diffuse, outTexCoord) * highlight;
}
