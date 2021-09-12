fragment float4 fragment_main(
	Vertex inVertex [[stage_in]],
	constant Uniforms& uniforms [[buffer(1)]],
	texture2d<half> diffuse [[ texture(0) ]])
{
	constexpr sampler colorSampler(mip_filter::linear, mag_filter::linear, min_filter::linear, address::clamp_to_edge);
	half4 colorSample = diffuse.sample(colorSampler, inVertex.texCoord.xy);

	vec4 blendedColor = vec4(colorSample) * uniforms.u[COLOR];
	return mix(blendedColor, vec4(uniforms.solidColor.x, uniforms.solidColor.y, uniforms.solidColor.z, blendedColor.w), uniforms.solidColor.w);
}
