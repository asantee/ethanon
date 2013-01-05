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

#ifndef ETH_DIRECTORIES_H_
#define ETH_DIRECTORIES_H_

#include <Types.h>

class ETHDirectories
{
public:
	static gs2d::str_type::string GetShaderDirectory();
	static gs2d::str_type::string GetBitmapFontDirectory();
	static gs2d::str_type::string GetSceneDirectory();
	static gs2d::str_type::string GetEntityDirectory();
	static gs2d::str_type::string GetNormalMapDirectory();
	static gs2d::str_type::string GetHaloDirectory();
	static gs2d::str_type::string GetParticlesDirectory();
	static gs2d::str_type::string GetEffectsDirectory();
	static gs2d::str_type::string GetSoundFXDirectory();
};

#endif