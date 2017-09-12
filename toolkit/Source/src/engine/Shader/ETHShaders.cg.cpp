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
	return ETHGlobal::Cg_defaultVS_cg;
}

static const std::string emptyStr;

const std::string& ETHShaders::DefaultPS()
{
	return emptyStr;
}

// ambient pass
const std::string& ETHShaders::Ambient_VS_Hor()
{
	return ETHGlobal::Cg_hAmbientVS_cg;
}

const std::string& ETHShaders::Ambient_VS_Ver()
{
	return ETHGlobal::Cg_vAmbientVS_cg;
}

const std::string& ETHShaders::Highlight_PS()
{
	return ETHGlobal::Cg_highlightPS_cg;
}

const std::string& ETHShaders::SolidColor_PS()
{
	return ETHGlobal::Cg_solidPS_cg;
}

// particles
const std::string& ETHShaders::Particle_VS()
{
	return ETHGlobal::Cg_particleVS_cg;
}

// shadow
const std::string& ETHShaders::Shadow_VS_Ver()
{
	return ETHGlobal::Cg_dynaShadowVS_cg;
}

// pixel light
const std::string& ETHShaders::PL_PS_Hor_Diff()
{
	return ETHGlobal::Cg_hPixelLightDiff_cg;
}

const std::string& ETHShaders::PL_PS_Ver_Diff()
{
	return ETHGlobal::Cg_vPixelLightDiff_cg;
}

const std::string& ETHShaders::PL_PS_Hor_Spec()
{
	return ETHGlobal::Cg_hPixelLightSpec_cg;
}

const std::string& ETHShaders::PL_PS_Ver_Spec()
{
	return ETHGlobal::Cg_vPixelLightSpec_cg;
}

const std::string& ETHShaders::PL_VS_Hor_Light()
{
	return ETHGlobal::Cg_hPixelLightVS_cg;
}

const std::string& ETHShaders::PL_VS_Ver_Light()
{
	return ETHGlobal::Cg_vPixelLightVS_cg;
}
