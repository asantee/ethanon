/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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
	m_callback(0),
	m_constructorCallback(0),
	m_hadRunConstructor(false)
{
}

ETHRawEntityController::ETHRawEntityController(
	const ETHEntityControllerPtr& old,
	asIScriptContext *pContext,
	asIScriptFunction* callback,
	asIScriptFunction* constructorCallback) :
	m_pContext(pContext),
	m_callback(callback),
	m_constructorCallback(constructorCallback),
	m_hadRunConstructor(false)
{
	m_pos = old->GetPos();
	m_angle = old->GetAngle();
}

ETHRawEntityController::~ETHRawEntityController() {}

void ETHRawEntityController::Update(const float lastFrameElapsedTime, ETHBucketManager& buckets)
{
	GS2D_UNUSED_ARGUMENT(lastFrameElapsedTime);
	GS2D_UNUSED_ARGUMENT(buckets);
}

Vector3 ETHRawEntityController::GetPos() const
{
	return m_pos;
}

Vector2 ETHRawEntityController::GetPosXY() const
{
	return Vector2(m_pos.x, m_pos.y);
}

float ETHRawEntityController::GetPosX() const
{
	return m_pos.x;
}

float ETHRawEntityController::GetPosY() const
{
	return m_pos.y;
}

float ETHRawEntityController::GetPosZ() const
{
	return m_pos.z;
}

float ETHRawEntityController::GetAngle() const
{
	return m_angle;
}

void ETHRawEntityController::SetPos(const Vector3& pos)
{
	m_pos = pos;
}

void ETHRawEntityController::SetPosXY(const Vector2& pos)
{
	m_pos.x = pos.x;
	m_pos.y = pos.y;
}

void ETHRawEntityController::SetPosX(const float v)
{
	m_pos.x = v;
}

void ETHRawEntityController::SetPosY(const float v)
{
	m_pos.y = v;
}

void ETHRawEntityController::SetPosZ(const float v)
{
	m_pos.z = v;
}

void ETHRawEntityController::AddToPosXY(const Vector2& pos)
{
	AddToPosX(pos.x);
	AddToPosY(pos.y);
}

void ETHRawEntityController::AddToPosX(const float v)
{
	m_pos.x += v;
}

void ETHRawEntityController::AddToPosY(const float v)
{
	m_pos.y += v;
}

void ETHRawEntityController::AddToPosZ(const float v)
{
	m_pos.z += v;
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
	if (HasConstructorCallback() && !m_hadRunConstructor)
	{
		ETHGlobal::RunEntityCallback(m_pContext, entity, m_constructorCallback);
		m_hadRunConstructor = true;
	}
	if (HasCallback())
	{
		ETHGlobal::RunEntityCallback(m_pContext, entity, m_callback);
	}
	return true;
}

asIScriptContext* ETHRawEntityController::GetScriptContext()
{
	return m_pContext;
}

asIScriptFunction* ETHRawEntityController::GetCallback()
{
	return m_callback;
}

asIScriptFunction* ETHRawEntityController::GetConstructorCallback()
{
	return m_constructorCallback;
}

void ETHRawEntityController::Destroy()
{
}

void ETHRawEntityController::Scale(const Vector2& scale, ETHEntity* entity)
{
	GS2D_UNUSED_ARGUMENT(scale);
	GS2D_UNUSED_ARGUMENT(entity);
}
