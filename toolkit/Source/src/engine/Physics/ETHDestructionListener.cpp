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
