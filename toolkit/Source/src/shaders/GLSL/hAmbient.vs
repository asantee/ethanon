#define float4 vec4
#define float3 vec3
#define float2 vec2

attribute vec4 vPosition;
attribute vec2 vTexCoord;

varying vec4 v_color;
varying vec2 v_texCoord;


uniform mat4 viewMatrix;
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




float2 computeParallaxOffset(float2 vertPos)
{
	return ((vertPos - parallaxOrigin) / screenSize.x) * entityPos3DZ_parallaxIntensity.x * entityPos3DZ_parallaxIntensity.y;
}

float4 transformSprite(float3 position)
{
	float4 newPos = float4(position, 1.0) * vec4(flipMul, 1.0, 1.0) + vec4(flipAdd, 0.0, 0.0);
	newPos = newPos * float4(size, 1.0, 1.0) - float4(center, 0.0, 0.0);

	newPos = vec4(newPos.x * rotationMatrix[0][0] + newPos.y * rotationMatrix[1][0],
				 -newPos.x * rotationMatrix[1][0] + newPos.y * rotationMatrix[0][0],
				  newPos.z,
				  newPos.w);

	vec2 halfScreenSize = screenSize / 2.0;

	newPos += vec4(entityPos.x, entityPos.y, 0.0, 0.0);
	newPos -= vec4(cameraPos.x, cameraPos.y, 0.0, 0.0);

	newPos += float4(computeParallaxOffset(float2(newPos.x,newPos.y)), 0.0, 0.0);

	newPos -= vec4(halfScreenSize.x, halfScreenSize.y, 0.0, 0.0);

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
	outPos.z = 1.0 - depth.x;
	gl_Position = outPos;
	v_color = float4(colorRG, colorBA);
	v_texCoord = transformCoord(vTexCoord);
}
