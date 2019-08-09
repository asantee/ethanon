#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;

out vec4 outColor;
out vec2 outTexCoord;

uniform vec4 u[4];

#define SIZE_ORIGIN 0
#define SPRITEPOS_VIRTUALTARGETRESOLUTION 1
#define COLOR 2
#define RECTPOS_RECTSIZE 3

void main()
{
	vec4 vertexPos = vec4(inPosition, 1.0);

	vec2 size   = vec2(u[SIZE_ORIGIN].x, u[SIZE_ORIGIN].y);
	vec2 origin = vec2(u[SIZE_ORIGIN].z, u[SIZE_ORIGIN].w);

	vec2 spritePos = vec2(u[SPRITEPOS_VIRTUALTARGETRESOLUTION].x, u[SPRITEPOS_VIRTUALTARGETRESOLUTION].y);
	vec2 virtualTargetResolution = vec2(u[SPRITEPOS_VIRTUALTARGETRESOLUTION].z, u[SPRITEPOS_VIRTUALTARGETRESOLUTION].w);

	vec2 rectPos  = vec2(u[RECTPOS_RECTSIZE].x, u[RECTPOS_RECTSIZE].y);
	vec2 rectSize = vec2(u[RECTPOS_RECTSIZE].z, u[RECTPOS_RECTSIZE].w);

	// scale to size and flip y-axis
	size.y *= -1.0;
	vertexPos = vertexPos * vec4(size.x, size.y, 1.0, 1.0);

	// adjust origin
	vertexPos = vertexPos - vec4(origin * size, 0.0, 0.0);

	// translate
	vertexPos = vertexPos + vec4(spritePos.x,-spritePos.y, 0.0, 0.0);

	// adjust axis to traditional 2D
	vec2 halfScreenSize = virtualTargetResolution / 2.0;
	vertexPos = vertexPos + vec4(-halfScreenSize.x, halfScreenSize.y, 0.0, 0.0);

	// scale to match fixed virtual
	vertexPos.x /= halfScreenSize.x;
	vertexPos.y /= halfScreenSize.y;

	// map sprite rect
	vec2 texCoord = inTexCoord * rectSize;
	texCoord = texCoord + rectPos;
	outTexCoord = texCoord;

	gl_Position = vertexPos;
	outColor = u[COLOR];
}
