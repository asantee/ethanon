#ifndef GS2D_GLTEXTURE_H_
#define GS2D_GLTEXTURE_H_

#include "../../Texture.h"
#include "../../Platform/FileManager.h"

#include "../GL/GLInclude.h"

namespace gs2d {

class GLVideo;

class GLTexture : public Texture
{
	boost::weak_ptr<GLTexture> weak_this;

	Platform::FileManagerPtr m_fileManager;

	str_type::string m_fileName;

	GLuint m_texture;

	math::Vector2 m_resolution;

	GLTexture(VideoWeakPtr video, Platform::FileManagerPtr fileManager);

public:

    static boost::shared_ptr<GLTexture> Create(VideoWeakPtr video, Platform::FileManagerPtr fileManager);

	~GLTexture();

	math::Vector2 GetBitmapSize() const override;
	GLuint GetTexture() const;

	bool LoadTexture(
		VideoWeakPtr video,
		const str_type::string& fileName,
		const unsigned int nMipMaps = 0) override;

	bool LoadTexture(
		VideoWeakPtr video,
		const void* pBuffer,
		const unsigned int nMipMaps,
		const unsigned int bufferLength) override;
	
	void Free() override;
};

typedef boost::shared_ptr<GLTexture> GLTexturePtr;
typedef boost::weak_ptr<GLTexture> GLTextureWeakPtr;

} // namespace gs2d

#endif
