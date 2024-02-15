#import "AppleApplication.h"

#import "../../Video.h"
#import "../../Audio.h"
#import "../../Input.h"

#import "../../Audio/AudioDummy.h"

#import "../../BaseApplication.h"

#import "../../Platform/ios/IOSFileIOHub.h"

#import "../../Platform/ios/IOSNativeCommandListener.h"

#import "../../Input/iOS/IOSInput.h"

#import "../../../../engine/ETHEngine.h"

BOOL g_terminated = FALSE;

void CreateLoadingSprite(gs2d::VideoPtr video, gs2d::SpritePtr& sprite, const std::string& filePath)
{
	sprite = gs2d::SpritePtr(new gs2d::Sprite(video.get(), filePath));
	sprite->SetOrigin(Vector2(0.5f));
}

@interface AppleApplication ()
{
	gs2d::VideoPtr m_video;
	gs2d::InputPtr m_input;
	gs2d::AudioPtr m_audio;
	gs2d::BaseApplicationPtr m_application;
	gs2d::ETHEnginePtr m_engine;
	float m_pixelDensity;
	Platform::NativeCommandManager m_commandManager;

	gs2d::SpritePtr /*m_splashSprite, */m_cogSprite;
}

@end

@implementation AppleApplication

- (id)initWithVideo:(gs2d::VideoPtr)video
			  audio:(gs2d::AudioPtr)audio
			  input:(gs2d::InputPtr)input
   commandListeners:(const std::vector<Platform::NativeCommandListenerPtr>&) commandListeners;
{
	self = [super init];
	
	m_video = video;
	m_audio = audio;
	m_input = input;

	for (unsigned int t = 0; t < commandListeners.size(); t++)
	{
		m_commandManager.InsertCommandListener(commandListeners[t]);
	}

	m_commandManager.InsertCommandListener(Platform::IOSNativeCommmandListenerPtr(new Platform::IOSNativeCommmandListener));

	// setup default subplatform
	gs2d::Application::SharedData.SetSecured("com.ethanonengine.subplatform", "apple");

	NSString* appVersionNameString = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
	NSString* appBuildNumberString = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"];

	if (appVersionNameString != nil) gs2d::Application::SharedData.SetSecured("com.ethanonengine.versionName", [appVersionNameString UTF8String]);
	if (appBuildNumberString != nil) gs2d::Application::SharedData.SetSecured("com.ethanonengine.buildNumber", [appBuildNumberString UTF8String]);

	// setup language code
	NSString *language = [[NSLocale preferredLanguages] objectAtIndex:0];
	NSRange designatorRange = NSMakeRange(0, 2);
	language = [language substringWithRange:designatorRange];
	gs2d::Application::SharedData.SetSecured("ethanon.system.language", [language cStringUsingEncoding:1]);

	// Here
	m_application = gs2d::CreateBaseApplication(false /*autoStartScriptEngine*/);
	m_engine = ETHEngine::Cast(m_application);
	m_engine->Start(m_video, m_input, m_audio);

	gs2d::FileIOHubPtr fileIOHub = m_engine->ETHScriptWrapper::GetProvider()->GetFileIOHub();
	
	//CreateLoadingSprite(video, m_splashSprite, fileIOHub->GetResourceDirectory() + "data/splash.png");
	CreateLoadingSprite(video, m_cogSprite,    fileIOHub->GetResourceDirectory() + "data/cog.png");

	// Dispatch a task to a background queue
	dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
		self->m_engine->StartScriptEngine();
		dispatch_async(dispatch_get_main_queue(), ^{
			// Update UI or perform any main thread tasks
		});
	});
	
	return self;
}

- (void)update
{
	if (!m_engine || g_terminated) return;

	m_input->Update();
	m_audio->Update();
	m_video->HandleEvents();

	const float elapsedTime = (gs2d::ComputeElapsedTimeF(m_video));

	if (m_engine->IsScriptEngineLoaded())
	{
		m_engine->Update(gs2d::math::Min(400.0f, elapsedTime));
	}
	m_commandManager.RunCommands(m_video->PullCommands());
}

- (void)renderFrame
{
	if (!m_engine || g_terminated)
		return;

	if (m_engine->IsScriptEngineLoaded())
	{
		m_engine->RenderFrame();
	}
	else
	{
		m_video->SetBackgroundColor(gs2d::constant::WHITE);
		m_video->BeginRendering(gs2d::constant::WHITE);
		if (m_cogSprite)
		{
			const Vector2 screenSize(m_video->GetScreenSizeF());
			//const float scale = (screenSize.y / 720.0f) * 0.8f;
			//m_splashSprite->Draw(Vector3(screenSize * 0.5f, 0.0f), scale, 0.0f, Rect2D());

			const Vector2 cogMiddle(screenSize.x * 0.5f, screenSize.y * 0.618f);
			static float angle = 0.0f;
			m_cogSprite->Draw(Vector3(cogMiddle - Vector2(32.0f, 0.0f), 0.0f), 0.6f,-angle + 24.0f, Rect2D());
			m_cogSprite->Draw(Vector3(cogMiddle + Vector2(32.0f, 0.0f), 0.0f), 0.6f,-angle - 24.0f, Rect2D());
			m_cogSprite->Draw(Vector3(cogMiddle, 0.0f), 0.6f, angle, Rect2D());
			angle -= 2.0f;
		}
		m_video->EndRendering();
	}
}

- (void)destroy
{
	if (!m_engine->IsScriptEngineLoaded())
		return;

	m_engine->Destroy();
}

- (void)restore
{
	if (m_engine && m_engine->IsScriptEngineLoaded())
	{
		m_engine->Restore();
	}
}

- (void)detectJoysticks
{
	if (m_input)
	{
		m_input->DetectJoysticks();
	}
}

- (gs2d::Input*)input
{
	return m_input.get();
}

- (void)forceGamepadPause
{
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(m_input.get());
	if (input)
	{
		input->ForcePause();
	}
}

- (void)insertCommandListener:(const Platform::NativeCommandListenerPtr) listener
{
	m_commandManager.InsertCommandListener(listener);
}

+ (void) terminate
{
	g_terminated = TRUE;
}

@end
