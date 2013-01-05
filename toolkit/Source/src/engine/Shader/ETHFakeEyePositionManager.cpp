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

#include "ETHFakeEyePositionManager.h"

#include <Video.h>

ETHFakeEyePositionManager::ETHFakeEyePositionManager()
	: m_fakeEyeHeight(768.0f)
{
}

void ETHFakeEyePositionManager::SetFakeEyeHeight(const float f)
{
	m_fakeEyeHeight = Max(f, 0.0f);
}

float ETHFakeEyePositionManager::GetFakeEyeHeight() const
{
	return m_fakeEyeHeight;
}

Vector3 ETHFakeEyePositionManager::ComputeFakeEyePosition(
	VideoPtr video,
	ShaderPtr pShader,
	const bool drawToTarget,
	const Vector3& v3LightPos,
	const float entityAngle)
{
	const Vector2& v2CamPos(video->GetCameraPos());
	const Vector2& v2ScreenDim(video->GetScreenSizeF());
	Vector3 v3RelativeEyePos(0, v2ScreenDim.y * 1.5f, GetFakeEyeHeight());

	if (!drawToTarget)
		v3RelativeEyePos.y -= v3LightPos.y - v2CamPos.y;

	Matrix4x4 matRot = RotateZ(-DegreeToRadian(entityAngle));
	v3RelativeEyePos = Multiply(v3RelativeEyePos, matRot);

	//pShader->SetConstant(GS_L("fakeEyePos"), v3RelativeEyePos+Vector3(0, v2CamPos.y, 0)+Vector3(v3LightPos.x,0,0));
	return v3RelativeEyePos + Vector3(0, v2CamPos.y, 0) + Vector3(v3LightPos.x,0,0);
}
