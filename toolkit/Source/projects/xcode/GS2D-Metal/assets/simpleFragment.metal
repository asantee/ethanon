fragment float4 fragment_main(
	Vertex inVertex [[stage_in]],
	constant Uniforms& uniforms [[buffer(1)]])
{
	return inVertex.texCoord * uniforms.color;
}
