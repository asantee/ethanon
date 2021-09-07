#include <metal_stdlib>

using namespace metal;

struct Vertex
{
	float4 position [[position]];
	float4 texCoord;
	float4 normal;
};

struct Uniforms
{
	float4 SIZE_ORIGIN;
	float4 SPRITEPOS_VIRTUALTARGETRESOLUTION;
	float2 posOffset;
	float4 COLOR;
	float4 FLIPADD_FLIPMUL;
	float4 RECTPOS_RECTSIZE;
	float4 color;
	float4 ANGLE_PARALLAXINTENSITY_ZPOS;
};

vertex Vertex vertex_main(
	const device Vertex* vertices [[buffer(0)]],
	constant Uniforms& uniforms [[buffer(1)]],
	uint vid [[vertex_id]])
{
	Vertex vout = vertices[vid];
	vout.position.y *= -1.0f;
	vout.position += float4(uniforms.posOffset.x, uniforms.posOffset.y, 0.0f, 0.0f);
	return vout;
}
