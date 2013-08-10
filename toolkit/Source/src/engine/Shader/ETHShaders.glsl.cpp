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

#include "ETHShaders.h"

const std::string& ETHShaders::DefaultVS()
{
	return ETHGlobal::GLSL_default_vs;
}

static const std::string emptyStr;

const std::string& ETHShaders::DefaultPS()
{
	return emptyStr;
}

// ambient pass
const std::string& ETHShaders::Ambient_VS_Hor()
{
	return ETHGlobal::GLSL_hAmbient_vs;
}

const std::string& ETHShaders::Ambient_VS_Ver()
{
	return ETHGlobal::GLSL_vAmbient_vs;
}

// particles
const std::string& ETHShaders::Particle_VS()
{
	return ETHGlobal::GLSL_particle_vs;
}

// shadow
const std::string& ETHShaders::Shadow_VS_Ver()
{
	// TO-DO/TODO: implement shadows here
	return ETHGlobal::GLSL_default_vs;
}

// pixel light
const std::string& ETHShaders::PL_PS_Hor_Diff()
{
	return ETHGlobal::GLSL_hPixelLightDiff_ps;
}

const std::string& ETHShaders::PL_PS_Ver_Diff()
{
	return ETHGlobal::GLSL_vPixelLightDiff_ps;
}

const std::string& ETHShaders::PL_PS_Hor_Spec()
{
	// TO-DO/TODO: implement specular component
	return ETHGlobal::GLSL_hPixelLightDiff_ps;
}

const std::string& ETHShaders::PL_PS_Ver_Spec()
{
	// TO-DO/TODO: implement specular component
	return ETHGlobal::GLSL_vPixelLightDiff_ps;
}

const std::string& ETHShaders::PL_VS_Hor_Light()
{
	return ETHGlobal::GLSL_hPixelLight_vs;
}

const std::string& ETHShaders::PL_VS_Ver_Light()
{
	return ETHGlobal::GLSL_vPixelLight_vs;
}

const std::string& ETHShaders::VL_VS_Hor_Diff()
{
	return ETHGlobal::GLSL_default_vs;
}

const std::string& ETHShaders::VL_VS_Ver_Diff()
{
	return ETHGlobal::GLSL_default_vs;
}
