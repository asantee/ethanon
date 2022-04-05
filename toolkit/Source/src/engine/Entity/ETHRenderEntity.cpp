#include "ETHRenderEntity.h"

#include "../Shader/ETHShaderManager.h"

ETHRenderEntity::ETHRenderEntity(const std::string& filePath, ETHResourceProviderPtr provider, const int nId) :
	ETHSpriteEntity(filePath, provider, nId)
{
}

ETHRenderEntity::ETHRenderEntity(
	TiXmlElement *pElement,
	ETHResourceProviderPtr provider,
	ETHEntityCache& entityCache,
	const std::string &entityPath,
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

bool ETHRenderEntity::DrawAmbientPass(
	const float maxHeight,
	const float minHeight,
	const ETHSceneProperties& sceneProps)
{
	if (!m_pSprite || IsHidden())
	{
		return false;
	}

	SetDepth(maxHeight, minHeight);

	const VideoPtr& video = m_provider->GetVideo();

	// sets the alpha mode according to the entity's property
	const Video::ALPHA_MODE& am = GetBlendMode();

	if (am != video->GetAlphaMode())
	{
		video->SetAlphaMode(am);
	}

	SetOrigin();

	const float angle = GetAngle();
	const Vector2 pos = ETHGlobal::ToScreenPos(GetPosition(), sceneProps.zAxisDirection);

	// compute color
	Vector4 diffuseColor = Vector4(sceneProps.ambient, 1.0f);
	diffuseColor.x = Min(1.0f, diffuseColor.x + m_properties.emissiveColor.x);
	diffuseColor.y = Min(1.0f, diffuseColor.y + m_properties.emissiveColor.y);
	diffuseColor.z = Min(1.0f, diffuseColor.z + m_properties.emissiveColor.z);
	diffuseColor = diffuseColor * m_v4Color * m_properties.diffuseColor;

	const bool shouldUseHighlightPS = ShouldUseHighlightPixelShader();
	const bool shouldUseSolidColorPS = ShouldUseSolidColorPixelShader();
	const bool shouldUsePass1PS = ShouldUsePass1AddPixelShader();

	ShaderPtr shader = Sprite::GetDefaultShader();

	ShaderParametersPtr customParams(new ShaderParameters);

	if (shouldUsePass1PS)
	{
		shader = Sprite::GetAddShader();
		(*customParams)["secondary"] = boost::shared_ptr<Shader::ShaderParameter>(new Shader::TextureShaderParameter(m_pLightmap->GetTexture(), 1));
	}
	else if (shouldUseSolidColorPS)
	{
		shader = Sprite::GetSolidColorShader();
		(*customParams)["solidColor"] = boost::shared_ptr<Shader::ShaderParameter>(new Shader::Vector4ShaderParameter(GetSolidColorARGB()));
		(*customParams)["highlight"] = boost::shared_ptr<Shader::ShaderParameter>(new Shader::Vector4ShaderParameter(GetColorARGB()));
	}
	else if (shouldUseHighlightPS)
	{
		shader = Sprite::GetHighlightShader();
		(*customParams)["highlight"] = boost::shared_ptr<Shader::ShaderParameter>(new Shader::Vector4ShaderParameter(GetColorARGB()));
	}

	Sprite::SetParallaxIntensity(GetParallaxIntensity() * sceneProps.parallaxIntensity);
	m_pSprite->Draw(
		video->GetCameraPos(),
		Vector3(pos, GetPositionZ()),
		GetSize(),
		diffuseColor,
		angle,
		m_packedFrames->GetRect(m_spriteFrame),
		GetFlipX(),
		GetFlipY(),
		shader,
		customParams);

	return true;
}

bool ETHRenderEntity::IsSpriteVisible(
	const ETHSceneProperties& sceneProps,
	const ETHBackBufferTargetManagerPtr& backBuffer) const
{
	if (!m_pSprite || IsHidden())
		return false;

	const float angle = GetAngle();
	const Vector2& bufferSize = m_provider->GetVideo()->GetScreenSizeF();
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
	const float depth,
	const ETHSceneProperties& sceneProps)
{
	if (!GetHalo() || !HasLightSource() || IsHidden())
	{
		return false;
	}

	const ETHLight* light = m_properties.light.get();

	const Vector3 v3EntityPos = GetPosition();

	// if it has a particle system in the first slot, adjust the light
	// brightness according to the number of active particles
	float brightness = 1.0f;
	const ETHParticleManagerPtr paticleManager = GetParticleManager(0);
	if (paticleManager)
	{
		brightness = static_cast<float>(paticleManager->GetNumActiveParticles()) / static_cast<float>(paticleManager->GetNumParticles());
	}

	Vector3 v3HaloPos = (light->pos * Vector3(GetScale(), 1.0f)) + v3EntityPos;

	const Vector4 color(Vector4(light->color, 1.0f) * light->haloBrightness * brightness);
	Vector2 v2Size(light->haloSize, light->haloSize);

	Sprite::SetParallaxIntensity(GetParallaxIntensity() * sceneProps.parallaxIntensity);
	m_pHalo->Draw(
		m_provider->GetVideo()->GetCameraPos(),
		Vector3(ETHGlobal::ToScreenPos(v3HaloPos, zAxisDirection), v3EntityPos.z + 2.0f),
		v2Size,
		Vector2(0.5f),
		color,
		0.0f,
		Rect2D(),
		false,
		false,
		Sprite::GetDefaultShader());

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
			GetParallaxIntensity() * sceneProps.parallaxIntensity,
			ComputeDepth(maxHeight, minHeight));
		return true;
	}
}

void ETHRenderEntity::DrawCollisionBox(const ETHSceneProperties& sceneProps, SpritePtr pOutline, const Color& color, const Vector2 &zAxisDirection) const
{
	VideoPtr video = m_provider->GetVideo();
	const bool collidable = static_cast<bool>(m_properties.collision);
	const Vector3 v3Size = (collidable) ? m_properties.collision->size : Vector3(32,32,32);
	const Vector3 v3Pos = (collidable) ? (m_properties.collision->pos + GetPosition()) : GetPosition();

	const Video::ALPHA_MODE alphaMode = video->GetAlphaMode();
	video->SetAlphaMode(Video::AM_PIXEL);

	const bool zBuffer = video->GetZBuffer();
	video->SetZBuffer(false);

	pOutline->Draw(
		m_provider->GetVideo()->GetCameraPos(),
		Vector3(v3Pos),
		Vector2(v3Size.x, v3Size.y),
		Vector2(0.5f),
		color,
		0.0f,
		Rect2D(),
		false,
		false,
		Sprite::GetDefaultShader());

	video->SetZBuffer(zBuffer);
	video->SetAlphaMode(alphaMode);
}
