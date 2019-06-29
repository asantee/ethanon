#include "ETHDestructionListener.h"
#include "../Entity/ETHRenderEntity.h"
#include "ETHPhysicsEntityController.h"

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
	GS2D_UNUSED_ARGUMENT(fixture);
	// Let's not say goodbye to these guys
}
