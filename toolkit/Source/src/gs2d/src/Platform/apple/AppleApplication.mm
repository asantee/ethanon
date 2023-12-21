#import "AppleApplication.h"

#import "../../Video.h"
#import "../../Audio.h"
#import "../../Input.h"

#import "../../Audio/AudioDummy.h"

#import "../../BaseApplication.h"

#import "../../Platform/ios/IOSFileIOHub.h"

#import "../../Platform/ios/IOSNativeCommandListener.h"

#import "../../Input/iOS/IOSInput.h"

BOOL g_terminated = FALSE;

@interface AppleApplication ()
{
	gs2d::VideoPtr m_video;
	gs2d::InputPtr m_input;
	gs2d::AudioPtr m_audio;
	gs2d::BaseApplicationPtr m_engine;
	float m_pixelDensity;
	Platform::NativeCommandManager m_commandManager;
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
	
	m_engine = gs2d::CreateBaseApplication();
	m_engine->Start(m_video, m_input, m_audio);
	
	return self;
}

- (void)update
{
	if (!m_engine || g_terminated) return;

	m_input->Update();
	m_audio->Update();
	m_video->HandleEvents();

	const float elapsedTime = (gs2d::ComputeElapsedTimeF(m_video));
	m_engine->Update(gs2d::math::Min(400.0f, elapsedTime));
	
	m_commandManager.RunCommands(m_video->PullCommands());
}

- (void)renderFrame
{
	if (!m_engine || g_terminated)
		return;

	m_engine->RenderFrame();
}

- (void)destroy
{
	m_engine->Destroy();
}

- (void)restore
{
	if (m_engine)
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
