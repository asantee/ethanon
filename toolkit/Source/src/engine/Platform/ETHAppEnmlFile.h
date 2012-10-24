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

#ifndef ETH_APP_ENML_FILE_H_
#define ETH_APP_ENML_FILE_H_

#include <Enml/Enml.h>
#include <Platform/FileManager.h>
#include "../Resource/ETHSpriteDensityManager.h"

class ETHAppEnmlFile
{
public:
	ETHAppEnmlFile(
		const gs2d::str_type::string& fileName,
		const Platform::FileManagerPtr& fileManager,
		const gs2d::str_type::string& platformName);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	float GetHdDensityValue() const;
	float GetFullHdDensityValue() const;
	unsigned int GetMinScreenHeightForHdVersion() const;
	unsigned int GetMinScreenHeightForFullHdVersion() const;

	bool IsWindowed() const;
	bool IsVsyncEnabled() const;
	bool IsRichLightingEnabled() const;
	gs2d::str_type::string GetTitle() const;
	gs2d::str_type::string GetFixedWidth() const;
	gs2d::str_type::string GetFixedHeight() const;

	const ETHSpriteDensityManager& GetDensityManager() const;
	const std::vector<gs2d::str_type::string>& GetDefinedWords() const;

private:
	void LoadProperties(const gs2d::str_type::string& platformName, const gs2d::enml::File& file);

	ETHSpriteDensityManager densityManager;

	unsigned int width, height;

	float hdDensityValue;
	float fullHdDensityValue;
	unsigned int minScreenHeightForHdVersion, minScreenHeightForFullHdVersion;

	bool windowed, vsync;
	bool richLighting;
	gs2d::str_type::string title;
	gs2d::str_type::string fixedWidth, fixedHeight;

	std::vector<gs2d::str_type::string> definedWords;
};

#endif
