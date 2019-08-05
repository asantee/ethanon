#ifndef GS2D_GL_VIDEO_H_
#define GS2D_GL_VIDEO_H_

#include "../../Video.h"

#include "../../Math/Rect2Di.h"

#include "GLInclude.h"

#include "GLShader.h"

#include "../../Utilities/RecoverableResourceManager.h"

#include <list>

namespace gs2d {

class GLVideo : public virtual Video, public RecoverableResourceManager
{
	ALPHA_MODE m_alphaMode;
	GLenum m_textureExtension;
	const double m_alphaRef;
	const float m_zNear, m_zFar;
	Color m_backgroundColor;

	void Enable2DStates();

protected:
	bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
		const bool maximizable = false);

	math::Vector2 GetCurrentTargetSize() const;

public:
	GLVideo();
	
	boost::any GetVideoInfo();

	ShaderContextPtr GetShaderContext();

	boost::any GetGraphicContext();
		
	void SetZBuffer(const bool enable);
	bool GetZBuffer() const;
		
	void SetBGColor(const Color& backgroundColor);
	Color GetBGColor() const;
	
	bool BeginRendering(const Color& bgColor = math::constant::ZERO_VECTOR4);
	bool EndRendering();
	
	bool SetAlphaMode(const ALPHA_MODE mode);
	ALPHA_MODE GetAlphaMode() const;
	
	bool SaveScreenshot(
		const str_type::char_t* wcsName,
		const Texture::BITMAP_FORMAT fmt = Texture::BF_BMP,
		math::Rect2Di rect = math::Rect2Di(0,0,0,0));
};

typedef boost::shared_ptr<GLVideo> GLVideoPtr;
typedef boost::weak_ptr<GLVideo> GLVideoWeakPtr;

}

#endif
