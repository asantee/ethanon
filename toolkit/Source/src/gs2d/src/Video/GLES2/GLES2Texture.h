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

#ifndef GS2D_GLES2_TEXTURE_H_
#define GS2D_GLES2_TEXTURE_H_

#include "../../Video.h"
#include "../../Platform/FileLogger.h"

#ifdef APPLE_IOS
  #include "../../Platform/ios/Platform.ios.h"
  #include <OpenGLES/ES2/gl.h>
#endif

#ifdef ANDROID
  #include "../../Platform/android/Platform.android.h"
  #include <GLES2/gl2.h>
#endif

namespace gs2d {

void CheckFrameBufferStatus(const Platform::FileLogger& logger, const GLuint fbo, const GLuint tex, const bool showSuccessMessage);

class GLES2Texture : public Texture
{
public:
	GLES2Texture(VideoWeakPtr video, const str_type::string& fileName, Platform::FileManagerPtr fileManager);
	~GLES2Texture();
	bool SetTexture(const unsigned int passIdx = 0);
	PROFILE GetProfile() const;
	TYPE GetTextureType() const;
	boost::any GetTextureObject();
	GLuint GetTextureID() const;
	GLuint GetFrameBufferID() const;

	bool CreateRenderTarget(VideoWeakPtr video, const unsigned int width, const unsigned int height, const Texture::TARGET_FORMAT fmt);

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

	math::Vector2 GetBitmapSize() const;
	const str_type::string& GetFileName() const;

	static const str_type::string TEXTURE_LOG_FILE;
	static const str_type::string ETC1_FILE_FORMAT;

	struct ETC1Header
	{
		char tag[6];        // "PKM 10"
		short format;         // Format == number of mips (== zero)
		short texWidth;       // Texture dimensions, multiple of 4 (big-endian)
		short texHeight;
		short origWidth;      // Original dimensions (big-endian)
		short origHeight;
	};

private:

	bool HasETC1CompressedVersion(str_type::string& fileName);

	bool LoadTexture(
		VideoWeakPtr video,
		const void* pBuffer,
		Color mask,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps,
		const unsigned int bufferLength,
		const bool compressed);

	bool LoadETC1Texture(
		VideoWeakPtr video,
		const void* pBuffer,
		Color mask,
		const unsigned int width,
		const unsigned int height,
		const unsigned int nMipMaps,
		const unsigned int bufferLength);

	struct TEXTURE_INFO
	{
		TEXTURE_INFO();
		GLuint m_texture, m_frameBuffer;
	} m_textureInfo;

	TYPE m_type;
	PROFILE m_profile;
	str_type::string m_fileName;
	Platform::FileManagerPtr m_fileManager;
	static Platform::FileLogger m_logger;
	static GLuint m_textureID;
};

typedef boost::shared_ptr<GLES2Texture> GLES2TexturePtr;

} // namespace gs2d

#endif
