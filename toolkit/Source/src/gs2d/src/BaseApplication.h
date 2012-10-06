/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

    Permission is hereby granted, free of charge, to any person obtaining a copy of this
    software and associated documentation files (the "Software"), to deal in the
    Software without restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the
    following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

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
	virtual Application::APP_STATUS Update(unsigned long lastFrameDeltaTimeMS) = 0;
	virtual void RenderFrame() = 0;
	virtual void Destroy() = 0;
	virtual void Restore() = 0;
	virtual std::string RunOnUIThread(const std::string& input) { return (input); }
};

typedef boost::shared_ptr<BaseApplication> BaseApplicationPtr;

BaseApplicationPtr CreateBaseApplication();

} // namespace gs2d

#endif
