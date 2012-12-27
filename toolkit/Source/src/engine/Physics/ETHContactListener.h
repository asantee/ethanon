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
