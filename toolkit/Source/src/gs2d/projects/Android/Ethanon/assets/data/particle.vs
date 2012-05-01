attribute vec4 vPosition;
attribute vec2 vTexCoord;

varying vec4 v_color;
varying vec2 v_texCoord;

uniform mat4 viewMatrix;
uniform mat4 rotationMatrix;

uniform vec2 screenSize;

uniform vec2 params[10];

#define rectPos params[0]
#define rectSize params[1]
#define center params[2]
#define size params[3]
#define entityPos params[4]
#define cameraPos params[5]
#define bitmapSize params[6]
#define colorRG params[7]
#define colorBA params[8]
#define depth params[9]

vec4 transformSprite(vec3 position)
{
	vec4 newPos = vec4(position, 1.0);
	newPos = newPos * vec4(size, 1.0, 1.0) - vec4(center, 0.0, 0.0);
	newPos = (rotationMatrix * newPos);
	newPos += vec4(entityPos, 0.0, 0.0)-vec4(screenSize/2.0, 0.0, 0.0)-vec4(cameraPos, 0.0, 0.0);
	newPos *= vec4(1.0, -1.0, 1.0, 1.0);
	return (viewMatrix * newPos);
}

vec2 transformCoord(vec2 texCoord)
{
	vec2 newCoord = texCoord * (rectSize/bitmapSize);
	return newCoord + (rectPos/bitmapSize);
}

void main()
{
	gl_Position = transformSprite(vec3(vPosition.x, vPosition.y, 1.0 - depth.x)); 
	v_color = vec4(colorRG, colorBA);
	v_texCoord = transformCoord(vTexCoord);
}