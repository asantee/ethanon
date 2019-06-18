#ifndef ETH_PARALLAX_MANAGER_H_
#define ETH_PARALLAX_MANAGER_H_

#include <Shader.h>

using namespace gs2d;
using namespace math;

class ETHParallaxManager
{
	Vector2 m_normalizedOrigin;
	float m_intensity, m_verticalIntensity;

public:
	ETHParallaxManager();
	ETHParallaxManager(const Vector2& normalizedOrigin, const float intensity);
	void SetNormalizedOrigin(const Vector2& normalizedOrigin);
	Vector2 GetNormalizedOrigin() const;
	Vector2 GetInScreenOrigin(const VideoConstPtr& video) const;

	Vector2 ComputeOffset(
		const VideoPtr& video,
		const Vector3& pos,
		const float& individualParallaxIntensity) const;

	void SetIntensity(const float intensity);
	float GetIntensity() const;
	void SetVerticalIntensity(const float intensity);
	float GetVerticalIntensity() const;

	void SetShaderParameters(
		const VideoConstPtr& video,
		const ShaderPtr& shader,
		const Vector3& entityPos,
		const float& individualParallaxIntensity,
		const bool drawToTarget) const;
};

#endif
