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

#ifndef ETH_SPRITE_DENSITY_MANAGER_H_
#define ETH_SPRITE_DENSITY_MANAGER_H_

#include <Enml/Enml.h>
#include <Video.h>

class ETHAppEnmlFile;

class ETHSpriteDensityManager
{
public:
	static const gs2d::str_type::string HD_VERSION_PATH_NAME;
	static const gs2d::str_type::string FULL_HD_VERSION_PATH_NAME;
	static const gs2d::str_type::string LD_VERSION_PATH_NAME;
	static const gs2d::str_type::string XLD_VERSION_PATH_NAME;

	enum DENSITY_LEVEL
	{
		NORMAL = 0,
		HD = 1,
		FULL_HD = 2,
		LD = 3, // low-def
		XLD = 4 // very low-def
	};

	ETHSpriteDensityManager();
	void FillParametersFromFile(const ETHAppEnmlFile& file);

	bool ShouldUseHdResources(const gs2d::VideoPtr& video) const;
	bool ShouldUseFullHdResources(const gs2d::VideoPtr& video) const;

	bool ShouldUseLdResources(const gs2d::VideoPtr& video) const;
	bool ShouldUseXLdResources(const gs2d::VideoPtr& video) const;

	gs2d::str_type::string ChooseSpriteVersion(const gs2d::str_type::string& fullFilePath,
		const gs2d::VideoPtr& video, ETHSpriteDensityManager::DENSITY_LEVEL& densityLevel);
	void SetSpriteDensity(const gs2d::SpritePtr& sprite, const DENSITY_LEVEL& level) const;
	float GetDensityValue(const DENSITY_LEVEL& level) const;

	float hdDensityValue;
	float fullHdDensityValue;
	float ldDensityValue;
	float xldDensityValue;
	unsigned int minScreenHeightForHdResources, minScreenHeightForFullHdResources;
	unsigned int maxScreenHeightBeforeNdVersion, maxScreenHeightBeforeLdVersion;
};

#endif