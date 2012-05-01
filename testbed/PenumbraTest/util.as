/*
PORTUGUÊS
---------

Este arquivo é parte do Penumbra.

Penumbra é software livre; você pode redistribuí-lo e/ou
modificá-lo sob os termos da Licença Pública Menos Geral (LGPL)
GNU, conforme publicada pela Free Software Foundation; tanto a
versão 3 da Licença como (a seu critério) qualquer versão mais
nova.

Penumbra é distribuído na expectativa de ser útil, mas
SEM QUALQUER GARANTIA; sem mesmo a garantia implícita de
COMERCIALIZAÇÃO ou de ADEQUAÇÃO A QUALQUER PROPÓSITO EM
PARTICULAR. Consulte a Licença Pública Geral Menor (LGPL) GNU
para obter mais detalhes.

Você deve ter recebido uma cópia da Licença Pública Menos Geral
GNU junto com este programa; se não, veja em
<http://www.gnu.org/licenses/>.

ENGLISH
-------

This file is part of Penumbra.

Penumbra is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Penumbra is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Ethanon Engine. If not, see
<http://www.gnu.org/licenses/>.
*/

ETHEntity @findAmongNeighbourEntities(ETHEntity @thisEntity, const string entityName, const bool horizontalOnly)
{
	ETHEntityArray entityArray;
	const vector2 bucket = thisEntity.GetCurrentBucket();
	GetEntitiesFromBucket(bucket, entityArray);
	GetEntitiesFromBucket(bucket+vector2(1,0), entityArray);
	GetEntitiesFromBucket(bucket+vector2(-1,0), entityArray);
	GetEntitiesFromBucket(bucket+vector2(2,0), entityArray);
	GetEntitiesFromBucket(bucket+vector2(-2,0), entityArray);
	if (!horizontalOnly)
	{
		GetEntitiesFromBucket(bucket+vector2(1,1), entityArray);
		GetEntitiesFromBucket(bucket+vector2(-1,1), entityArray);		
		GetEntitiesFromBucket(bucket+vector2(1,-1), entityArray);
		GetEntitiesFromBucket(bucket+vector2(-1,-1), entityArray);		
	}
	uint size = entityArray.size();
	for (uint t=0; t<size; t++)
	{
		if (entityArray[t].GetEntityName() == entityName)
		{
			return @entityArray[t];
		}
	}
	return null;
}

ETHEntity @findMainCharNeighbours(ETHEntity @thisEntity, const bool horizontalOnly)
{
	ETHEntity @handle;
	ETHEntity @handle0 = findAmongNeighbourEntities(thisEntity, MAIN_CHARACTER_ENTITY0, horizontalOnly);
	
	if (!hasASecondController())
		return handle0;
	
	ETHEntity @handle1 = findAmongNeighbourEntities(thisEntity, MAIN_CHARACTER_ENTITY1, horizontalOnly);
	
	if (handle0 !is null && handle1 !is null)
	{
		const float dist0 = getSquaredDist(thisEntity.GetPositionXY(), handle0.GetPositionXY());
		const float dist1 = getSquaredDist(thisEntity.GetPositionXY(), handle1.GetPositionXY());
		if (dist0<dist1)
			return handle0;
		else
			return handle1;
	}
	else if (handle0 !is null)
	{
		return handle0;
	}
	return handle1;
}

bool isInScreen(ETHEntity @thisEntity)
{
	const vector2 pos = thisEntity.GetPositionXY();
	const vector2 screenMin = GetCameraPos();
	const vector2 screenMax = GetCameraPos()+GetScreenSize();
	if (pos.x-SIZE_TOLERANCE > screenMax.x)
		return false;
	if (pos.x+SIZE_TOLERANCE < screenMin.x)
		return false;
	if (pos.y-SIZE_TOLERANCE > screenMax.y)
		return false;
	if (pos.y+SIZE_TOLERANCE < screenMin.y)
		return false;
	return true;
}

