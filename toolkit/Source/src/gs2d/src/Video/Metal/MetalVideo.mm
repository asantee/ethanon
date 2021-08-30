#include "MetalVideo.h"

#include "MetalPolygonRenderer.h"
#include "MetalShader.h"

#include "../../Platform/getRealTime.h"

namespace gs2d {

MetalVideo::MetalVideo(
	Platform::FileIOHubPtr fileIOHub,
	MTKView *view) :
	m_fileIOHub(fileIOHub),
	m_view(view),
	m_quit(false),
	m_fpsRate(60.0f),
	m_renderPassDescriptor(nil)
{
	m_startTime = getRealTime();

	m_screenSizeInPixels.x = view.bounds.size.width;
	m_screenSizeInPixels.y = view.bounds.size.height;

	m_device = MTLCreateSystemDefaultDevice();
	
	m_commandQueue = [m_device newCommandQueue];

	m_view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
	m_view.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
	m_view.sampleCount = 1;
	
	gs2d::Application::SharedData.Create("com.ethanonengine.usingSuperSimple", "true", true /*constant*/);
}

bool MetalVideo::BeginRendering(const Color& bgColor)
{
	m_rendering = true;
	m_commandBuffer = [m_commandQueue commandBuffer];
	
	m_renderPassDescriptor = m_view.currentRenderPassDescriptor;
	if (m_renderPassDescriptor != nil)
	{
		m_renderPassDescriptor.colorAttachments[0].clearColor =
			MTLClearColorMake(m_backgroundColor.GetR(), m_backgroundColor.GetG(), m_backgroundColor.GetB(), m_backgroundColor.GetA());

		m_commandEncoder = [m_commandBuffer renderCommandEncoderWithDescriptor:m_renderPassDescriptor];
		m_commandEncoder.label = @"GS2DRenderEncoder";

		[m_commandEncoder pushDebugGroup:@"DrawAll"];

	}
	return true;
}

bool MetalVideo::EndRendering()
{
	if (m_renderPassDescriptor != nil)
	{
		[m_commandEncoder popDebugGroup];
		[m_commandEncoder endEncoding];
		[m_commandBuffer presentDrawable:m_view.currentDrawable];
	}

	[m_commandBuffer commit];
	
	m_rendering = false;
	return true;
}

PolygonRendererPtr MetalVideo::CreatePolygonRenderer(
		const std::vector<PolygonRenderer::Vertex>& vertices,
		const std::vector<uint32_t>& indices,
		const PolygonRenderer::POLYGON_MODE mode)
{
	return PolygonRendererPtr(new MetalPolygonRenderer(this, vertices, indices, mode));
}

ShaderPtr MetalVideo::LoadShaderFromFile(
	const std::string& vsFileName,
	const std::string& vsEntry,
	const std::string& psFileName,
	const std::string& psEntry)
{
	ShaderPtr shader = ShaderPtr(new MetalShader(this, GetFileIOHub()->GetFileManager()));
	if (shader->LoadShaderFromFile(ShaderContextPtr(), vsFileName, vsEntry, psFileName, psEntry))
	{
		return shader;
	}
	else
	{
		return ShaderPtr();
	}
}

ShaderPtr MetalVideo::LoadShaderFromString(
	const std::string& vsShaderName,
	const std::string& vsCodeAsciiString,
	const std::string& vsEntry,
	const std::string& psShaderName,
	const std::string& psCodeAsciiString,
	const std::string& psEntry)
{
	ShaderPtr shader(new MetalShader(this, GetFileIOHub()->GetFileManager()));
	if (shader->LoadShaderFromString(
		ShaderContextPtr(),
		vsShaderName,
		vsCodeAsciiString,
		vsEntry,
		psShaderName,
		psCodeAsciiString,
		psEntry))
	{
		return shader;
	}
	else
	{
		return ShaderPtr();
	}
}

TexturePtr MetalVideo::CreateTextureFromFileInMemory(
	const void *pBuffer,
	const unsigned int bufferLength,
	const unsigned int nMipMaps)
{
	/*TexturePtr texture(GLTexture::Create(weak_this, GetFileIOHub()->GetFileManager()));
	if (texture->LoadTexture(weak_this, pBuffer, nMipMaps, bufferLength))
	{
		return texture;
	}*/
	return TexturePtr();
}

TexturePtr MetalVideo::LoadTextureFromFile(
	const std::string& fileName,
	const unsigned int nMipMaps)
{
	/*TexturePtr texture(GLTexture::Create(weak_this, GetFileIOHub()->GetFileManager()));
	if (texture->LoadTexture(weak_this, fileName, nMipMaps))
	{
		return texture;
	}*/
	return TexturePtr();
}

id<MTLDevice> MetalVideo::GetDevice()
{
	return m_device;
}

MTKView* MetalVideo::GetView()
{
	return m_view;
}

id<MTLRenderCommandEncoder> MetalVideo::GetRenderCommandEncoder()
{
	return m_commandEncoder;
}

bool MetalVideo::SetAlphaMode(const ALPHA_MODE mode)
{
	return true;
}

Video::ALPHA_MODE MetalVideo::GetAlphaMode() const
{
	return Video::AM_UNKNOWN;
}

void MetalVideo::SetZBuffer(const bool enable)
{
}

bool MetalVideo::GetZBuffer() const
{
	return false;
}

void MetalVideo::SetBackgroundColor(const Color& backgroundColor)
{
	m_backgroundColor = backgroundColor;
}

Color MetalVideo::GetBackgroundColor() const
{
	return m_backgroundColor;
}

bool MetalVideo::ResetVideoMode(
	const VIDEO_MODE& mode,
	const bool toggleFullscreen)
{
	return ResetVideoMode(mode.width, mode.height, mode.pf, toggleFullscreen);
}

bool MetalVideo::ResetVideoMode(
	const unsigned int width,
	const unsigned int height,
	const Texture::PIXEL_FORMAT pfBB,
	const bool toggleFullscreen)
{
	m_screenSizeInPixels.x = static_cast<float>(width);
	m_screenSizeInPixels.y = static_cast<float>(height);

	// call listener if there's any
	ScreenSizeChangeListenerPtr listener = m_screenSizeChangeListener.lock();
	if (listener)
	{
		listener->ScreenSizeChanged(GetScreenSizeF());
	}
	return true;
}

// Window and application implementations

Video::VIDEO_MODE MetalVideo::GetVideoMode(const unsigned int modeIdx) const
{
	return VIDEO_MODE();
}

unsigned int MetalVideo::GetVideoModeCount() const
{
	return 0;
}

Platform::FileIOHubPtr MetalVideo::GetFileIOHub()
{
	return m_fileIOHub;
}

void MetalVideo::Message(const std::string& text, const GS_MESSAGE_TYPE type) const
{
	ShowMessage(text, type);
}

Application::APP_STATUS MetalVideo::HandleEvents()
{
	APP_STATUS r = APP_OK;

	if (m_quit)
		r = APP_QUIT;

	return r;
}

void MetalVideo::Quit()
{
	m_quit = true;
}

float MetalVideo::GetElapsedTimeF(const TIME_UNITY unity) const
{
	return static_cast<float>(GetElapsedTimeD(unity));
}

unsigned long MetalVideo::GetElapsedTime(const TIME_UNITY unity) const
{
	return static_cast<unsigned long>(GetElapsedTimeD(unity));
}

double MetalVideo::GetElapsedTimeD(const TIME_UNITY unity) const
{
	double elapsedTimeS = getRealTime() - m_startTime;
	switch (unity)
	{
	case TU_HOURS:
		elapsedTimeS /= 60.0;
		elapsedTimeS /= 60.0;
		break;
	case TU_MINUTES:
		elapsedTimeS /= 60.0;
		break;
	case TU_MILLISECONDS:
		elapsedTimeS *= 1000.0;
	case TU_SECONDS:
	default:
		break;
	};
	return elapsedTimeS;
}

void MetalVideo::ComputeFPSRate()
{
	static float counter = 0.0f;
	const clock_t current = GetElapsedTime(TU_MILLISECONDS);
	static clock_t last = current;

	const clock_t elapsed = current - last;
	if (elapsed > 500)
	{
		m_fpsRate = (counter * 2);
		counter = 0.0f;
		last = current;
	}
	else
	{
		counter++;
	}
}

float MetalVideo::GetFPSRate() const
{
	return math::Max(1.0f, m_fpsRate);
}

std::string MetalVideo::GetWindowTitle() const
{
	return "";
}

bool MetalVideo::SetWindowTitle(const std::string& title)
{
	return false;
}

math::Vector2i MetalVideo::GetScreenSize() const
{
	return GetScreenSizeF().ToVector2i();
}

math::Vector2 MetalVideo::GetScreenSizeF() const
{
	const float virtualScreenHeight = GetVirtualScreenHeight();
	const float virtualWidth = m_screenSizeInPixels.x * (virtualScreenHeight / m_screenSizeInPixels.y);
	return math::Vector2(virtualWidth, virtualScreenHeight);
}

math::Vector2 MetalVideo::GetScreenSizeInPixels() const
{
	return m_screenSizeInPixels;
}

void MetalVideo::EnableQuitShortcuts(const bool enable)
{
	// not allowed on Mac for now (AppStore rejects apps that completely ignore it)
}

bool MetalVideo::QuitShortcutsEnabled()
{
	return true;
}

bool MetalVideo::IsWindowed() const
{
	return true;
}

bool MetalVideo::WindowVisible() const
{
	return true;
}

bool MetalVideo::WindowInFocus() const
{
	return true;
}

void MetalVideo::ForwardCommand(const std::string& cmd)
{
#	ifdef _DEBUG
	std::cout << cmd << std::endl;
	breakCodeIfGetsHere
#	endif
	Command(cmd);
}


std::string MetalVideo::PullCommands()
{
	std::string out;
	ForwardCommands(out);
	return out;
}

bool MetalVideo::HideCursor(const bool hide)
{
	return false;
}

bool MetalVideo::IsCursorHidden() const
{
	return false;
}

math::Vector2i MetalVideo::GetClientScreenSize() const
{
	return math::Vector2i(static_cast<int>(m_screenSizeInPixels.x), static_cast<int>(m_screenSizeInPixels.y));
}

math::Vector2i MetalVideo::GetWindowPosition()
{
	return math::Vector2i(0, 0);
}

void MetalVideo::SetWindowPosition(const math::Vector2i &v2)
{
}

math::Vector2i MetalVideo::ScreenToWindow(const math::Vector2i &v2Point) const
{
	// yet to be implemented
	return v2Point;
}

} // namespace gs2d
