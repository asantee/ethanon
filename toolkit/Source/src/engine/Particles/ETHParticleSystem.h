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

#ifndef ETH_PARTICLE_SYSTEM_H_
#define ETH_PARTICLE_SYSTEM_H_

#include "../ETHTypes.h"
#include <Platform/FileManager.h>
#include <Video.h>

class ETHParticleSystem
{
public:
	enum FRAME_ANIMATION_MODE
	{
		PLAY_ANIMATION = 1,
		PICK_RANDOM_FRAME = 2
	};

	ETHParticleSystem();
	void Reset();

	void Scale(const float scale);
	void MirrorX(const bool mirrorGravity);
	void MirrorY(const bool mirrorGravity);

	bool ReadFromXMLFile(TiXmlElement *pElement);

	bool ReadFromFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager);
	bool WriteToXMLFile(TiXmlElement *pRoot) const;
	int GetNumFrames() const;
	
	bool ShouldUseHighlightPS() const;

	str_type::string GetActualBitmapFile() const;

	str_type::string bitmapFile;
	ETH_BOOL allAtOnce;
	float boundingSphere;
	Video::ALPHA_MODE alphaMode;
	int nParticles;
	Vector2 gravityVector;
	Vector2 directionVector;
	Vector2 randomizeDir;
	Vector3 startPoint;
	Vector2 randStartPoint;

	Vector2i spriteCut;

	Vector4 color0;
	Vector4 color1;

	float lifeTime;
	float randomizeLifeTime;

	float angleDir;
	float randAngle;
	float size;
	float randomizeSize;
	float growth;
	float minSize;
	float maxSize;

	int repeat;
	int animationMode;
	Vector3 emissive;
	float angleStart;
	float randAngleStart;
};

#endif
