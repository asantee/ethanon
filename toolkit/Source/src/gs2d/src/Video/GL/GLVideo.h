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
		const bool maximizable = false) override;

	math::Vector2 GetCurrentTargetSize() const;

public:
	GLVideo();
	
	boost::any GetVideoInfo() override;

	ShaderContextPtr GetShaderContext() override;

	boost::any GetGraphicContext() override;
		
	void SetZBuffer(const bool enable) override;
	bool GetZBuffer() const override;
		
	void SetBGColor(const Color& backgroundColor) override;
	Color GetBGColor() const override;
	
	bool BeginRendering(const Color& bgColor = math::constant::ZERO_VECTOR4) override;
	bool EndRendering() override;
	
	bool SetAlphaMode(const ALPHA_MODE mode) override;
	ALPHA_MODE GetAlphaMode() const override;
};

typedef boost::shared_ptr<GLVideo> GLVideoPtr;
typedef boost::weak_ptr<GLVideo> GLVideoWeakPtr;

}

#endif
