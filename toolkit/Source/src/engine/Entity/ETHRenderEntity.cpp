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
	const str_type::string &entityPath) :
	ETHSpriteEntity(pElement, provider, entityCache, entityPath)
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

	if (GetType() != ETHEntityProperties::ET_VERTICAL)
		return false;

	if ((parallaxIntensity * GetParallaxIntensity()) == 0.0f)
		return false;

	return true;
}

bool ETHRenderEntity::DrawAmbientPass(
	const float maxHeight,
	const float minHeight,
	const bool enableLightmaps,
	const ETHSceneProperties& sceneProps,
	const float parallaxIntensity)
{
	if (!m_pSprite || IsHidden())
		return false;

	SetDepth(maxHeight, minHeight);

	const VideoPtr& video = m_provider->GetVideo();

	// apply lightmap textures
	const bool applyLightmap = (m_pLightmap && enableLightmaps && IsStatic());
	if (applyLightmap)
	{
		video->SetBlendMode(1, Video::BM_ADD);
		m_pLightmap->SetAsTexture(1);
	}
	else
	{
		video->UnsetTexture(1);
	}

	// sets the alpha mode according to the entity's property
	const Video::ALPHA_MODE& am = GetBlendMode();

	if (am != video->GetAlphaMode())
		video->SetAlphaMode(am);

	ValidateSpriteCut(m_pSprite);
	m_pSprite->SetRect(m_spriteFrame);
	SetOrigin();

	const bool shouldUseFourTriangles = ShouldUseFourTriangles(parallaxIntensity);
	const float angle = (m_properties.type == ETHEntityProperties::ET_VERTICAL) ? 0.0f : GetAngle();
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

	m_pSprite->DrawOptimal(pos, diffuseColor, angle, GetCurrentSize());

	if (shouldUseFourTriangles)
		m_pSprite->SetRectMode(Sprite::RM_TWO_TRIANGLES);

	m_pSprite->FlipX(false);
	m_pSprite->FlipY(false);

	if (applyLightmap)
	{
		video->UnsetTexture(1);
	}
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

bool ETHRenderEntity::IsSpriteVisible(const ETHSceneProperties& sceneProps, const ETHBackBufferTargetManagerPtr& backBuffer) const
{
	if (!m_pSprite || IsHidden())
		return false;

	const float angle = GetAngle();
	if (GetType() == ETHEntityProperties::ET_VERTICAL || angle == 0.0f)
	{
		const Vector2& bufferSize = backBuffer->GetBufferSize();
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
		// TODO/TO-DO perform this in the OrientedBoundingBox class
		const Vector2& size = GetCurrentSize();
		const float radianAngle = -DegreeToRadian(angle);
		const OrientedBoundingBox entityObb(ComputeInScreenSpriteCenter(sceneProps), size, radianAngle);
		return entityObb.Overlaps(*(backBuffer->GetOBB().get()));
	}
}

bool ETHRenderEntity::DrawShadow(
	const float maxHeight,
	const float minHeight,
	const ETHSceneProperties& sceneProps,
	const ETHLight& light,
	ETHSpriteEntity *pParent,
	const bool maxOpacity,
	const bool drawToTarget,
	const float targetAngle,
	const Vector3& v3TargetPos)
{
	return DrawProjShadow(maxHeight, minHeight, sceneProps, light, pParent, maxOpacity, drawToTarget, targetAngle, v3TargetPos);
}

bool ETHRenderEntity::DrawProjShadow(
	const float maxHeight,
	const float minHeight,
	const ETHSceneProperties& sceneProps,
	const ETHLight& light,
	ETHSpriteEntity *pParent,
	const bool maxOpacity,
	const bool drawToTarget,
	const float targetAngle,
	const Vector3& v3TargetPos)
{
	if (!m_pSprite || IsHidden())
		return false;

	VideoPtr video = m_provider->GetVideo();
	ETHShaderManagerPtr shaderManager = m_provider->GetShaderManager();
	SpritePtr pShadow = shaderManager->GetProjShadow();

	Vector3 v3LightPos;
	Vector3 v3ParentPos(0,0,0);

	const Vector3 v3EntityPos = GetPosition();

	if (pParent)
	{
		v3ParentPos = pParent->GetPosition();
		v3LightPos = Vector3(v3ParentPos.x, v3ParentPos.y, 0) + light.pos;
	}
	else
	{
		v3LightPos = light.pos;
	}

	// if the object is higher than the light, then the shadow shouldn't be cast on the floor
	if (v3LightPos.z < v3EntityPos.z)
	{
		return true;
	}

	const float scale = (m_properties.shadowScale <= 0.0f) ? 1.0f : m_properties.shadowScale;
	const float opacity = (m_properties.shadowOpacity <= 0.0f) ? 1.0f : m_properties.shadowOpacity;
	const Vector2 v2Size = GetCurrentSize();
	Vector2 v2ShadowSize(v2Size.x, v2Size.y);
	Vector2 v2ShadowPos(v3EntityPos.x, v3EntityPos.y);

	// if we are drawing to a target of a rotated entity
	if (drawToTarget && targetAngle != 0)
	{
		// rotate the shadow position according to entity angle
		Matrix4x4 matRot = RotateZ(-DegreeToRadian(targetAngle));
		Vector3 newShadowPos(v2ShadowPos, 0);
		newShadowPos = newShadowPos - v3TargetPos;
		newShadowPos = Multiply(newShadowPos, matRot);
		newShadowPos = newShadowPos + v3TargetPos;
		v2ShadowPos.x = newShadowPos.x;
		v2ShadowPos.y = newShadowPos.y;

		// rotate the light source to cast it correctly
		Vector3 newPos = v3LightPos - v3TargetPos;
		newPos = Multiply(newPos, matRot);
		v3LightPos = newPos + v3TargetPos;
	}

	Vector3 diff = v3EntityPos - v3LightPos;
	const float squaredDist = DP3(diff, diff);
	float squaredRange = light.range * light.range;

	if (squaredDist > squaredRange)
	{
		return true;
	}

	v2ShadowSize.x *= _ETH_SHADOW_SCALEX * scale;

	// calculate the correct shadow length according to the light height
	if ((GetPosition().z+v2Size.y) < light.pos.z) // if the light is over the entity
	{
		const float planarDist = Distance(GetPositionXY(), ETHGlobal::ToVector2(v3LightPos));
		const float verticalDist = Abs((v3EntityPos.z+v2Size.y)-v3LightPos.z);
		const float totalDist = (planarDist/verticalDist)*Abs(v3LightPos.z);
		v2ShadowSize.y = totalDist-planarDist;

		// clamp shadow length to the object's height. This is not realistic
		// but it looks better for the real-time shadows.
		v2ShadowSize.y = Min(v2Size.y*_ETH_SHADOW_FAKE_STRETCH, v2ShadowSize.y);
	}
	else
	{
		v2ShadowSize.y *= ((drawToTarget) ? _ETH_SHADOW_SCALEY : _ETH_SHADOW_SCALEY/4);
	}

	// specify a minimum length for the shadow
	v2ShadowSize.y = Max(v2ShadowSize.y, v2Size.y);

	ShaderPtr pVS = video->GetVertexShader();
	pVS->SetConstant(GS_L("shadowLength"), v2ShadowSize.y * m_properties.shadowLengthScale);
	pVS->SetConstant(GS_L("entityZ"), Max(m_shadowZ, v3EntityPos.z));
	pVS->SetConstant(GS_L("shadowZ"), m_shadowZ);
	pVS->SetConstant(GS_L("lightPos"), v3LightPos);
	video->SetSpriteDepth(
		(GetType() == ETHEntityProperties::ET_VERTICAL) ?
		ETHEntity::ComputeDepth(m_shadowZ, maxHeight, minHeight)
		: Max(0.0f, ComputeDepth(maxHeight, minHeight) - m_layrableMinimumDepth));

	v2ShadowSize.y = 1.0f;

	Vector2 lightPos2(v3LightPos.x, v3LightPos.y);
	const float shadowAngle = ::GetAngle((lightPos2 - Vector2(v3EntityPos.x, v3EntityPos.y))) + DegreeToRadian(targetAngle);

	squaredRange = Max(squaredDist, squaredRange);
	float attenBias = 1;

	// adjust brightness according to ambient light
	if (!maxOpacity)
	{
		attenBias = (1-(squaredDist/squaredRange));
		//fade the color according to the light brightness
		const float colorLen = Max(Max(light.color.x, light.color.y), light.color.z);
		attenBias *= Min(colorLen, 1.0f);

		//fade the color according to the ambient light brightness
		const Vector3 &ambientColor = sceneProps.ambient;
		const float ambientColorLen = 1.0f - ((ambientColor.x + ambientColor.y + ambientColor.z) / 3.0f);
		attenBias = Min(attenBias * ambientColorLen, 1.0f);
		attenBias *= Max(Min((1 - (GetPosition().z / Max(GetCurrentSize().y, 1.0f))), 1.0f), 0.0f);
	}

	GS_BYTE alpha = static_cast<GS_BYTE>(attenBias*255.0f*opacity);

	if (alpha < 8)
		return true;

	Color dwShadowColor(alpha,255,255,255);

	pShadow->SetOrigin(Vector2(0.5f, 0.79f));
	pShadow->SetRectMode(Sprite::RM_THREE_TRIANGLES);
	pShadow->DrawShaped(v2ShadowPos, v2ShadowSize,
		dwShadowColor, dwShadowColor, dwShadowColor, dwShadowColor,
		RadianToDegree(shadowAngle));
	pShadow->SetRectMode(Sprite::RM_TWO_TRIANGLES);

	return true;
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

	Vector3 v3HaloPos = light->pos + v3EntityPos;

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

void ETHRenderEntity::DrawCollisionBox(SpritePtr pOutline, const Color& dwColor, const Vector2 &zAxisDirection) const
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

	ShaderPtr pVS = video->GetVertexShader();
	video->SetVertexShader(ShaderPtr());

	const Color dwH = ARGB(150,dwColor.r,dwColor.g,dwColor.b);
	const float depth = video->GetSpriteDepth();

	// base
	video->SetSpriteDepth(1.0f);
	pOutline->DrawShaped(v2Pos, Vector2(v3Size.x, v3Size.y), dwH, dwH, dwH, dwH, GetAngle());

	video->SetZBuffer(zBuffer);
	video->SetZBuffer(zWrite);
	video->SetAlphaMode(alphaMode);
	video->SetVertexShader(pVS);
	video->SetSpriteDepth(depth);
}
