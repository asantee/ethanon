attribute vec3 vPosition;
attribute vec3 v1;
attribute vec2 vTexCoord;

varying vec4 v_color;
varying vec2 v_texCoord;

uniform mat4 rotationMatrix;

uniform vec2 params[13];

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
#define screenSize params[10]

vec4 transformSprite(vec3 position)
{
	vec4 newPos = vec4(position, 1.0);
	newPos = newPos * vec4(size, 1.0, 1.0) - vec4(center, 0.0, 0.0);

	newPos = vec4(newPos.x * rotationMatrix[0][0] + newPos.y * rotationMatrix[1][0],
				 -newPos.x * rotationMatrix[1][0] + newPos.y * rotationMatrix[0][0],
				  newPos.z,
				  newPos.w);

	vec2 halfScreenSize = screenSize / 2.0;

	newPos += vec4(entityPos.x, entityPos.y, 0.0, 0.0);
	newPos -= vec4(halfScreenSize.x, halfScreenSize.y, 0.0, 0.0);
	newPos -= vec4(cameraPos.x, cameraPos.y, 0.0, 0.0);

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
	gl_Position = transformSprite(vec3(vPosition.x, vPosition.y, depth.x)); 
	v_color = vec4(colorRG, colorBA);
	v_texCoord = transformCoord(vTexCoord);
}