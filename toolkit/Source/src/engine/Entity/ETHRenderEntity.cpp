#include "ETHRenderEntity.h"

#include "../Shader/ETHShaderManager.h"

ETHRenderEntity::ETHRenderEntity(const str_type::string& filePath, ETHResourceProviderPtr provider, const int nId) :
	ETHSpriteEntity(filePath, provider, nId)
{
}

ETHRenderEntity::ETHRenderEntity(
	TiXmlElement *pElement,
	ETHResourceProviderPtr provider,
	ETHEntityCache& entityCache,
	const str_type::string &entityPath,
	const bool shouldGenerateNewID) :
	ETHSpriteEntity(pElement, provider, entityCache, entityPath, shouldGenerateNewID)
{
}

ETHRenderEntity::ETHRenderEntity(
	ETHResourceProviderPtr provider,
	const ETHEntityProperties& properties,
	const float angle,
	const float scale) :
	ETHSpriteEntity(provider, properties, angle, scale)
{
}

ETHRenderEntity::ETHRenderEntity(ETHResourceProviderPtr provider) :
	ETHSpriteEntity(provider)
{
}

bool ETHRenderEntity::ShouldUseFourTriangles(const float parallaxIntensity) const
{
	if (!m_pSprite)
		return false;

	if ((parallaxIntensity * GetParallaxIntensity()) == 0.0f)
		return false;

	return true;
}

bool ETHRenderEntity::DrawAmbientPass(
	const float maxHeight,
	const float minHeight,
	const ETHSceneProperties& sceneProps,
	const float parallaxIntensity)
{
	if (!m_pSprite || IsHidden())
		return false;

	SetDepth(maxHeight, minHeight);

	const VideoPtr& video = m_provider->GetVideo();

	// apply lightmap textures
	if (m_pLightmap)
	{
		m_pLightmap->SetAsTexture(1);
	}

	// sets the alpha mode according to the entity's property
	const Video::ALPHA_MODE& am = GetBlendMode();

	if (am != video->GetAlphaMode())
		video->SetAlphaMode(am);

	ValidateSpriteCut(m_pSprite);
	m_pSprite->SetRect(m_spriteFrame);
	SetOrigin();

	const bool shouldUseFourTriangles = ShouldUseFourTriangles(parallaxIntensity);
	const float angle = GetAngle();
	const Vector2 pos = ETHGlobal::ToScreenPos(GetPosition(), sceneProps.zAxisDirection);

	// Set sprite flip
	m_pSprite->FlipX(GetFlipX());
	m_pSprite->FlipY(GetFlipY());

	// compute color
	Vector4 diffuseColor = Vector4(sceneProps.ambient, 1.0f);
	diffuseColor.x = Min(1.0f, diffuseColor.x + m_properties.emissiveColor.x);
	diffuseColor.y = Min(1.0f, diffuseColor.y + m_properties.emissiveColor.y);
	diffuseColor.z = Min(1.0f, diffuseColor.z + m_properties.emissiveColor.z);
	diffuseColor = diffuseColor * m_v4Color * m_properties.diffuseColor;

	if (shouldUseFourTriangles)
		m_pSprite->SetRectMode(Sprite::RM_FOUR_TRIANGLES);

	m_pSprite->DrawOptimal(pos, diffuseColor, angle, GetSize());

	if (shouldUseFourTriangles)
		m_pSprite->SetRectMode(Sprite::RM_TWO_TRIANGLES);

	m_pSprite->FlipX(false);
	m_pSprite->FlipY(false);

	return true;
}

bool ETHRenderEntity::DrawLightPass(const Vector2 &zAxisDirection, const float parallaxIntensity, const bool drawToTarget)
{
	if (!m_pSprite || IsHidden())
		return false;

	ValidateSpriteCut(m_pSprite);
	m_pSprite->SetRect(m_spriteFrame);
	SetOrigin();

	const bool shouldUseFourTriangles = ShouldUseFourTriangles(parallaxIntensity);

	if (shouldUseFourTriangles)
		m_pSprite->SetRectMode(Sprite::RM_FOUR_TRIANGLES);

	const float angle = (!IsRotatable() || drawToTarget) ? 0.0f : GetAngle();
	m_pSprite->DrawOptimal(ETHGlobal::ToScreenPos(GetPosition(), zAxisDirection),
		GetColorARGB(), angle, m_properties.scale * m_pSprite->GetFrameSize());

	if (shouldUseFourTriangles)
		m_pSprite->SetRectMode(Sprite::RM_TWO_TRIANGLES);

	return true;
}

