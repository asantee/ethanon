attribute vec4 vPosition;
attribute vec2 vTexCoord;

varying vec4 v_color;
varying vec2 v_texCoord;

uniform vec2 rectPos;
uniform vec2 rectSize;
uniform vec2 bitmapSize;

uniform mat4 viewMatrix;
uniform mat4 rotationMatrix;
uniform vec2 screenSize;
uniform vec2 size;
uniform vec2 entityPos;
uniform vec2 center;
uniform vec2 cameraPos;

uniform vec4 color0;
uniform vec4 color1;
uniform vec4 color2;
uniform vec4 color3;

uniform float depth;

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

vec4 getVertexColor(vec4 position)
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