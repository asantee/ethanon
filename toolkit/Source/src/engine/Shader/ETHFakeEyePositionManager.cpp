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

	Matrix4x4 matRot = Matrix4x4::RotateZ(-DegreeToRadian(entityAngle));
	v3RelativeEyePos = Matrix4x4::Multiply(v3RelativeEyePos, matRot);

	//pShader->SetConstant(GS_L("fakeEyePos"), v3RelativeEyePos+Vector3(0, v2CamPos.y, 0)+Vector3(v3LightPos.x,0,0));
	return v3RelativeEyePos + Vector3(0, v2CamPos.y, 0) + Vector3(v3LightPos.x,0,0);
}
