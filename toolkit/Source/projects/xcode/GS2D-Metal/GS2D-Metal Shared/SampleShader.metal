#include <metal_stdlib>

using namespace metal;

struct Vertex
{
	float4 position [[position]];
	float4 color;
};

vertex Vertex vertex_main(const device Vertex* vertices [[buffer(0)]], uint vid [[vertex_id]])
{
	return vertices[vid];
}

fragment float4 fragment_main(Vertex inVertex [[stage_in]])
{
	return inVertex.color;
}
