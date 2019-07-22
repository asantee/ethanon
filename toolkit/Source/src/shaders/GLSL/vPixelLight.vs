#define float4 vec4
#define float3 vec3
#define float2 vec2

attribute vec3 vPosition;
attribute vec3 v1;
attribute vec2 vTexCoord;

varying vec4 v_color;
varying vec2 v_texCoord;
varying vec3 v_vertPos3D;

uniform mat4 rotationMatrix;

uniform vec2 params[17];

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
#define flipAdd params[11]
#define flipMul params[12]
#define entityPos3DXY params[13]
#define entityPos3DZ_parallaxIntensity params[14]
#define parallaxOrigin params[15]
#define verticalIntensity params[16]

#define LIGHT_PRECISION_DOWNSCALE 0.00078125

uniform float spaceLength;

uniform float3 topLeft3DPos;

float computeVerticalOffsetInPixels(float posY, float rectSizeY)
{
	return ((1.0 - posY) * rectSizeY);
}

float2 computeParallaxOffset(float2 vertPos, float3 inPosition)
{
	float vOffset = computeVerticalOffsetInPixels(inPosition.y, rectSize.y);
	return ((vertPos - parallaxOrigin) / screenSize.x) * (entityPos3DZ_parallaxIntensity.x + (vOffset * verticalIntensity.x)) * entityPos3DZ_parallaxIntensity.y;
}

float4 transformSprite(float3 position)
{
	float4 newPos = float4(position, 1.0);
	newPos = newPos * float4(size, 1.0, 1.0) - float4(center, 0.0, 0.0);

	newPos += float4(entityPos, 0.0, 0.0) - float4(cameraPos, 0.0, 0.0);
	newPos += float4(computeParallaxOffset(float2(newPos.x, newPos.y), position), 0.0, 0.0) - float4(screenSize / 2.0, 0.0, 0.0);
	newPos *= float4(1.0,-1.0, 1.0, 1.0);
	return vec4(newPos.x / (screenSize.x * 0.5), newPos.y / (screenSize.y * 0.5), newPos.z, newPos.w);
}

float2 transformCoord(float2 texCoord)
{
	float2 newCoord = texCoord * (rectSize / bitmapSize);
	newCoord += (rectPos / bitmapSize);
	return (newCoord);
}

void main()
{
	float4 outPos = transformSprite(vec3(vPosition.x, vPosition.y, vPosition.z)); 
	outPos.z = (1.0 - depth.x) - ((computeVerticalOffsetInPixels(vPosition.y, rectSize.y)) / spaceLength);
	gl_Position = outPos;
	v_color = float4(colorRG, colorBA);
	v_texCoord = transformCoord(vTexCoord);
	v_vertPos3D = (topLeft3DPos + (float3(vPosition.x, 0.0, vPosition.y) * float3(size.x, 0.0,-size.y))) * LIGHT_PRECISION_DOWNSCALE;
}
