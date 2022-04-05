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
#define RECTPOS_RECTSIZE 3

struct Uniforms
{
	vec4 u[4];
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

	vec2 rectPos  = vec2(uniforms.u[RECTPOS_RECTSIZE].x, uniforms.u[RECTPOS_RECTSIZE].y);
	vec2 rectSize = vec2(uniforms.u[RECTPOS_RECTSIZE].z, uniforms.u[RECTPOS_RECTSIZE].w);

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

	vout.position = vertexPos;
	vout.texCoord = vec4((vout.texCoord.xy * rectSize) + rectPos, 1.0f, 1.0f);
	return vout;
}
