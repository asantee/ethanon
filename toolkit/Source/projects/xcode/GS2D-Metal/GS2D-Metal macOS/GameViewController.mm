#import "GameViewController.h"

#include <Platform/FileIOHub.h>
#include <Platform/StdFileManager.h>

#include <Video/Metal/MetalVideo.h>

@interface MetalRenderer : NSObject <MTKViewDelegate>
-(nonnull instancetype)initWithMetalVideo:(gs2d::MetalVideo*)video fileIOHub:(Platform::FileIOHubPtr)fileIOHub;
@end

@implementation GameViewController
{
	boost::shared_ptr<gs2d::Video> m_video;

	MTKView *_view;

	MetalRenderer *_renderer;
}

- (void)viewDidLoad
{
	[super viewDidLoad];

	_view = (MTKView *)self.view;

	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());
	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, "fonts/");

	m_video = gs2d::VideoPtr(new gs2d::MetalVideo(fileIOHub, _view));
	m_video->SetBackgroundColor(gs2d::Color(0xFF003366));

	gs2d::MetalVideo* metalVideo = (gs2d::MetalVideo*)m_video.get();

	_view.device = metalVideo->GetDevice();

	if(!_view.device)
	{
		NSLog(@"Metal is not supported on this device");
		self.view = [[NSView alloc] initWithFrame:self.view.frame];
		return;
	}

	_renderer = [[MetalRenderer alloc] initWithMetalVideo:metalVideo fileIOHub:fileIOHub];

	[_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];

	_view.delegate = _renderer;
}

@end

@implementation MetalRenderer
{
	gs2d::MetalVideo* m_video;
	gs2d::PolygonRendererPtr m_polygonRenderer;

	gs2d::ShaderPtr m_shader;
	gs2d::ShaderPtr m_spriteShader;
	gs2d::ShaderPtr m_spriteShaderHighlight;
	gs2d::ShaderPtr m_spriteShaderAdd;
	gs2d::ShaderPtr m_spriteShaderModulate;

	gs2d::TexturePtr m_textureA;
	gs2d::TexturePtr m_textureB;
	gs2d::TexturePtr m_spaceship;
	gs2d::TexturePtr m_spaceshipAdd;

	Platform::FileIOHubPtr m_fileIOHub;

	id <MTLDevice> _device;
}