bool isInScreen(const vector2 pos)
{
	const vector2 screenMin = GetCameraPos();
	const vector2 screenMax = GetCameraPos()+GetScreenSize();
	if (pos.x-SIZE_TOLERANCE > screenMax.x)
		return false;
	if (pos.x+SIZE_TOLERANCE < screenMin.x)
		return false;
	if (pos.y-SIZE_TOLERANCE > screenMax.y)
		return false;
	if (pos.y+SIZE_TOLERANCE < screenMin.y)
		return false;
	return true;
}

collisionBox getAbsoluteCollisionBox(ETHEntity @thisEntity)
{
	collisionBox r = thisEntity.GetCollisionBox();
	r.pos += thisEntity.GetPosition();
	return r;
}

float getLength(const vector2 v)
{
	return sqrt(v.x*v.x + v.y*v.y);
}

float getDist(const vector2 v0, const vector2 v1)
{
	return getLength(v1-v0);
}

float getSquaredDist(const vector2 v0, const vector2 v1)
{
	const vector2 v(v1-v0);
	return v.x*v.x + v.y*v.y;
}

bool isOnSight(ETHEntity @thisEntity, ETHEntity @other, const float mult)
{
	vector2 pos = thisEntity.GetPositionXY();
	vector2 otherPos = other.GetPositionXY();
	if (otherPos.y > pos.y+SIZE_TOLERANCE*mult)
		return false;
	if (otherPos.y < pos.y-SIZE_TOLERANCE*mult)
		return false;
	return true;
}

void knockBack(ETHEntity @thisEntity, const vector2 v)
{
	const float x = thisEntity.GetFloatData("knockBackX");
	const float y = thisEntity.GetFloatData("knockBackY");
	thisEntity.AddFloatData("knockBackX", x+v.x);
	thisEntity.AddFloatData("knockBackY", y+min(0.0f, v.y));
}

vector2 getKnockBackVector(ETHEntity @thisEntity)
{
	return vector2(thisEntity.GetFloatData("knockBackX"),thisEntity.GetFloatData("knockBackY"));
}

void setKnockBackVector(ETHEntity @thisEntity, const vector2 v)
{
	thisEntity.AddFloatData("knockBackX", v.x);
	thisEntity.AddFloatData("knockBackY", v.y);
}

void applyForce(ETHEntity @thisEntity, const vector2 v)
{
	const float x = thisEntity.GetFloatData("forceX");
	const float y = thisEntity.GetFloatData("forceY");
	thisEntity.AddFloatData("forceX", x+v.x);
	thisEntity.AddFloatData("forceY", y+v.y);
}

void move(ETHEntity @thisEntity)
{
	// if it's down and touching the ground, remove force Y to avoid unnecessary movement
	if (thisEntity.GetUIntData("touchingGround") == 1 && thisEntity.GetFloatData("forceY") > 0)
		thisEntity.AddFloatData("forceY", 0);
	
	vector2 v(thisEntity.GetFloatData("forceX"), thisEntity.GetFloatData("forceY"));
	const float fps = GetFPSRate() == 0 ? 60 : GetFPSRate();
	v = (v/fps);
	
	// do not let it be greater than the entity's size to avoid passing through objects
	const float size = min(thisEntity.GetSize().x, thisEntity.GetSize().y)*0.4f;
	if (getLength(v) >= size)
	{
		v = normalize(v)*size*0.9f;
	}

	vector2 knockVector(thisEntity.GetFloatData("knockBackX"), thisEntity.GetFloatData("knockBackY"));
	if (getLength(knockVector) >= size)
	{
		knockVector = normalize(knockVector)*size*0.9f;
	}

	knockVector *= 0.8f;
	if (abs(knockVector.x) < 0.1f)
		knockVector.x = 0.0f;
	if (abs(knockVector.y) < 0.1f)
		knockVector.y = 0.0f;
	setKnockBackVector(thisEntity, knockVector);

	thisEntity.AddToPositionXY(v+knockVector);
}

