#ifndef GS2D_BASE_APPLICATION_H_
#define GS2D_BASE_APPLICATION_H_

#include "Video.h"
#include "Input.h"
#include "Audio.h"
#include "Platform/FileIOHub.h"

namespace gs2d {

class BaseApplication
{
public:
	virtual void Start(VideoPtr video, InputPtr input, AudioPtr audio) = 0;
	virtual Application::APP_STATUS Update(const float lastFrameDeltaTimeMS) = 0;
	virtual void RenderFrame() = 0;
	virtual void Destroy() = 0;
//	virtual void Suspend() = 0;
	virtual void Restore() = 0;
	virtual std::string RunOnUIThread(const std::string& input) { return (input); }
};

typedef boost::shared_ptr<BaseApplication> BaseApplicationPtr;

BaseApplicationPtr CreateBaseApplication(const bool autoStartScriptEngine = true);

} // namespace gs2d

#endif
