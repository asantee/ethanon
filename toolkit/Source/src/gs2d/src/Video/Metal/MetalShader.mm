#include "MetalShader.h"

#include "MetalVideo.h"
#include "MetalTexture.h"

#include <iostream>

namespace gs2d {

const uint8_t MetalShader::MAX_BUFFERS_IN_FLIGHT = 3;

MetalShader::MetalShader(MetalVideo* metalVideo, Platform::FileManagerPtr fileManager) :
	m_metalVideo(metalVideo),
	m_fileManager(fileManager),
	m_pipelineDescriptor(nil)
{
	m_view = metalVideo->GetView();
	m_device = m_view.device;
}

MetalShader::~MetalShader()
{
}

bool MetalShader::LoadShaderFromFile(
	ShaderContextPtr context,
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	std::string vsCode, psCode;
	if (!m_fileManager->GetAnsiFileString(vsFileName, vsCode))
	{
		std::cerr << "Couldn't open " << vsFileName << std::endl;
		return false;
	}
	if (!m_fileManager->GetAnsiFileString(psFileName, psCode))
	{
		std::cerr << "Couldn't open " << psFileName << std::endl;
		return false;
	}
	return LoadShaderFromString(context, vsFileName, vsCode, vsEntry, psFileName, psCode, psEntry);
}

bool MetalShader::LoadShaderFromString(
	ShaderContextPtr context,
	const std::string& vsShaderName,
	const std::string& vsCodeAsciiString,
	const std::string& vsEntry,
	const std::string& psShaderName,
	const std::string& psCodeAsciiString,
	const std::string& psEntry)
{
	m_vsShaderName = vsShaderName;
	m_psShaderName = psShaderName;

	m_programName = vsShaderName + "/" + psShaderName;

	NSString* vertexCode   = [NSString stringWithUTF8String:vsCodeAsciiString.c_str()];
	NSString* fragmentCode = [NSString stringWithUTF8String:psCodeAsciiString.c_str()];
	NSString* fullSourceCode = [NSString stringWithFormat:@"// vertex shader\n%@\n\n// fragment shader\n%@", vertexCode, fragmentCode];

	MTLCompileOptions* options = [[MTLCompileOptions alloc] init];

	// Runtime checks for the iOS version independently of minimum target iOS.
	if (@available(macOS 10.14, iOS 12.0, tvOS 12.0, *))
	{
		[options setLanguageVersion:MTLLanguageVersion2_1];
	}
	else if (@available(macOS 10.13, iOS 11.0, tvOS 11.0, *))
	{
		[options setLanguageVersion:MTLLanguageVersion2_0];
	}
	else if (@available(macOS 10.12, iOS 10.0, tvOS 10.0, *))
	{
		[options setLanguageVersion:MTLLanguageVersion1_2];
	}
	else if (@available(macOS 10.11, iOS 9.0, tvOS 9.0, *))
	{
		[options setLanguageVersion:MTLLanguageVersion1_1];
	}
	[options setFastMathEnabled:YES];

	NSError* error;
	m_library = [m_device newLibraryWithSource:fullSourceCode options:options error:&error];
	
	if (error != nil)
	{
		NSLog(@"%@", [error description]);
	}

	m_vertexFunction = [m_library newFunctionWithName:[NSString stringWithUTF8String:vsEntry.c_str()]];
	m_fragmentFunction = [m_library newFunctionWithName:[NSString stringWithUTF8String:psEntry.c_str()]];

	m_pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
	m_pipelineDescriptor.vertexFunction = m_vertexFunction;
	m_pipelineDescriptor.fragmentFunction = m_fragmentFunction;
	m_pipelineDescriptor.sampleCount = m_view.sampleCount;
	m_pipelineDescriptor.colorAttachments[0].pixelFormat = m_view.colorPixelFormat;
	m_pipelineDescriptor.depthAttachmentPixelFormat = m_view.depthStencilPixelFormat;
	m_pipelineDescriptor.stencilAttachmentPixelFormat = m_view.depthStencilPixelFormat;

	// create uniform buffers
	MTLRenderPipelineReflection* reflectionObj;
	MTLPipelineOption option = MTLPipelineOptionBufferTypeInfo | MTLPipelineOptionArgumentInfo;
	m_renderPipelineState = [m_device newRenderPipelineStateWithDescriptor:m_pipelineDescriptor options:option reflection:&reflectionObj error:&error];
	
	for (MTLArgument *arg in reflectionObj.vertexArguments)
	{
		NSLog(@"Found vertex arg: %@ size: %lu type: %lu\n", arg.name, arg.bufferDataSize, arg.type);

		if (arg.bufferDataType == MTLDataTypeStruct && [arg.name isEqualToString:@"uniforms"])
		{
			m_shaderBytes.resize(arg.bufferDataSize, '\0');

			for (MTLStructMember* uniform in arg.bufferStructType.members)
			{
				NSLog(@"uniform: %@ type:%lu, location: %lu", uniform.name, (unsigned long)uniform.dataType, (unsigned long)uniform.offset);
				UniformBufferMember member;
				member.type = uniform.dataType;
				member.offset = uniform.offset;
				m_uniformMembers[std::string([uniform.name UTF8String])] = member;
			}
		}
	}

	for (MTLArgument *arg in reflectionObj.fragmentArguments)
	{
		NSLog(@"Found fragment arg: %@ type: %lu index: %lu\n", arg.name, arg.type, arg.index);
		if (arg.type == MTLArgumentTypeTexture)
		{
			TextureArgument argument;
			argument.index = arg.index;
			m_textureArguments[std::string([arg.name UTF8String])] = argument;
		}
	}
	
	/*MTLDepthStencilDescriptor *depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
	depthStateDesc.depthCompareFunction = MTLCompareFunctionLess;
	depthStateDesc.depthWriteEnabled = YES;
	m_depthState = [m_device newDepthStencilStateWithDescriptor:depthStateDesc];*/

	return true;
}

void MetalShader::SetShader()
{
	[m_metalVideo->GetRenderCommandEncoder() setRenderPipelineState:m_renderPipelineState];
	[m_metalVideo->GetRenderCommandEncoder()
		setVertexBytes:&m_shaderBytes[0]
		length:m_shaderBytes.size()
		atIndex:1];

	[m_metalVideo->GetRenderCommandEncoder()
		setFragmentBytes:&m_shaderBytes[0]
		length:m_shaderBytes.size()
		atIndex:1];
	//[m_metalVideo->GetRenderCommandEncoder() setDepthStencilState:m_depthState];
}

void MetalShader::SetConstant(const std::string& name, const math::Vector4 &v)
{
	tsl::hopscotch_map<std::string, UniformBufferMember>::iterator iter = m_uniformMembers.find(name);
	if (iter != m_uniformMembers.end())
	{
		math::Vector4* p = (math::Vector4*)(&m_shaderBytes[0] + iter.value().offset);
		*p = v;
	}
}

void MetalShader::SetConstant(const std::string& name, const math::Vector3 &v)
{
	tsl::hopscotch_map<std::string, UniformBufferMember>::iterator iter = m_uniformMembers.find(name);
	if (iter != m_uniformMembers.end())
	{
		math::Vector3* p = (math::Vector3*)(&m_shaderBytes[0] + iter.value().offset);
		*p = v;
	}
}

void MetalShader::SetConstant(const std::string& name, const math::Vector2 &v)
{
	tsl::hopscotch_map<std::string, UniformBufferMember>::iterator iter = m_uniformMembers.find(name);
	if (iter != m_uniformMembers.end())
	{
		math::Vector2* p = (math::Vector2*)(&m_shaderBytes[0] + iter.value().offset);
		*p = v;
	}
}

void MetalShader::SetConstant(const std::string& name, const float x)
{
	tsl::hopscotch_map<std::string, UniformBufferMember>::iterator iter = m_uniformMembers.find(name);
	if (iter != m_uniformMembers.end())
	{
		float* p = (float*)(&m_shaderBytes[0] + iter.value().offset);
		*p = x;
	}
}

void MetalShader::SetConstant(const std::string& name, const uint32_t n)
{
	tsl::hopscotch_map<std::string, UniformBufferMember>::iterator iter = m_uniformMembers.find(name);
	if (iter != m_uniformMembers.end())
	{
		uint32_t* p = (uint32_t*)(&m_shaderBytes[0] + iter.value().offset);
		*p = n;
	}
}

void MetalShader::SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector2* v)
{
	tsl::hopscotch_map<std::string, UniformBufferMember>::iterator iter = m_uniformMembers.find(name);
	if (iter != m_uniformMembers.end())
	{
		math::Vector2* p = (math::Vector2*)(&m_shaderBytes[0] + iter.value().offset);
		memcpy(p, v, sizeof(math::Vector2) * nElements);
	}
}

