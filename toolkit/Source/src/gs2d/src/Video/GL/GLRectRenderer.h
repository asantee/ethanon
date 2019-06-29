#ifndef GL_RECT_RENDERER_H_
#define GL_RECT_RENDERER_H_

#include "../../Sprite.h"

#include "GLInclude.h"

namespace gs2d {

class GLRectRenderer
{
	GLuint m_vboId, m_iboId;
public:
	GLRectRenderer();
	void Draw(const Sprite::RECT_MODE mode) const;
};

} // namespace gs2d

#endif
