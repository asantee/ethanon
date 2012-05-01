#define float4 vec4
#define float3 vec3
#define float2 vec2

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

uniform float2 parallaxOrigin;
uniform float4 entityPos3D_parallaxIntensity;
uniform float spaceLength;

float2 computeParallaxOffset()
{
	float2 screenSpacePos = float2(entityPos3D_parallaxIntensity.x, entityPos3D_parallaxIntensity.y) - cameraPos;
	return ((screenSpacePos - parallaxOrigin) / screenSize.x) * entityPos3D_parallaxIntensity.z * entityPos3D_parallaxIntensity.w;
}

float4 transformSprite(float3 position)
{
	float4 newPos = float4(position, 1.0);
	newPos = newPos * float4(size, 1.0, 1.0) - float4(center, 0.0, 0.0);
	newPos = (rotationMatrix * newPos);
	newPos += float4(entityPos, 0.0, 0.0) - float4(screenSize / 2.0, 0.0, 0.0) - float4(cameraPos, 0.0, 0.0)
		+ float4(computeParallaxOffset(), 0.0, 0.0);
	newPos *= float4(1.0,-1.0, 1.0, 1.0);
	return (viewMatrix * newPos);
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
	outPos.z = (1.0 - depth.x) - (((1.0 - vPosition.y) * rectSize.y) / spaceLength);
	gl_Position = outPos;
	v_color = float4(colorRG, colorBA);
	v_texCoord = transformCoord(vTexCoord);
}
