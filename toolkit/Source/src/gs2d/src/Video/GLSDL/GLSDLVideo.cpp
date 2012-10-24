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

#include "GLSDLVideo.h"

namespace gs2d {

VideoPtr CreateVideo(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Platform::FileIOHubPtr& fileIOHub,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	return GLSDLVideo::Create(width, height, winTitle, windowed, sync, fileIOHub, pfBB, maximizable);
}

boost::shared_ptr<GLSDLVideo> GLSDLVideo::Create(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Platform::FileIOHubPtr& fileIOHub,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	boost::shared_ptr<GLSDLVideo> p(
		new GLSDLVideo(fileIOHub, width, height, winTitle, windowed, sync, maximizable));
	p->weak_this = p;
	return p;
}

GLSDLVideo::GLSDLVideo(
	Platform::FileIOHubPtr fileIOHub,
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const bool maximizable) :
	SDLWindow(fileIOHub)
{
	StartApplication(width, height, winTitle, windowed, sync, Texture::PF_UNKNOWN, maximizable);
}

bool GLSDLVideo::StartApplication(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const bool windowed,
	const bool sync,
	const Texture::PIXEL_FORMAT pfBB,
	const bool maximizable)
{
	bool rW, rGL;
	rW = SDLWindow::StartApplication(width, height, winTitle, windowed, sync, pfBB, maximizable);
	rGL =  GLVideo::StartApplication(width, height, winTitle, windowed, sync, pfBB, maximizable);
	return (rW && rGL);
}

bool GLSDLVideo::EndSpriteScene()
{
	GLVideo::EndSpriteScene();
	return SDLWindow::EndSpriteScene();
}

} // namespace gs2d
