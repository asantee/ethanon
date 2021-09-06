#include "MetalShader.h"

#include "MetalVideo.h"

#include <iostream>

namespace gs2d {

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

	m_renderPipelineState = [m_device newRenderPipelineStateWithDescriptor:m_pipelineDescriptor error:nil];

	/*MTLDepthStencilDescriptor *depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
	depthStateDesc.depthCompareFunction = MTLCompareFunctionLess;
	depthStateDesc.depthWriteEnabled = YES;
	m_depthState = [m_device newDepthStencilStateWithDescriptor:depthStateDesc];*/

	return true;
}

void MetalShader::SetShader()
{
	[m_metalVideo->GetRenderCommandEncoder() setRenderPipelineState:m_renderPipelineState];
	//[m_metalVideo->GetRenderCommandEncoder() setDepthStencilState:m_depthState];
}

void MetalShader::SetConstant(const std::string& name, const math::Vector4 &v)
{
}

void MetalShader::SetConstant(const std::string& name, const math::Vector3 &v)
{
}

void MetalShader::SetConstant(const std::string& name, const math::Vector2 &v)
{
}

void MetalShader::SetConstant(const std::string& name, const float x)
{
}

void MetalShader::SetConstant(const std::string& name, const int n)
{
}

void MetalShader::SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector2* v)
{
}

void MetalShader::SetConstantArray(const std::string& name, unsigned int nElements, const math::Vector4* v)
{
}

void MetalShader::SetMatrixConstant(const std::string& name, const math::Matrix4x4 &matrix)
{
}

void MetalShader::SetTexture(const std::string& name, TexturePtr pTexture, const unsigned int index)
{
}

} //namespace gs2d
