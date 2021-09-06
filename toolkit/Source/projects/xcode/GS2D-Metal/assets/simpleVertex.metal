#include <metal_stdlib>

using namespace metal;

struct Vertex
{
	float4 position [[position]];
	float4 texCoord;
	float4 normal;
};

vertex Vertex vertex_main(const device Vertex* vertices [[buffer(0)]], uint vid [[vertex_id]])
{
	return vertices[vid];
}
