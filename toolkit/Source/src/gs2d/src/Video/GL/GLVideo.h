#ifndef GS2D_GL_VIDEO_H_
#define GS2D_GL_VIDEO_H_

#include "../../Video.h"

#include "GLInclude.h"
#include "GLRectRenderer.h"

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

	boost::shared_ptr<GLRectRenderer> m_rectRenderer;

	void Enable2DStates();

	ShaderPtr m_defaultShader;
    ShaderPtr m_rectShader;
    ShaderPtr m_fastShader;
    ShaderPtr m_modulateShader;
    ShaderPtr m_addShader;

	ShaderPtr m_currentShader;

protected:
	bool StartApplication(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT pfBB = Texture::PF_UNKNOWN,
		const bool maximizable = false);

	void UpdateInternalShadersViewData(const math::Vector2& screenSize, const bool invertY);
	static void UpdateShaderViewData(const ShaderPtr& shader, const math::Vector2& screenSize);

	math::Vector2 GetCurrentTargetSize() const;

public:
	GLVideo();
	
	boost::any GetVideoInfo();

    ShaderPtr GetDefaultShader();
    ShaderPtr GetRectShader();
    ShaderPtr GetFastShader();
    ShaderPtr GetModulateShader();
    ShaderPtr GetAddShader();
    ShaderPtr GetCurrentShader();

	ShaderContextPtr GetShaderContext();
	bool SetCurrentShader(ShaderPtr shader);

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

	const GLRectRenderer& GetRectRenderer() const;
};

typedef boost::shared_ptr<GLVideo> GLVideoPtr;
typedef boost::weak_ptr<GLVideo> GLVideoWeakPtr;

}

#endif
