#include "ETHDestructionListener.h"

#include "../Entity/ETHRenderEntity.h"

#include "ETHPhysicsEntityController.h"

#define UNUSED_ARGUMENT(argument) ((void)(argument))

void ETHDestructionListener::SayGoodbye(b2Joint* joint)
{
	ETHEntity *jointOwner = static_cast<ETHEntity*>(joint->GetUserData());

	// Set joint pointer to NULL for safety
	jointOwner->AddRef();
	ETHPhysicsEntityControllerPtr ownerController = boost::dynamic_pointer_cast<ETHPhysicsEntityController>(jointOwner->GetController());
	assert(ownerController);
	ownerController->KillJoint(joint);
	jointOwner->Release();
}

void ETHDestructionListener::SayGoodbye(b2Fixture* fixture)
{
	UNUSED_ARGUMENT(fixture);
	// Let's not say goodbye to these guys
}
