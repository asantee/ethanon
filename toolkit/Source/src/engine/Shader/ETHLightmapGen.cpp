#include "ETHLightmapGen.h"
#include "../Scene/ETHBucketManager.h"
#include "ETHShaderManager.h"

ETHLightmapGen::ETHLightmapGen(
	ETHRenderEntity* entity,
	boost::shared_ptr<ETHShaderManager> shaderManager,
	std::list<ETHLight>::iterator iBegin,
	std::list<ETHLight>::iterator iEnd,
	ETHBucketManager& buckets,
	const Vector3& oldPos,
	const Vector3& newPos,
	const float minHeight,
	const float maxHeight,
	const ETHSceneProperties &sceneProps)
{
	entity->m_pLightmap.reset();
	const SpritePtr& sprite = entity->m_pSprite;
	const ETHEntityProperties& props = entity->m_properties;
	if (!sprite || !props.staticEntity || !props.applyLight)
	{
		return;
	}

	const VideoPtr& video = entity->m_provider->GetVideo();
	const Platform::FileLogger* logger = entity->m_provider->GetLogger();

	if (video->Rendering())
	{
		ETH_STREAM_DECL(ss) << GS_L("Entity ID #") << entity->GetID() << GS_L(": lightmaps can't be generated during application render.");
		logger->Log(ss.str(), Platform::FileLogger::ERROR);
		return;
	}

	const Vector2 v2Size = (sprite->GetNumRects() <= 1) ? sprite->GetBitmapSizeF() : sprite->GetRect().size;
	if (!(entity->m_pLightmap = video->CreateRenderTarget(static_cast<unsigned int>(v2Size.x), static_cast<unsigned int>(v2Size.y))))
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHRenderEntity::GenerateLightmap: coudn't create the render target.");
		logger->Log(ss.str(), Platform::FileLogger::ERROR);
		entity->m_pLightmap.reset();
		return;
	}

	const bool zBuffer = video->GetZBuffer();
	const bool zWrite = video->GetZWrite();

	video->SetZBuffer(false); video->SetZWrite(false);

	// Paint it black
	video->SetRenderTarget(entity->m_pLightmap);
	if (!video->BeginTargetScene(gs2d::constant::BLACK, true))
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHRenderEntity::GenerateLightmap: coudn't render to target.");
		logger->Log(ss.str(), Platform::FileLogger::ERROR);
		entity->m_pLightmap.reset();
		return;
	}
	video->EndTargetScene();

	entity->m_controller->SetPos(newPos / Vector3(entity->GetScale(), 1));
	Vector2 v2CamPos = video->GetCameraPos();
	video->SetCameraPos(Vector2(0,0));

	for (std::list<ETHLight>::iterator iter = iBegin; iter != iEnd; ++iter)
	{
		if (!iter->staticLight)
			continue;

		SpritePtr tempTarget;
		if (!(tempTarget = video->CreateRenderTarget(static_cast<unsigned int>(v2Size.x), static_cast<unsigned int>(v2Size.y))))
		{
			ETH_STREAM_DECL(ss) << GS_L("ETHRenderEntity::GenerateLightmap: coudn't create temporary render target.");
			logger->Log(ss.str(), Platform::FileLogger::ERROR);
			entity->m_pLightmap.reset();
			return;
		}

		if (!video->SetRenderTarget(tempTarget))
		{
			ETH_STREAM_DECL(ss) << GS_L("ETHRenderEntity::GenerateLightmap: coudn't set render target.");
			logger->Log(ss.str(), Platform::FileLogger::ERROR);
			entity->m_pLightmap.reset();
			return;
		}

		if (!video->BeginTargetScene(gs2d::constant::BLACK, true))
		{
			ETH_STREAM_DECL(ss) << GS_L("ETHRenderEntity::GenerateLightmap: coudn't render to temporary target.");
			logger->Log(ss.str(), Platform::FileLogger::ERROR);
			entity->m_pLightmap.reset();
			return;
		}

		// draw light
		const Vector2 tmpScale(entity->GetScale());
		entity->SetScale(Vector2(1, 1));
		iter->pos /= tmpScale.y;
		iter->range /= tmpScale.y;
		if (shaderManager->BeginLightPass(entity, &(*iter), video->GetScreenSizeF().y, 0.0f, sceneProps.lightIntensity, 0, true))
		{
			entity->DrawLightPass(sceneProps.zAxisDirection, 0.0f, true);
			shaderManager->EndLightPass();
		}

		video->EndTargetScene();

		iter->pos *= tmpScale.y;
		iter->range *= tmpScale.y;
		entity->SetScale(tmpScale);

		// draw the shadowed light by adding it to the final lightmap
		video->SetRenderTarget(entity->m_pLightmap);
		video->BeginTargetScene(gs2d::constant::BLACK, false);
		const Video::ALPHA_MODE oldAM = video->GetAlphaMode();
		video->SetAlphaMode(Video::AM_ADD);
		tempTarget->Draw(Vector2(0,0));
		video->SetAlphaMode(oldAM);
		video->EndTargetScene();
	}

	video->SetZBuffer(zBuffer); video->SetZWrite(zWrite);

	entity->m_pLightmap->GenerateBackup();
	video->SetRenderTarget(SpritePtr());
	video->SetCameraPos(v2CamPos);
}
