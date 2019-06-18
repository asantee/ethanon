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
	boost::weak_ptr<GLTexture> weak_this;

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

	GLTexture(VideoWeakPtr video, Platform::FileManagerPtr fileManager);

public:

    static boost::shared_ptr<GLTexture> Create(VideoWeakPtr video, Platform::FileManagerPtr fileManager);

	~GLTexture();

	bool IsAllBlack() const;

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
