#import "GameViewController.h"

#import "../../Video/Metal/MetalVideo.h"

#import "../../Platform/StdFileManager.h"

#import "../../Video/Metal/MetalViewDelegate.h"

#import "../../Audio/AudioDummy.h"

@implementation GameViewController
{
	boost::shared_ptr<gs2d::Video> m_video;

	MTKView *_view;

	MetalViewDelegate *_renderer;
}

- (void)viewDidLoad
{
	[super viewDidLoad];

	_view = (MTKView *)self.view;

	// create video device
	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());
	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, "data/");

	m_video = gs2d::VideoPtr(new gs2d::MetalVideo(fileIOHub, _view));
	m_video->SetBackgroundColor(gs2d::Color(0xFF000000));

	gs2d::MetalVideo* metalVideo = (gs2d::MetalVideo*)m_video.get();

	_view.device = metalVideo->GetDevice();

	if(!_view.device)
	{
		NSLog(@"Metal is not supported on this device");
		self.view = [[UIView alloc] initWithFrame:self.view.frame];
		return;
	}

	// create IO devices
	gs2d::AudioPtr audio = gs2d::AudioDummyPtr(new gs2d::AudioDummy()); //gs2d::CreateAudio(0);
	gs2d::InputPtr input = gs2d::CreateInput(false);

	// in case the fmod Audio object creationg should fail for reasons yet to be investigated,
	// try using dummy audio objects to prevent app crashes
	if (!audio)
	{
		audio = gs2d::AudioDummyPtr(new gs2d::AudioDummy());
	}
	
	self.ethanonApplication = [[AppleApplication alloc] initWithVideo:m_video audio:audio input:input];

	// setup renderer
	_renderer = [[MetalViewDelegate alloc] initWithMetalVideo:metalVideo fileIOHub:fileIOHub application:self.ethanonApplication];

	[_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];

	_view.delegate = _renderer;
}

@end
