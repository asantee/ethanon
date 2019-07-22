attribute vec3 vPosition;
attribute vec3 v1;
attribute vec2 vTexCoord;

varying vec4 v_color;
varying vec2 v_texCoord;

uniform vec2 rectPos;
uniform vec2 rectSize;
uniform vec2 bitmapSize;

uniform mat4 rotationMatrix;
uniform vec2 screenSize;
uniform vec2 size;
uniform vec2 entityPos;
uniform vec2 center;
uniform vec2 cameraPos;

uniform vec2 flipMul;
uniform vec2 flipAdd;

uniform vec4 color0;
uniform vec4 color1;
uniform vec4 color2;
uniform vec4 color3;

uniform float depth;

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

vec4 getVertexColor(vec3 position)
{
	vec4 vertex0 = color0 * (1.0-position.x) * (1.0-position.y);
	vec4 vertex1 = color1 * (position.x) * (1.0-position.y);
	vec4 vertex2 = color2 * (1.0-position.x) * (position.y);
	vec4 vertex3 = color3 * (position.x) * (position.y);
	return vertex0 + vertex1 + vertex2 + vertex3;
}

void main()
{
	gl_Position = transformSprite(vec3(vPosition.x, vPosition.y, depth)); 
	v_color = getVertexColor(vPosition);
	v_texCoord = transformCoord(vTexCoord);
}