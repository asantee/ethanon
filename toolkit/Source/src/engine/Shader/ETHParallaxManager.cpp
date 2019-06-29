#include "ETHParallaxManager.h"

#include <Video.h>

// TODO/TO-DO: this is an ugly workaround for performance boost. Fix that some day...
#ifdef GLES2
	#include <Video/GLES2/GLES2Sprite.h>
#endif

ETHParallaxManager::ETHParallaxManager()
 : m_normalizedOrigin(0.5f, 0.5f), m_intensity(0), m_verticalIntensity(0.1f)
{
}

ETHParallaxManager::ETHParallaxManager(const Vector2& normalizedOrigin, const float intensity)
{
	SetNormalizedOrigin(normalizedOrigin);
	SetIntensity(intensity);
}

void ETHParallaxManager::SetNormalizedOrigin(const Vector2& normalizedOrigin)
{
	m_normalizedOrigin = normalizedOrigin;
}

Vector2 ETHParallaxManager::GetNormalizedOrigin() const
{
	return m_normalizedOrigin;
}

Vector2 ETHParallaxManager::GetInScreenOrigin(const VideoConstPtr& video) const
{
	return GetNormalizedOrigin() * video->GetScreenSizeF();
}

void ETHParallaxManager::SetIntensity(const float intensity)
{
	m_intensity = intensity;
}

float ETHParallaxManager::GetIntensity() const
{
	return m_intensity;
}

void ETHParallaxManager::SetShaderParameters(const VideoConstPtr& video, const ShaderPtr& shader, const Vector3& entityPos,
											 const float& individualParallaxIntensity, const bool drawToTarget) const
{
	const float parallaxIntensity = (drawToTarget) ? 0.0f : (GetIntensity() * individualParallaxIntensity);
	const Vector2 origin(GetInScreenOrigin(video));
	const float verticalIntensity = GetVerticalIntensity();

	#ifdef GLES2
		std::vector<Vector2> params(4);
		params[0] = Vector2(entityPos.x, entityPos.y);
		params[1] = Vector2(entityPos.z, parallaxIntensity);
		params[2] = Vector2(origin);
		params[3] = Vector2(verticalIntensity, verticalIntensity);
		GLES2Sprite::AttachParametersToOptimalRenderer(params);
	#else
		shader->SetConstant(GS_L("entityPos3D"), entityPos);
		shader->SetConstant(GS_L("parallaxIntensity"), parallaxIntensity);
		shader->SetConstant(GS_L("parallaxOrigin_verticalIntensity"), Vector3(origin, verticalIntensity));
	#endif
}

void ETHParallaxManager::SetVerticalIntensity(const float intensity)
{
	m_verticalIntensity = intensity;
}

float ETHParallaxManager::GetVerticalIntensity() const
{
	return m_verticalIntensity;
}

gs2d::math::Vector2 ETHParallaxManager::ComputeOffset(
	const VideoPtr& video,
	const Vector3 &pos,
	const float& individualParallaxIntensity) const
{
	const Vector2 screenSpacePos = Vector2(pos.x, pos.y) - video->GetCameraPos();
	return ((screenSpacePos - GetInScreenOrigin(video)) / video->GetScreenSizeF().x) * pos.z * m_intensity * individualParallaxIntensity;
}
