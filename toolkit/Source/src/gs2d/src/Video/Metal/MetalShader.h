#ifndef GS2D_METAL_SHADER_H_
#define GS2D_METAL_SHADER_H_

#include "../../Shader.h"

#include "../../Platform/FileManager.h"

#include <hopscotch_map.h>

#import <MetalKit/MetalKit.h>

namespace gs2d {

class MetalVideo;

class MetalShader : public Shader
{
	std::string m_vsShaderName;
	std::string m_psShaderName;

	std::string m_programName;

	Platform::FileManagerPtr m_fileManager;

	MetalVideo* m_metalVideo;
	MTKView* m_view;
	id<MTLDevice> m_device;

	id<MTLLibrary> m_library;
	id<MTLFunction> m_vertexFunction;
	id<MTLFunction> m_fragmentFunction;
	std::vector<uint8_t> m_shaderBytes;
	id<MTLRenderPipelineState> m_renderPipelineState;
	//id<MTLDepthStencilState> m_depthState;
	MTLRenderPipelineDescriptor* m_pipelineDescriptor;

	struct UniformBufferMember
	{
		MTLDataType type;
		NSUInteger offset;
	};
	
	struct TextureArgument
	{
		NSUInteger index;
	};
	
	tsl::hopscotch_map<std::string, UniformBufferMember> m_uniformMembers;
	tsl::hopscotch_map<std::string, TextureArgument> m_textureArguments;

public:
	static const uint8_t MAX_BUFFERS_IN_FLIGHT;

	MetalShader(MetalVideo* metalVideo, Platform::FileManagerPtr fileManager);
	~MetalShader();

	bool LoadShaderFromFile(
		ShaderContextPtr context,
		const std::string& vsFileName,
		const std::string& vsEntry,
		const std::string& psFileName,
		const std::string& psEntry) override;

	bool LoadShaderFromString(
		ShaderContextPtr context,
		const std::string& vsShaderName,
		const std::string& vsCodeAsciiString,
		const std::string& vsEntry,
		const std::string& psShaderName,
		const std::string& psCodeAsciiString,
		const std::string& psEntry) override;

	void SetConstant(const std::string& name, const math::Vector4 &v) override;
	void SetConstant(const std::string& name, const math::Vector3 &v) override;
	void SetConstant(const std::string& name, const math::Vector2 &v) override;
	void SetConstant(const std::string& name, const float x) override;
	void SetConstant(const std::string& name, const uint32_t n) override;
	void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector2* v) override;
	void SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector4* v) override;
	void SetMatrixConstant(const std::string& name, const math::Matrix4x4 &matrix) override;
	void SetTexture(const std::string& name, TexturePtr pTexture, const unsigned int index) override;

	void SetShader() override;
};

typedef boost::shared_ptr<MetalShader> MetalShaderPtr;

} // namespace gs2d

#endif
