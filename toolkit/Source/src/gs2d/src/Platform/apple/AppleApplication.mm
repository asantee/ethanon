#import "AppleApplication.h"

#import "../NativeCommandManager.h"

#import "../../Video.h"
#import "../../Audio.h"
#import "../../Input.h"

#import "../../Audio/AudioDummy.h"

#import "../../BaseApplication.h"

#import "../../Platform/ios/IOSFileIOHub.h"

#import "../../Input/iOS/IOSInput.h"

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

- (id)initWithVideo:(gs2d::VideoPtr)video audio:(gs2d::AudioPtr)audio input:(gs2d::InputPtr)input;
{
	self = [super init];
	
	m_video = video;
	m_audio = audio;
	m_input = input;

	// setup default subplatform
	gs2d::Application::SharedData.Create("com.ethanonengine.subplatform", "apple", true  /*constant*/);

	NSString* appVersionNameString = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
	NSString* appBuildNumberString = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"];

	if (appVersionNameString != nil) gs2d::Application::SharedData.Create("com.ethanonengine.versionName", [appVersionNameString UTF8String], true /*constant*/);
	if (appBuildNumberString != nil) gs2d::Application::SharedData.Create("com.ethanonengine.buildNumber", [appBuildNumberString UTF8String], true /*constant*/);

	// setup language code
	NSString *language = [[NSLocale preferredLanguages] objectAtIndex:0];
	NSRange designatorRange = NSMakeRange(0, 2);
	language = [language substringWithRange:designatorRange];
	gs2d::Application::SharedData.Create("ethanon.system.language", [language cStringUsingEncoding:1], false /*constant*/);
	
	m_engine = gs2d::CreateBaseApplication();
	m_engine->Start(m_video, m_input, m_audio);
	
	return self;
}

- (void)update
{
	if (!m_engine) return;

	m_input->Update();
	m_audio->Update();
	m_video->HandleEvents();

	const float elapsedTime = (gs2d::ComputeElapsedTimeF(m_video));
	m_engine->Update(gs2d::math::Min(400.0f, elapsedTime));
	
	m_commandManager.RunCommands(m_video->PullCommands());
}

- (void)renderFrame
{
	if (m_engine)
	{
		m_engine->RenderFrame();
	}
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

@end
