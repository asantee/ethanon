#include "ETHEntityPieceRenderer.h"

ETHEntityPieceRenderer::ETHEntityPieceRenderer(ETHRenderEntity* entity) :
	m_entity(entity)
{
	m_entity->AddRef();
}

ETHEntityPieceRenderer::~ETHEntityPieceRenderer()
{
	m_entity->Release();
}

ETHRenderEntity* ETHEntityPieceRenderer::GetEntity()
{
	return m_entity;
}
