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

#include "ETHLightmapGen.h"
#include "../Scene/ETHBucketManager.h"
#include "ETHShaderManager.h"

ETHLightmapGen::ETHLightmapGen(ETHRenderEntity* entity,
			   boost::shared_ptr<ETHShaderManager> shaderManager, std::list<ETHLight>::iterator iBegin, std::list<ETHLight>::iterator iEnd,
			   ETHBucketManager& buckets, const Vector3& oldPos, const Vector3& newPos, const float minHeight, const float maxHeight,
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

	// Paint it black
	video->SetRenderTarget(entity->m_pLightmap);
	if (!video->BeginTargetScene(GS_BLACK))
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

		if (!video->BeginTargetScene(GS_BLACK))
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
			entity->DrawLightPass(sceneProps.zAxisDirection, true);
			shaderManager->EndLightPass();
		}

		// draw shadows
		if (entity->GetType() != ETH_VERTICAL)
		{
			for (ETHBucketMap::iterator bucketIter = buckets.GetFirstBucket(); bucketIter != buckets.GetLastBucket(); ++bucketIter)
			{
				for (ETHEntityList::iterator entityIter = bucketIter->second.begin();
					entityIter != bucketIter->second.end(); ++entityIter)
				{
					ETHRenderEntity* ent = (*entityIter);
					if (!ent->IsStatic())
						continue;
					Vector3 oldPos2 = ent->GetPosition();
					Vector3 newPos2 = oldPos2-(oldPos-newPos);
					ent->SetOrphanPosition(newPos2 / Vector3(ent->GetScale(), 1));

					const Vector2 tmpScale(ent->GetScale());
					ent->SetScale(Vector2(1, 1));
					if (shaderManager->BeginShadowPass(ent, &(*iter), maxHeight, minHeight))
					{
						ent->DrawShadow(maxHeight, minHeight, sceneProps, *iter, 0, true, true, entity->GetAngle(), entity->GetPosition());
						shaderManager->EndShadowPass();
					}
					ent->SetScale(tmpScale);
					ent->SetOrphanPosition(oldPos2);
				}
			}
		}
		video->EndTargetScene();

		iter->pos *= tmpScale.y;
		iter->range *= tmpScale.y;
		entity->SetScale(tmpScale);

		// draw the shadowed light by adding it to the final lightmap
		video->SetRenderTarget(entity->m_pLightmap);
		video->BeginTargetScene(GS_BLACK, false);
		const GS_ALPHA_MODE oldAM = video->GetAlphaMode();
		video->SetAlphaMode(GSAM_ADD);
		tempTarget->Draw(Vector2(0,0));
		video->SetAlphaMode(oldAM);
		video->EndTargetScene();
	}

	entity->m_pLightmap->GenerateBackup();
	video->SetRenderTarget(SpritePtr());
	video->SetCameraPos(v2CamPos);
}
