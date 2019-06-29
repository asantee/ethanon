#ifndef ETH_FAKE_EYE_POSITION_MANAGER_H_
#define ETH_FAKE_EYE_POSITION_MANAGER_H_

#include <Shader.h>

using namespace gs2d;
using namespace gs2d::math;

class ETHFakeEyePositionManager
{
public:
	ETHFakeEyePositionManager();
	void SetFakeEyeHeight(const float f);
	float GetFakeEyeHeight() const;

	Vector3 ComputeFakeEyePosition(
		VideoPtr video,
		ShaderPtr pShader,
		const bool drawToTarget,
		const Vector3 &v3LightPos,
		const float entityAngle);

private:
	float m_fakeEyeHeight;
};

typedef boost::shared_ptr<ETHFakeEyePositionManager> ETHFakeEyePositionManagerPtr;

#endif