vector2 getCurrentForce(ETHEntity @thisEntity)
{
	const float x = thisEntity.GetFloatData("forceX");
	const float y = thisEntity.GetFloatData("forceY");
	return vector2(x,y);
}

void setForceX(ETHEntity @thisEntity, const float f)
{
	thisEntity.AddFloatData("forceX", f);
}

void setForceY(ETHEntity @thisEntity, const float f)
{
	thisEntity.AddFloatData("forceY", f);
}

const uint RIGHT = 0;
const uint LEFT = 1;
const uint DOWN = 2;
const uint UP = 3;
uint findDirection(const vector2 v)
{	
	const float angle = GetAngle(v);
	const float angleInDregree = radianToDegree(angle);

	uint dir;
	if (angleInDregree >= 45 && angleInDregree < 135)
	{
		dir = RIGHT;
	}
	else if (angleInDregree >= 135 && angleInDregree < 225)
	{
		dir = UP;
	}
	else if (angleInDregree >= 225 && angleInDregree < 315)
	{
		dir = LEFT;
	}
	else
	{
		dir = DOWN;
	}
	return dir;
}

uint findBoxDirection(const collisionBox a, const collisionBox b)
{
	vector2 halfSizeA = vector2(a.size.x/2, a.size.y/2);
	vector2 halfSizeB = vector2(b.size.x/2, b.size.y/2);
	vector2 posA = vector2(a.pos.x, a.pos.y);
	vector2 posB = vector2(b.pos.x, b.pos.y);
	
	const bool verticalyAligned =
		(posA.x > posB.x-halfSizeB.x)
	&&  (posA.x < posB.x+halfSizeB.x);
	const bool horizontalyAligned =
		(posA.y > posB.y-halfSizeB.y)
	&&  (posA.y < posB.y+halfSizeB.y);
	
	if (horizontalyAligned)
	{
		if (posA.x < posB.x)
			return RIGHT;
		else
			return LEFT;
	}
	if (verticalyAligned)
	{
		if (posA.y < posB.y)
			return DOWN;
		else
			return UP;
	}

	// um pouco de "engenharia alternativa"
	if (posA.y+halfSizeA.y-5 <= posB.y-halfSizeB.y)
	{
		return DOWN;
	}
	
	return findDirection(b.pos - a.pos);
}

string directionToString(const uint dir)
{
	switch (dir)
	{
	case LEFT:
		return "LEFT";
	case RIGHT:
		return "RIGHT";
	case UP:
		return "UP";
	case DOWN:
		return "DOWN";
	}
	return "";
}

uint findDirection(const vector3 v)
{
	return findDirection(vector2(v.x,v.y));
}

ETHEntityArray @findDestinationBuckets(ETHEntity @thisEntity, const vector2 dir, const bool forceBottomBuckets)
{
	ETHEntityArray entityArray;
	vector2 bucket = thisEntity.GetCurrentBucket();
	
	if (dir.x < 0)
		GetEntitiesFromBucket(bucket+vector2(-1,0), entityArray);
	if (dir.x > 0)
		GetEntitiesFromBucket(bucket+vector2(1,0), entityArray);

	if (dir.y > 0 || forceBottomBuckets)
		GetEntitiesFromBucket(bucket+vector2(0,1), entityArray);
	if (dir.y < 0)
		GetEntitiesFromBucket(bucket+vector2(0,-1), entityArray);
	
	if (forceBottomBuckets)
	{
		GetEntitiesFromBucket(bucket+vector2(-1,1), entityArray);
		GetEntitiesFromBucket(bucket+vector2(1,1), entityArray);
	}
	
	return @entityArray;
}

