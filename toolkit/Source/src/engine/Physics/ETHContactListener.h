#ifndef ETH_CONTACT_LISTENER
#define ETH_CONTACT_LISTENER

#include "../Entity/ETHEntity.h"

#include <Box2D/Box2D.h>

class ETHContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* contact); 
	void EndContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	bool m_disableNextContact, m_runningPreSolveContactCallback;

	struct EndContactCallbackData
	{
		math::Vector2 point0, point1, normal;
		ETHEntity *entityA, *entityB;
	};

	typedef boost::shared_ptr<EndContactCallbackData> EndContactCallbackDataPtr;

	std::list<EndContactCallbackDataPtr> m_stackedEndContactCallbacks;

	void ClearStackedEndContactCallbacks();

public:
	ETHContactListener();
	~ETHContactListener();
	void DisableNextContact();
	bool IsRunningPreSolveContactCallback() const;
	void RunAndClearStackedEndContactCallbacks();
};

#endif
