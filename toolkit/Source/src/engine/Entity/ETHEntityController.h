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

#ifndef ETH_ENTITY_CONTROLLER_H_
#define ETH_ENTITY_CONTROLLER_H_

#include "../Util/ETHASUtil.h"

#include "ETHScriptEntity.h"

using namespace gs2d;
using namespace gs2d::math;

class ETHBucketManager;

class ETHEntityController
{
public:
	virtual void Update(const unsigned long lastFrameElapsedTime, ETHBucketManager& buckets) = 0;
	virtual Vector3 GetPos() const = 0;
	virtual Vector2 GetPosXY() const = 0;
	virtual float GetPosX() const = 0;
	virtual float GetPosY() const = 0;
	virtual float GetPosZ() const = 0;
	virtual float GetAngle() const = 0;
	virtual void SetPos(const Vector3& pos) = 0;
	virtual void SetPosXY(const Vector2& pos) = 0;
	virtual void SetPosX(const float v) = 0;
	virtual void SetPosY(const float v) = 0;
	virtual void SetPosZ(const float v) = 0;

	virtual void AddToPos(const Vector3& pos) = 0;
	virtual void AddToPosXY(const Vector2& pos) = 0;
	virtual void AddToPosX(const float v) = 0;
	virtual void AddToPosY(const float v) = 0;
	virtual void AddToPosZ(const float v) = 0;
	virtual void AddToAngle(const float angle) = 0;

	virtual void SetAngle(const float angle) = 0;
	virtual bool HasAnyCallbackFunction() const = 0;
	virtual bool RunCallback(ETHScriptEntity* entity) = 0;

	virtual void Destroy() = 0;
	virtual void Scale(const Vector2& scale, ETHEntity* entity) = 0;
};

typedef boost::shared_ptr<ETHEntityController> ETHEntityControllerPtr;

class ETHRawEntityController : public ETHEntityController
{
protected:
	asIScriptFunction* m_callback;
	asIScriptFunction* m_constructorCallback;
	asIScriptContext *m_pContext;
	Vector3 m_pos;
	float m_angle;
	bool m_hadRunConstructor;

public:
	ETHRawEntityController(const Vector3& pos, const float angle);
	ETHRawEntityController(const ETHEntityControllerPtr& old, asIScriptContext *pContext,
						   asIScriptFunction* callback, asIScriptFunction* constructorCallback);
	virtual ~ETHRawEntityController();
	void Update(const unsigned long lastFrameElapsedTime, ETHBucketManager& buckets);
	Vector3 GetPos() const;
	Vector2 GetPosXY() const;
	float GetAngle() const;
	void SetPos(const Vector3& pos);
	void SetAngle(const float angle);

	bool HasConstructorCallback() const { return (m_constructorCallback != 0); }
	bool HasCallback() const { return (m_callback != 0); }
	bool HasAnyCallbackFunction() const { return (HasCallback() || HasConstructorCallback()); }
	bool RunCallback(ETHScriptEntity* entity);

	void AddToPos(const Vector3& pos);
	void AddToAngle(const float angle);
	asIScriptContext* GetScriptContext();
	asIScriptFunction* GetCallback();
	asIScriptFunction* GetConstructorCallback();
	void Destroy();
	void Scale(const Vector2& scale, ETHEntity* entity);

	float GetPosX() const;
	float GetPosY() const;
	float GetPosZ() const;
	void SetPosXY(const Vector2& pos);
	void SetPosX(const float v);
	void SetPosY(const float v);
	void SetPosZ(const float v);
	void AddToPosXY(const Vector2& pos);
	void AddToPosX(const float v);
	void AddToPosY(const float v);
	void AddToPosZ(const float v);
};

typedef boost::shared_ptr<ETHRawEntityController> ETHRawEntityControllerPtr;

#endif
