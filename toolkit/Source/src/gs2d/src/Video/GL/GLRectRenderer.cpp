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

#include "GLRectRenderer.h"

#include "GLInclude.h"

namespace gs2d {

void GLRectRenderer::Draw(const Sprite::RECT_MODE mode) const
{
	switch (mode)
	{
	case Sprite::RM_TWO_TRIANGLES:
		glBegin(GL_TRIANGLE_FAN);
		{
			glTexCoord2f(0, 1);
			glVertex3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(0, 0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glTexCoord2f(1, 0);
			glVertex3f(1.0f, 0.0f, 0.0f);
			glTexCoord2f(1, 1);
			glVertex3f(1.0f, 1.0f, 0.0f);
		}
		glEnd();
		break;

	case Sprite::RM_THREE_TRIANGLES:
		glBegin(GL_TRIANGLE_STRIP);
		{
			glTexCoord2f(0, 0);
			glVertex3f(0.0f, 0.0f, 0.0f);

			glTexCoord2f(0, 1);
			glVertex3f(0.0f, 1.0f, 0.0f);

			glTexCoord2f(0.5f, 0);
			glVertex3f(0.5f, 0.0f, 0.0f);

			glTexCoord2f(1, 1);
			glVertex3f(1.0f, 1.0f, 0.0f);

			glTexCoord2f(1, 0);
			glVertex3f(1.0f, 0.0f, 0.0f);
		}
		glEnd();
		break;

	case Sprite::RM_FOUR_TRIANGLES:
		glBegin(GL_TRIANGLE_FAN);
		{
			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(0.5f, 0.5f, 0.0f);

			glTexCoord2f(0, 1);
			glVertex3f(0.0f, 1.0f, 0.0f);

			glTexCoord2f(0, 0);
			glVertex3f(0.0f, 0.0f, 0.0f);

			glTexCoord2f(1, 0);
			glVertex3f(1.0f, 0.0f, 0.0f);

			glTexCoord2f(1, 1);
			glVertex3f(1.0f, 1.0f, 0.0f);

			glTexCoord2f(0, 1);
			glVertex3f(0.0f, 1.0f, 0.0f);
		}
		glEnd();
		break;
	default:
		break;
	};
}

} // namespace gs2d
