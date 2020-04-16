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

	bool ReadFromFile(const std::string& fileName, const Platform::FileManagerPtr& fileManager);
	bool WriteToXMLFile(TiXmlElement *pRoot) const;
	int GetNumFrames() const;
	
	bool ShouldUseHighlightPS() const;

	std::string GetActualBitmapFile() const;

	std::string bitmapFile;
	ETH_BOOL allAtOnce;
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
