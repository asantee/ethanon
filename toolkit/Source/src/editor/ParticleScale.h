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

#ifndef _CPARTICLE_SCALE_HEADER_32546357879455
#define _CPARTICLE_SCALE_HEADER_32546357879455

#include "IEditorPlugin.h"
#include "gsgui.h"

#define ETH_MAX_PARTICLE_SYS_PER_ENTITY 2

class cParticleScale : public IEditorPlugin
{
public:
	void LoadEditor(EditorBase *pEditor);
	float DoEditor(const Vector2 &v2Pos, ETHEntity* pEntity, EditorBase *pEditor);
	bool IsCursorAvailable() const;
	bool IsFieldActive() const;

private:
	GSGUI_FLOAT_INPUT m_scaleInput[ETH_MAX_PARTICLE_SYS_PER_ENTITY];
};

#endif