void MetalShader::SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector4* v)
{
	tsl::hopscotch_map<std::string, UniformBufferMember>::iterator iter = m_uniformMembers.find(name);
	if (iter != m_uniformMembers.end())
	{
		math::Vector4* p = (math::Vector4*)(&m_shaderBytes[0] + iter.value().offset);
		memcpy(p, v, sizeof(math::Vector4) * nElements);
	}
}

void MetalShader::SetMatrixConstant(const std::string& name, const math::Matrix4x4 &matrix)
{
	tsl::hopscotch_map<std::string, UniformBufferMember>::iterator iter = m_uniformMembers.find(name);
	if (iter != m_uniformMembers.end())
	{
		math::Matrix4x4* p = (math::Matrix4x4*)(&m_shaderBytes[0] + iter.value().offset);
		*p = matrix;
	}
}

void MetalShader::SetTexture(const std::string& name, TexturePtr pTexture, const unsigned int index)
{
	if (pTexture)
	{
		tsl::hopscotch_map<std::string, TextureArgument>::iterator iter = m_textureArguments.find(name);
		if (iter != m_textureArguments.end())
		{
			MetalTexture* metalTexture = (MetalTexture*)pTexture.get();
			[m_metalVideo->GetRenderCommandEncoder() setFragmentTexture:metalTexture->GetTexture() atIndex:iter.value().index];
		}
	}
	else
	{
		[m_metalVideo->GetRenderCommandEncoder() setFragmentTexture:nil atIndex:index];
	}
}

} //namespace gs2d