-(nonnull instancetype)initWithMetalVideo:(gs2d::MetalVideo*)video fileIOHub:(Platform::FileIOHubPtr)fileIOHub;
{
	self = [super init];
	if(self)
	{
		m_video = video;
		m_fileIOHub = fileIOHub;
		_device = video->GetView().device;
		
		using namespace gs2d;
		std::vector<PolygonRenderer::Vertex> vertices =
		{
			PolygonRenderer::Vertex(math::Vector3( 0.0f, 0.0f, 0.0f), math::Vector3(1.0f), math::Vector2(0.0f, 0.0f)),
			PolygonRenderer::Vertex(math::Vector3( 0.0f, 1.0f, 0.0f), math::Vector3(1.0f), math::Vector2(0.0f, 1.0f)),
			PolygonRenderer::Vertex(math::Vector3( 1.0f, 0.0f, 0.0f), math::Vector3(1.0f), math::Vector2(1.0f, 0.0f)),
			PolygonRenderer::Vertex(math::Vector3( 1.0f, 1.0f, 0.0f), math::Vector3(1.0f), math::Vector2(1.0f, 1.0f))
		};

		std::vector<uint32_t> indices = { 0, 1, 2, 3 };

		m_polygonRenderer = m_video->CreatePolygonRenderer(vertices, indices, PolygonRenderer::TRIANGLE_STRIP);
		m_shader = m_video->LoadShaderFromFile(
			m_fileIOHub->GetResourceDirectory() + "simpleVertex.metal",
			"vertex_main",
			m_fileIOHub->GetResourceDirectory() + "simpleFragment.metal",
			"fragment_main");

		m_spriteShader = m_video->LoadShaderFromFile(
			m_fileIOHub->GetResourceDirectory() + "default-sprite.vs",
			"vertex_main",
			m_fileIOHub->GetResourceDirectory() + "default-sprite.fs",
			"fragment_main");

		m_spriteShaderHighlight = m_video->LoadShaderFromFile(
			m_fileIOHub->GetResourceDirectory() + "default-sprite.vs",
			"vertex_main",
			m_fileIOHub->GetResourceDirectory() + "default-sprite-highlight.fs",
			"fragment_main");

		m_spriteShaderAdd = m_video->LoadShaderFromFile(
			m_fileIOHub->GetResourceDirectory() + "default-sprite.vs",
			"vertex_main",
			m_fileIOHub->GetResourceDirectory() + "default-sprite-add.fs",
			"fragment_main");

		m_spriteShaderModulate = m_video->LoadShaderFromFile(
			m_fileIOHub->GetResourceDirectory() + "default-sprite.vs",
			"vertex_main",
			m_fileIOHub->GetResourceDirectory() + "default-sprite-modulate.fs",
			"fragment_main");

		m_textureA = m_video->LoadTextureFromFile(m_fileIOHub->GetResourceDirectory() + "asantee-small.png", 0);
		m_textureB = m_video->LoadTextureFromFile(m_fileIOHub->GetResourceDirectory() + "catarina.png", 0);
		m_spaceship = m_video->LoadTextureFromFile(m_fileIOHub->GetResourceDirectory() + "spaceship.png", 0);
		m_spaceshipAdd = m_video->LoadTextureFromFile(m_fileIOHub->GetResourceDirectory() + "spaceship_add.png", 0);

		/*m_shader2 = m_video->LoadShaderFromFile(
			m_fileIOHub->GetResourceDirectory() + "simpleVertex.metal",
			"vertex_main",
			m_fileIOHub->GetResourceDirectory() + "simpleFragment.metal",
			"fragment_main");*/
	}
	return self;
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
	using namespace gs2d;
	using namespace gs2d::math;

	m_video->BeginRendering();
	
	m_video->SetAlphaMode(gs2d::Video::AM_MODULATE);
	m_polygonRenderer->BeginRendering(m_shader);
	m_shader->SetConstant("posOffset", gs2d::math::Vector2(-1.0f, 1.0f));
	m_shader->SetConstant("color", gs2d::math::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_shader->SetTexture("diffuse", m_textureB, 0);
	m_polygonRenderer->Render();
	m_polygonRenderer->EndRendering();

	m_video->SetAlphaMode(gs2d::Video::AM_NONE);
	m_polygonRenderer->BeginRendering(m_shader);
	m_shader->SetConstant("posOffset", gs2d::math::Vector2(0.0f, 1.0f));
	m_shader->SetConstant("color", gs2d::math::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_shader->SetTexture("diffuse", m_textureB, 0);
	m_polygonRenderer->Render();
	m_polygonRenderer->EndRendering();

	m_video->SetAlphaMode(gs2d::Video::AM_PIXEL);
	m_polygonRenderer->BeginRendering(m_shader);
	m_shader->SetConstant("posOffset", gs2d::math::Vector2(-1.0f, 0.0f));
	m_shader->SetConstant("color", gs2d::math::Vector4(1.0f, 1.0f, 1.0f, 0.1f));
	m_shader->SetTexture("diffuse", m_textureA, 0);
	m_polygonRenderer->Render();
	m_polygonRenderer->EndRendering();

	#define U_SIZE 6
	
	#define SIZE_ORIGIN 0
	#define SPRITEPOS_VIRTUALTARGETRESOLUTION 1
	#define COLOR 2
	#define FLIPADD_FLIPMUL 3
	#define RECTPOS_RECTSIZE 4
	#define ANGLE_PARALLAXINTENSITY_ZPOS 5

	m_video->SetAlphaMode(gs2d::Video::AM_PIXEL);
	m_polygonRenderer->BeginRendering(m_spriteShader);
	static Vector4 u[U_SIZE];
	u[COLOR] = Vector4(1.0f, 0.0f, 0.0f, 0.3f);
	u[SIZE_ORIGIN] = Vector4(m_spaceship->GetBitmapSize(), Vector2(0.5f, 0.5f));
	u[SPRITEPOS_VIRTUALTARGETRESOLUTION] = Vector4(Vector2(100.0f, 100.0f), m_video->GetScreenSizeF());
	u[FLIPADD_FLIPMUL] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	u[RECTPOS_RECTSIZE] = Vector4(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
	u[ANGLE_PARALLAXINTENSITY_ZPOS] = Vector4(Util::DegreeToRadian(0.0f), 0.0f /*parallaxIntensity*/, 0.0f /*zPos*/, 0.0f);
	m_spriteShader->SetConstantArray("u", U_SIZE, u);
	m_spriteShader->SetTexture("diffuse", m_spaceship, 0);
	m_polygonRenderer->Render();
	m_polygonRenderer->EndRendering();

	m_video->SetAlphaMode(gs2d::Video::AM_ADD);
	m_polygonRenderer->BeginRendering(m_shader);
	m_shader->SetConstant("posOffset", gs2d::math::Vector2(0.0f, 0.0f));
	m_shader->SetConstant("color", gs2d::math::Vector4(1.0f, 1.0f, 1.0f, 0.5f));
	m_shader->SetTexture("diffuse", m_textureA, 0);
	m_polygonRenderer->Render();
	m_polygonRenderer->EndRendering();
	
	m_video->SetAlphaMode(gs2d::Video::AM_PIXEL);
	m_polygonRenderer->BeginRendering(m_spriteShaderHighlight);
	u[COLOR] = Vector4(1.0f, 0.0f, 0.0f, 0.3f);
	u[SIZE_ORIGIN] = Vector4(m_spaceship->GetBitmapSize() * Vector2(2.0f, 2.0f), Vector2(0.5f, 0.0f));
	u[SPRITEPOS_VIRTUALTARGETRESOLUTION] = Vector4(Vector2(m_video->GetScreenSizeF().x - 100.0f, 100.0f), m_video->GetScreenSizeF());
	u[FLIPADD_FLIPMUL] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	u[RECTPOS_RECTSIZE] = Vector4(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
	u[ANGLE_PARALLAXINTENSITY_ZPOS] = Vector4(Util::DegreeToRadian(0.0f), 0.0f /*parallaxIntensity*/, 0.0f /*zPos*/, 0.0f);
	m_spriteShaderHighlight->SetConstantArray("u", U_SIZE, u);
	m_spriteShaderHighlight->SetConstant("highlight", Vector4(4.0f, 4.0f, 4.0f, 4.0f));
	m_spriteShaderHighlight->SetTexture("diffuse", m_spaceship, 0);
	m_polygonRenderer->Render();
	m_polygonRenderer->EndRendering();

	m_video->SetAlphaMode(gs2d::Video::AM_PIXEL);
	m_polygonRenderer->BeginRendering(m_spriteShaderAdd);
	u[COLOR] = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
	u[SIZE_ORIGIN] = Vector4(m_spaceship->GetBitmapSize() * Vector2(3.0f, 3.0f), Vector2(0.5f, 0.5f));
	u[SPRITEPOS_VIRTUALTARGETRESOLUTION] = Vector4(m_video->GetScreenSizeF() - Vector2(300.0f, 300.0f), m_video->GetScreenSizeF());
	u[FLIPADD_FLIPMUL] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	u[RECTPOS_RECTSIZE] = Vector4(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
	u[ANGLE_PARALLAXINTENSITY_ZPOS] = Vector4(Util::DegreeToRadian(0.0f), 0.0f /*parallaxIntensity*/, 0.0f /*zPos*/, 0.0f);
	m_spriteShaderAdd->SetConstantArray("u", U_SIZE, u);
	m_spriteShaderAdd->SetTexture("secondary", m_spaceshipAdd, 1);
	m_spriteShaderAdd->SetTexture("diffuse", m_spaceship, 0);
	m_polygonRenderer->Render();
	m_polygonRenderer->EndRendering();

	m_video->SetAlphaMode(gs2d::Video::AM_PIXEL);
	m_polygonRenderer->BeginRendering(m_spriteShaderModulate);
	u[COLOR] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	u[SIZE_ORIGIN] = Vector4(m_spaceship->GetBitmapSize() * Vector2(5.0f, 5.0f), Vector2(0.5f, 1.0f));
	u[SPRITEPOS_VIRTUALTARGETRESOLUTION] = Vector4(m_video->GetScreenSizeF() * Vector2(0.5f, 0.5f), m_video->GetScreenSizeF());
	u[FLIPADD_FLIPMUL] = Vector4(1.0f, 0.0f, -1.0f, 1.0f);
	u[RECTPOS_RECTSIZE] = Vector4(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
	u[ANGLE_PARALLAXINTENSITY_ZPOS] = Vector4(Util::DegreeToRadian(0.0f), 0.0f /*parallaxIntensity*/, 0.0f /*zPos*/, 0.0f);
	m_spriteShaderModulate->SetConstantArray("u", U_SIZE, u);
	m_spriteShaderModulate->SetTexture("secondary", m_textureB, 1);
	m_spriteShaderModulate->SetTexture("diffuse", m_spaceship, 0);
	m_polygonRenderer->Render();
	m_polygonRenderer->EndRendering();

	m_video->EndRendering();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
	m_video->ResetVideoMode((unsigned int)size.width, (unsigned int)size.height, gs2d::Texture::PF_DEFAULT);
}

@end