bool checkBoxHit(collisionBox box0, collisionBox box1)
{
	const vector3 halfSize0 = box0.size/2;
	const vector3 halfSize1 = box1.size/2;

	const vector3 v3Min0 = box0.pos-halfSize0;
	const vector3 v3Max0 = box0.pos+halfSize0;

	const vector3 v3Min1 = box1.pos-halfSize1;
	const vector3 v3Max1 = box1.pos+halfSize1;

	if (v3Min0.x > v3Max1.x)
		return false;
	if (v3Min0.y > v3Max1.y)
		return false;

	if (v3Max0.x < v3Min1.x)
		return false;
	if (v3Max0.y < v3Min1.y)
		return false;

	return true;
}

void drawRect(const uint rectColor)
{
	DrawRectangle(vector2(0,0), GetScreenSize(), rectColor, rectColor, rectColor, rectColor);
}

bool fadeIn(const uint startTime)
{
	const uint elapsed = GetTime()-startTime;
	if (elapsed < LIVE_FADE_IN_TIME)
	{
		const float alpha = 1.0f-(float(elapsed)/float(LIVE_FADE_IN_TIME));
		const uint rectColor = ARGB(uint8(alpha*255.0f),0,0,0);
		DrawRectangle(vector2(0,0), GetScreenSize(), rectColor, rectColor, rectColor, rectColor);
		return false;
	}
	return true;
}

bool fadeOut(const uint startTime, float &out bias)
{
	const uint elapsed = GetTime()-startTime;
	if (elapsed < LIVE_FADE_IN_TIME)
	{
		const float alpha = (float(elapsed)/float(LIVE_FADE_IN_TIME));
		bias = alpha;
		const uint rectColor = ARGB(uint8(alpha*255.0f),0,0,0);
		DrawRectangle(vector2(0,0), GetScreenSize(), rectColor, rectColor, rectColor, rectColor);
		return false;
	}
	return true;
}

void addToExp(const uint player, const int exp)
{
	g_exp[player] += exp;
	int nextExp = 0;
	g_gameData.getInt("global", "lv" + g_charLevel[player], nextExp);
	while (g_exp[player] >= nextExp)
	{
		const int diff = g_exp[player]-nextExp;
		g_charLevel[player]++;
		g_gameData.getInt("global", "lv" + g_charLevel[player], nextExp);
		g_exp[player] = diff;
	}
}

void addToHp(ETHEntity @thisEntity, const int value)
{
	int hp = thisEntity.GetIntData("hp");
	
	if (thisEntity.CheckCustomData("maxHp") == DT_NODATA)
	{
		hp = max(0, value+hp);
	}
	else
	{
		hp = max(0, min(value+hp, thisEntity.GetIntData("maxHp")));
	}
	thisEntity.AddIntData("hp", hp);
}

void addToMp(ETHEntity @thisEntity, const int value)
{
	int mp = thisEntity.GetIntData("mp");
	
	if (thisEntity.CheckCustomData("maxMp") == DT_NODATA)
	{
		mp = max(0, value+mp);
	}
	else
	{
		mp = max(0, min(value+mp, thisEntity.GetIntData("maxMp")));
	}
	thisEntity.AddIntData("mp", mp);
}

void shadowText(const vector2 pos, const string text, const string font, const float size,
				const uint8 a, const uint8 r, const uint8 g, const uint8 b)
{
	DrawText(pos+vector2(size*0.1f, size*0.1f), text, font, size, ARGB(a/2,0,0,0));
	DrawText(pos, text, font, size, ARGB(a,r,g,b));
}

void loadingMessage()
{
	shadowText(vector2(20,GetScreenSize().y-70), "Carregando...\n", "Arial Narrow", 60, 255, 203, 203, 228);
}

ETHEntity @findEntityInScreen(const string name)
{
	ETHEntityArray entities;
	GetVisibleEntities(entities);
	for (uint t=0; t<entities.size(); t++)
	{
		if (entities[t].GetEntityName() == name)
		{
			return entities[t];
		}
	}
	return null;
}