bool ETHRenderEntity::IsSpriteVisible(
	const ETHSceneProperties& sceneProps,
	const ETHBackBufferTargetManagerPtr& backBuffer) const
{
	if (!m_pSprite || IsHidden())
		return false;

	const float angle = GetAngle();
	const Vector2& bufferSize = backBuffer->GetBufferSize();
	if (angle == 0.0f)
	{
		const ETHEntityProperties::VIEW_RECT& rect = GetScreenRect(sceneProps);

		const Vector2& min = rect.min;
		const Vector2& max = rect.max;

		if (min.x > bufferSize.x || min.y > bufferSize.y)
		{
			return false;
		}
		else if (max.x < 0.0f || max.y < 0.0f)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		gs2d::math::OrientedBoundingBoxPtr backbufferObb(new gs2d::math::OrientedBoundingBox(bufferSize * 0.5f, bufferSize, 0.0f));

		const Vector2 size = GetSize();
		const float radianAngle = -Util::DegreeToRadian(angle);
		const OrientedBoundingBox entityObb(ComputeInScreenSpriteCenter(sceneProps), size, radianAngle);
		
		return entityObb.Overlaps(*backbufferObb);
	}
}

bool ETHRenderEntity::DrawHalo(
	const Vector2 &zAxisDirection,
	const float depth)
{
	if (!GetHalo() || !HasLightSource() || IsHidden())
		return false;

	m_pHalo->SetOrigin(Sprite::EO_CENTER);
	const ETHLight* light = m_properties.light.get();

	const Vector3 v3EntityPos = GetPosition();

	// will remain as 1.0 for a while
	m_provider->GetVideo()->SetSpriteDepth(depth);

	// if it has a particle system in the first slot, adjust the light
	// brightness according to the number of active particles
	float brightness = 1.0f;
	const ETHParticleManagerPtr paticleManager = GetParticleManager(0);
	if (paticleManager)
	{
		brightness = static_cast<float>(paticleManager->GetNumActiveParticles())/static_cast<float>(paticleManager->GetNumParticles());
	}

	Vector3 v3HaloPos = (light->pos * Vector3(GetScale(), 1.0f)) + v3EntityPos;

	const Vector4 color(Vector4(light->color, 1.0f) * light->haloBrightness * brightness);
	Vector2 v2Size(light->haloSize, light->haloSize);

	m_pHalo->DrawShaped(
		ETHGlobal::ToScreenPos(v3HaloPos, zAxisDirection) + ComputeParallaxOffset(),
		v2Size * m_properties.scale,
		color, color, color, color, 0.0f);
	return true;
}

bool ETHRenderEntity::DrawParticles(
	const std::size_t n,
	const float maxHeight,
	const float minHeight,
	const ETHSceneProperties& sceneProps)
{
	if (n >= m_particles.size() || IsHidden())
	{
		return false;
	}
	else
	{
		m_particles[n]->DrawParticleSystem(
			sceneProps.ambient,
			maxHeight, minHeight,
			ETHEntityProperties::ResolveDepthSortingMode(GetType()),
			sceneProps.zAxisDirection,
			ComputeParallaxOffset(),
			ComputeDepth(maxHeight, minHeight));
		return true;
	}
}

void ETHRenderEntity::DrawCollisionBox(SpritePtr pOutline, const Color& color, const Vector2 &zAxisDirection) const
{
	VideoPtr video = m_provider->GetVideo();
	const bool collidable = static_cast<bool>(m_properties.collision);
	const Vector3 v3Size = (collidable) ? m_properties.collision->size : Vector3(32,32,32);
	const Vector3 v3Pos = (collidable) ? (m_properties.collision->pos + GetPosition()) : GetPosition();

	const Vector2 v2Pos = ETHGlobal::ToScreenPos(v3Pos, zAxisDirection)/* + Vector2(0, v3Size.y/2)*/;
	pOutline->SetOrigin(Sprite::EO_CENTER);

	const Video::ALPHA_MODE alphaMode = video->GetAlphaMode();
	video->SetAlphaMode(Video::AM_PIXEL);

	const bool zBuffer = video->GetZBuffer();
	video->SetZBuffer(false);
	const bool zWrite = video->GetZWrite();
	video->SetZWrite(false);

	ShaderPtr currentShader = video->GetCurrentShader();
	video->SetCurrentShader(ShaderPtr());

	const float depth = video->GetSpriteDepth();

	// base
	video->SetSpriteDepth(1.0f);
	pOutline->DrawShaped(v2Pos, Vector2(v3Size.x, v3Size.y), color, color, color, color, GetAngle());

	video->SetZBuffer(zBuffer);
	video->SetZBuffer(zWrite);
	video->SetAlphaMode(alphaMode);
	video->SetCurrentShader(currentShader);
	video->SetSpriteDepth(depth);
}
