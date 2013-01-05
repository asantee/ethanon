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

gs2d::str_type::string ETHShaders::DefaultVS()
{
	return GS_L("defaultVS.cg");
}

gs2d::str_type::string ETHShaders::DefaultPS()
{
	return GS_L("");
}

// ambient pass
gs2d::str_type::string ETHShaders::Ambient_VS_Hor()
{
	return GS_L("hAmbientVS.cg");
}

gs2d::str_type::string ETHShaders::Ambient_VS_Ver()
{
	return GS_L("vAmbientVS.cg");
}

// particles
gs2d::str_type::string ETHShaders::Particle_VS()
{
	return GS_L("particleVS.cg");
}

// shadow
gs2d::str_type::string ETHShaders::Shadow_VS_Ver()
{
	return GS_L("dynaShadowVS.cg");
}

// pixel light
gs2d::str_type::string ETHShaders::PL_PS_Hor_Diff()
{
	return GS_L("hPixelLightDiff.cg");
}

gs2d::str_type::string ETHShaders::PL_PS_Ver_Diff()
{
	return GS_L("vPixelLightDiff.cg");
}

gs2d::str_type::string ETHShaders::PL_PS_Hor_Spec()
{
	return GS_L("hPixelLightSpec.cg");
}

gs2d::str_type::string ETHShaders::PL_PS_Ver_Spec()
{
	return GS_L("vPixelLightSpec.cg");
}

gs2d::str_type::string ETHShaders::PL_VS_Hor_Light()
{
	return GS_L("hPixelLightVS.cg");
}

gs2d::str_type::string ETHShaders::PL_VS_Ver_Light()
{
	return GS_L("vPixelLightVS.cg");
}

gs2d::str_type::string ETHShaders::VL_VS_Hor_Diff()
{
	return GS_L("hVertexLightShader.cg");
}

gs2d::str_type::string ETHShaders::VL_VS_Ver_Diff()
{
	return GS_L("vVertexLightShader.cg");
}
