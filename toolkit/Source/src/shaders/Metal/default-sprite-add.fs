fragment float4 fragment_main(
	Vertex inVertex [[stage_in]],
	constant Uniforms& uniforms [[buffer(1)]],
	texture2d<half> diffuse [[ texture(0) ]],
	texture2d<half> secondary [[ texture(1) ]])
{
	constexpr sampler colorSampler(mip_filter::linear, mag_filter::linear, min_filter::linear, address::clamp_to_edge);
	half4 colorSample = diffuse.sample(colorSampler, inVertex.texCoord.xy);
	half4 addSample = secondary.sample(colorSampler, inVertex.texCoord.xy);

	return (float4(colorSample) * uniforms.u[COLOR]) + float4(addSample.x, addSample.y, addSample.z, 0.0f);
}
