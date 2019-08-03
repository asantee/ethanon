#include "GLRectRenderer.h"

namespace gs2d {

static const unsigned int floatsPerVertex = 5;

static const float vertices[] = {
	// two triangles
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f,

	// four triangles
	0.5f, 0.5f, 0.0f,
	0.5f, 0.5f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f,

	// three triangles
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f,

	0.5f, 0.0f, 0.0f,
	0.5f, 0.0f,

	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f
};

static const GLushort indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLRectRenderer::GLRectRenderer()
{
	glGenBuffers(1, &m_vboId);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_iboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboId);

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, sizeof(float) * floatsPerVertex, BUFFER_OFFSET(0));

	//glClientActiveTexture(GL_TEXTURE0);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glTexCoordPointer(2, GL_FLOAT, sizeof(float) * floatsPerVertex, BUFFER_OFFSET(12));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId);
}

void GLRectRenderer::Draw(const Sprite::RECT_MODE mode) const
{
	switch (mode)
	{
	case Sprite::RM_TWO_TRIANGLES:
		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, (void*)0);
		break;

	case Sprite::RM_FOUR_TRIANGLES:
		glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort) * 4));
		break;

	case Sprite::RM_THREE_TRIANGLES:
		glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort) * 10));
		break;

	default:
		break;
	};
}

} // namespace gs2d
