/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#import <Foundation/Foundation.h>

#include "IOSGLES2Video.h"

namespace gs2d {

using namespace math;

GS2D_API VideoPtr CreateVideo(const unsigned int width, const unsigned int height, const Platform::FileIOHubPtr& fileIOHub)
{
	return IOSGLES2Video::Create(width, height, GS_L("GS2D"), fileIOHub);
}

IOSGLES2Video::IOSGLES2Video(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub) :
	GLES2Video(width, height, winTitle, fileIOHub)
{
}

boost::shared_ptr<GLES2Video> IOSGLES2Video::Create(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub)
{
	boost::shared_ptr<GLES2Video> p(new IOSGLES2Video(width, height, winTitle, fileIOHub));
	p->weak_this = p;
	return p;
}

// it will be implemented here for the boost timer is presenting strange behaviour
float IOSGLES2Video::GetElapsedTimeF(const TIME_UNITY unity) const
{
	@autoreleasepool {
		const double time = [[NSDate date] timeIntervalSince1970];

		double elapsedTimeMS = time * 1000.0;
		switch (unity)
		{
			case TU_HOURS:
				elapsedTimeMS /= 1000.0;
				elapsedTimeMS /= 60.0;
				elapsedTimeMS /= 60.0;
				break;
			case TU_MINUTES:
				elapsedTimeMS /= 1000.0;
				elapsedTimeMS /= 60.0;
				break;
			case TU_SECONDS:
				elapsedTimeMS /= 1000.0;
				break;
			case TU_MILLISECONDS:
			default:
				break;
		};
		return static_cast<float>(elapsedTimeMS);
	}
}

unsigned long IOSGLES2Video::GetElapsedTime(const TIME_UNITY unity) const
{
	return static_cast<unsigned long>(GetElapsedTimeF(unity));
}

} // namespace gs2d
