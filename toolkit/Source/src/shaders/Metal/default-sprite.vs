#include <metal_stdlib>

using namespace metal;

struct Vertex
{
	float4 position [[position]];
	float4 texCoord;
	float4 normal;
};

typedef float2 vec2;
typedef float3 vec3;
typedef float4 vec4;

#define SIZE_ORIGIN 0
#define SPRITEPOS_VIRTUALTARGETRESOLUTION 1
#define COLOR 2
#define FLIPADD_FLIPMUL 3
#define RECTPOS_RECTSIZE 4
#define ANGLE_PARALLAXINTENSITY_ZPOS 5

struct Uniforms
{
	vec4 u[6];
	vec4 highlight;
	vec4 solidColor;
};

vertex Vertex vertex_main(
	const device Vertex* vertices [[buffer(0)]],
	constant Uniforms& uniforms [[buffer(1)]],
	uint vid [[vertex_id]])
{
	Vertex vout = vertices[vid];
	vec4 vertexPos = vout.position;

	vec2 size   = vec2(uniforms.u[SIZE_ORIGIN].x, uniforms.u[SIZE_ORIGIN].y);
	vec2 origin = vec2(uniforms.u[SIZE_ORIGIN].z, uniforms.u[SIZE_ORIGIN].w);

	vec2 spritePos = vec2(uniforms.u[SPRITEPOS_VIRTUALTARGETRESOLUTION].x, uniforms.u[SPRITEPOS_VIRTUALTARGETRESOLUTION].y);
	vec2 virtualTargetResolution = vec2(uniforms.u[SPRITEPOS_VIRTUALTARGETRESOLUTION].z, uniforms.u[SPRITEPOS_VIRTUALTARGETRESOLUTION].w);

	vec2 flipAdd = vec2(uniforms.u[FLIPADD_FLIPMUL].x, uniforms.u[FLIPADD_FLIPMUL].y);
	vec2 flipMul = vec2(uniforms.u[FLIPADD_FLIPMUL].z, uniforms.u[FLIPADD_FLIPMUL].w);

	vec2 rectPos  = vec2(uniforms.u[RECTPOS_RECTSIZE].x, uniforms.u[RECTPOS_RECTSIZE].y);
	vec2 rectSize = vec2(uniforms.u[RECTPOS_RECTSIZE].z, uniforms.u[RECTPOS_RECTSIZE].w);

	float angle = uniforms.u[ANGLE_PARALLAXINTENSITY_ZPOS].x;
	float parallaxIntensity = uniforms.u[ANGLE_PARALLAXINTENSITY_ZPOS].y;
	float zPos = uniforms.u[ANGLE_PARALLAXINTENSITY_ZPOS].z;

	// apply flip
	vertexPos = vertexPos * vec4(flipMul.x, flipMul.y, 1.0f, 1.0f) + vec4(flipAdd.x, flipAdd.y, 0.0f, 0.0f);

	// scale to size and flip y-axis
	size.y *= -1.0f;
	vertexPos = vertexPos * vec4(size.x, size.y, 1.0f, 1.0f);

	// adjust origin
	vertexPos = vertexPos - vec4(origin * size, 0.0f, 0.0f);

	// rotate
	vertexPos = vec4(
		vertexPos.x * cos(angle) - vertexPos.y * sin(angle),
		vertexPos.x * sin(angle) + vertexPos.y * cos(angle),
		vertexPos.z,
		vertexPos.w);

	// translate
	vertexPos = vertexPos + vec4(spritePos.x,-spritePos.y, 0.0f, 0.0f);

	// compute parallax offset
	vec2 halfScreenSize = virtualTargetResolution / 2.0f;
	vec2 parallaxOffset = ((vertexPos.xy + vec2(-halfScreenSize.x, halfScreenSize.y)) / virtualTargetResolution.y) * parallaxIntensity * zPos;
	vertexPos = vertexPos + vec4(parallaxOffset, 0.0f, 0.0f);

	// adjust axis to traditional 2D
	vertexPos = vertexPos + vec4(-halfScreenSize.x, halfScreenSize.y, 0.0f, 0.0f);

	// scale to match fixed virtual
	vertexPos.x /= halfScreenSize.x;
	vertexPos.y /= halfScreenSize.y;

	// map sprite rect
	vec2 texCoord = vout.texCoord.xy * rectSize;
	texCoord = texCoord + rectPos;

	vout.position = vertexPos;
	vout.texCoord = vec4(texCoord, 1.0f, 1.0f);

	return vout;
}
