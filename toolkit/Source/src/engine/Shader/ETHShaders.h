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

class ETHShaders
{
public:
	static gs2d::str_type::string DefaultVS();
	static gs2d::str_type::string DefaultPS();

	// ambient pass
	static gs2d::str_type::string Ambient_VS_Hor();
	static gs2d::str_type::string Ambient_VS_Ver();

	// particles
	static gs2d::str_type::string Particle_VS();

	// shadow
	static gs2d::str_type::string Shadow_VS_Ver();

	// pixel light
	static gs2d::str_type::string PL_PS_Hor_Diff();
	static gs2d::str_type::string PL_PS_Ver_Diff();
	static gs2d::str_type::string PL_PS_Hor_Spec();
	static gs2d::str_type::string PL_PS_Ver_Spec();

	static gs2d::str_type::string PL_VS_Hor_Light();
	static gs2d::str_type::string PL_VS_Ver_Light();

	// vertex light
	static gs2d::str_type::string VL_VS_Hor_Diff();
	static gs2d::str_type::string VL_VS_Ver_Diff();
};

#endif