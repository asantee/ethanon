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

#ifndef ETH_SHADERS_H_
#define ETH_SHADERS_H_

#include <Types.h>
#include "../../shaders/shaders.h"

class ETHShaders
{
public:
	static const std::string& DefaultVS();
	static const std::string& DefaultPS();

	// ambient pass
	static const std::string& Ambient_VS_Hor();
	static const std::string& Ambient_VS_Ver();
	static const std::string& Highlight_PS();

	// particles
	static const std::string& Particle_VS();

	// shadow
	static const std::string& Shadow_VS_Ver();

	// pixel light
	static const std::string& PL_PS_Hor_Diff();
	static const std::string& PL_PS_Ver_Diff();
	static const std::string& PL_PS_Hor_Spec();
	static const std::string& PL_PS_Ver_Spec();

	static const std::string& PL_VS_Hor_Light();
	static const std::string& PL_VS_Ver_Light();
};

#endif