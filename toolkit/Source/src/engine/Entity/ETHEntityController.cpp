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

#include "ETHEntityController.h"
#include <iostream>

ETHRawEntityController::ETHRawEntityController(const Vector3& pos, const float angle) :
	m_pos(pos),
	m_angle(angle),
	m_pContext(0),
	m_callbackID(-1),
	m_constructorCallbackID(-1)
{
}

ETHRawEntityController::ETHRawEntityController(const ETHEntityControllerPtr& old, asIScriptContext *pContext,
											   const int callbackID, const int constructorCallbackID) :
	m_pContext(pContext),
	m_callbackID(callbackID),
	m_constructorCallbackID(constructorCallbackID)
{
	m_pos = old->GetPos();
	m_angle = old->GetAngle();
}

ETHRawEntityController::~ETHRawEntityController() {}

void ETHRawEntityController::Update(const unsigned long lastFrameElapsedTime, ETHBucketManager& buckets)
{
	GS2D_UNUSED_ARGUMENT(lastFrameElapsedTime);
	GS2D_UNUSED_ARGUMENT(buckets);
	// TODO/TO-DO
}

Vector3 ETHRawEntityController::GetPos() const
{
	return m_pos;
}

Vector2 ETHRawEntityController::GetPosXY() const
{
	return Vector2(m_pos.x, m_pos.y);
}

float ETHRawEntityController::GetAngle() const
{
	return m_angle;
}

void ETHRawEntityController::SetPos(const Vector3& pos)
{
	m_pos = pos;
}

void ETHRawEntityController::SetAngle(const float angle)
{
	m_angle = angle;
}

void ETHRawEntityController::AddToPos(const Vector3& pos)
{
	m_pos += pos;
}

void ETHRawEntityController::AddToAngle(const float angle)
{
	m_angle += angle;
}

bool ETHRawEntityController::RunCallback(ETHScriptEntity* entity)
{
	if (HasConstructorCallback())
	{
		RunCallback(entity, m_constructorCallbackID);
		m_constructorCallbackID =-1; // never run it again. that's the trick
	}
	if (HasCallback())
	{
		RunCallback(entity, m_callbackID);
	}
	return true;
}

bool ETHRawEntityController::RunCallback(ETHScriptEntity* entity, const int id)
{
	if (m_pContext->Prepare(id) < 0)
	{
		ETH_STREAM_DECL(ss) << GS_L("(RunCallbackScript) Couldn't prepare context for function ID ") << id
							<< GS_L(" - ") << ETHGlobal::RemoveExtension(entity->GetEntityName().c_str());
		return false;
	}

	if (m_pContext->SetArgObject(0, entity) >= 0)
	{
		ETHGlobal::ExecuteContext(m_pContext, id, false);
	}
	else
	{
		return false;
	}
	return true;
}

asIScriptContext* ETHRawEntityController::GetScriptContext()
{
	return m_pContext;
}

int ETHRawEntityController::GetCallbackID()
{
	return m_callbackID;
}

int ETHRawEntityController::GetConstructorCallbackID()
{
	return m_constructorCallbackID;
}

void ETHRawEntityController::Destroy()
{
}

void ETHRawEntityController::Scale(const Vector2& scale, ETHEntity* entity)
{
	GS2D_UNUSED_ARGUMENT(scale);
	GS2D_UNUSED_ARGUMENT(entity);
}
