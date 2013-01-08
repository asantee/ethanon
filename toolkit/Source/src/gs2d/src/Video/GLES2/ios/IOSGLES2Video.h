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

#ifndef GS2D_IOS_GLES2_H_
#define GS2D_IOS_GLES2_H_

#include "../GLES2Video.h"

namespace gs2d {

class IOSGLES2Video : public GLES2Video
{
	double m_startTime;

public:
	IOSGLES2Video(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);

	static boost::shared_ptr<GLES2Video> Create(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);

	float GetElapsedTimeF(const TIME_UNITY unity) const;
	unsigned long GetElapsedTime(const TIME_UNITY unity) const;
	void ResetTimer();
};

} // namespace gs2d

#endif
