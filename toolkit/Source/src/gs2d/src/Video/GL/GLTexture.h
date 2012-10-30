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

#ifndef GS2D_GLTEXTURE_H_
#define GS2D_GLTEXTURE_H_

#include "../../Texture.h"
#include "../../Platform/FileManager.h"
#include "../../Utilities/RecoverableResource.h"

#include "../GL/GLInclude.h"

#include <boost/shared_array.hpp>

namespace gs2d {

class GLVideo;

class GLTexture : public Texture, public RecoverableResource
{
	Platform::FileManagerPtr m_fileManager;
	boost::weak_ptr<GLVideo> m_video;

	TYPE m_type;
	PROFILE m_profile;
	str_type::string m_fileName;
	int m_channels;
	static GLuint m_textureID;

	GS_BYTE* m_bitmap;

	struct TEXTURE_INFO
	{
		TEXTURE_INFO();
		GLuint m_texture, m_frameBuffer, m_renderBuffer;
		GLenum glTargetFmt;
		GLenum glPixelType;
		TARGET_FORMAT gsTargetFmt;
		boost::shared_array<GS_BYTE> renderTargetBackup;
	} m_textureInfo;

	void FreeBitmap();
	void CreateTextureFromBitmap(GS_BYTE* data, const int width, const int height, const int channels, const bool pow2);
	void DeleteGLTexture();

public:
	GLTexture(VideoWeakPtr video, Platform::FileManagerPtr fileManager);
	~GLTexture();

	bool SetTexture(const unsigned int passIdx = 0);
	PROFILE GetProfile() const;
	TYPE GetTextureType() const;
	boost::any GetTextureObject();
	math::Vector2 GetBitmapSize() const;

	bool CreateRenderTarget(
		VideoWeakPtr video,
		const unsigned int width,
		const unsigned int height,const TARGET_FORMAT fmt);

	bool LoadTexture(
		VideoWeakPtr video,
		const str_type::string& fileName,
		Color mask,
		const unsigned int width = 0,
		const unsigned int height = 0,
		const unsigned int nMipMaps = 0);

	bool LoadTexture(
		VideoWeakPtr video,
		const void* pBuffer,
		Color mask,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps,
		const unsigned int bufferLength);

	const TEXTURE_INFO& GetTextureInfo() const;

	bool SaveBitmap(const str_type::char_t* name, const Texture::BITMAP_FORMAT fmt);
	bool SaveTargetSurfaceBackup();

	void Recover();
};

void CheckFrameBufferStatus(const GLuint fbo, const GLuint tex, const bool showSuccessMessage);
int GetSOILTexType(const Texture::BITMAP_FORMAT fmt, str_type::string& ext);

typedef boost::shared_ptr<GLTexture> GLTexturePtr;
typedef boost::weak_ptr<GLTexture> GLTextureWeakPtr;

} // namespace gs2d

#endif
