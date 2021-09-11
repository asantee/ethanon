fragment float4 fragment_main(
	Vertex inVertex [[stage_in]],
	constant Uniforms& uniforms [[buffer(1)]],
	texture2d<half> diffuse [[ texture(0) ]])
{
	constexpr sampler colorSampler(mip_filter::linear, mag_filter::linear, min_filter::linear);
	half4 colorSample = diffuse.sample(colorSampler, inVertex.texCoord.xy);

    return float4(colorSample) * uniforms.u[COLOR];
}
