attribute vec4 vPosition;
attribute vec2 vTexCoord;

varying vec4 v_color;
varying vec2 v_texCoord;

uniform vec2 params[8];

#define rectPos params[0]
#define rectSize params[1]
#define size params[2]
#define entityPos params[3]
#define bitmapSize params[4]
#define screenSize params[5]
#define colorRG params[6]
#define colorBA params[7]

vec4 transformSprite(vec3 position)
{
	vec4 newPos = vec4(position, 1.0) * vec4(size, 1.0, 1.0);
	newPos += vec4(entityPos, 0.0, 0.0)-vec4(screenSize/2.0, 0.0, 0.0);
	newPos *= vec4(1.0, -1.0, 1.0, 1.0);
	return vec4(newPos.x / (screenSize.x * 0.5), newPos.y / (screenSize.y * 0.5), newPos.z, newPos.w);
}

vec2 transformCoord(vec2 texCoord)
{
	vec2 newCoord = texCoord * (rectSize/bitmapSize);
	return newCoord + (rectPos/bitmapSize);
}

void main()
{
	gl_Position = transformSprite(vec3(vPosition.x, vPosition.y, vPosition.z));
	v_color = vec4(colorRG.x, colorRG.y, colorBA.x, colorBA.y);
	v_texCoord = transformCoord(vTexCoord);
}