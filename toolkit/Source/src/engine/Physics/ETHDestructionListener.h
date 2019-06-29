#ifndef ETH_DESTRUCTION_LISTENER_H_
#define ETH_DESTRUCTION_LISTENER_H_

#include <Box2D/Box2D.h>
#include <boost/shared_ptr.hpp>

class ETHDestructionListener : public b2DestructionListener
{
	void SayGoodbye(b2Joint* joint);
	void SayGoodbye(b2Fixture* fixture);
};

typedef boost::shared_ptr<ETHDestructionListener> ETHDestructionListenerPtr;

#endif
