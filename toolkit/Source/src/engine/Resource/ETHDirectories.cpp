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

#include "ETHDirectories.h"

#define ETH_SHADER_DIRECTORY		GS_L("data/")
#define ETH_BITMAP_FONT_DIRECTORY	GS_L("data/")
#define ETH_SCENES_DIRECTORY		GS_L("scenes/")
#define ETH_ENTITY_DIRECTORY		GS_L("entities/")
#define ETH_NORMAL_DIRECTORY		GS_L("entities/normalmaps/")
#define ETH_HALOS_DIRECTORY			GS_L("entities/")
#define ETH_PARTICLES_DIRECTORY		GS_L("particles/")
#define ETH_EFFECTS_DIRECTORY		GS_L("effects/")
#define ETH_SOUNDFX_DIRECTORY		GS_L("soundfx/")

gs2d::str_type::string ETHDirectories::GetShaderDirectory()
{
	return ETH_SHADER_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetBitmapFontDirectory()
{
	return ETH_BITMAP_FONT_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetSceneDirectory()
{
	return ETH_SCENES_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetEntityDirectory()
{
	return ETH_ENTITY_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetNormalMapDirectory()
{
	return ETH_NORMAL_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetHaloDirectory()
{
	return ETH_HALOS_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetParticlesDirectory()
{
	return ETH_PARTICLES_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetEffectsDirectory()
{
	return ETH_EFFECTS_DIRECTORY;
}

gs2d::str_type::string ETHDirectories::GetSoundFXDirectory()
{
	return ETH_SOUNDFX_DIRECTORY;
}
