#ifndef ETH_ENTITY_PIECE_RENDERER_H_
#define ETH_ENTITY_PIECE_RENDERER_H_

#include "../Entity/ETHRenderEntity.h"

#include "../Scene/ETHSceneProperties.h"

class ETHEntityPieceRenderer
{
protected:
	ETHRenderEntity* m_entity;

public:
	ETHEntityPieceRenderer(ETHRenderEntity* entity);
	~ETHEntityPieceRenderer();
	virtual void Render(const ETHSceneProperties& props, const float maxHeight, const float minHeight) = 0;
	ETHRenderEntity* GetEntity();
};

typedef boost::shared_ptr<ETHEntityPieceRenderer> ETHEntityPieceRendererPtr;

#endif
