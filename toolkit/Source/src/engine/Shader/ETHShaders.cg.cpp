#include "ETHShaders.h"

#include "../../shaders/shaders.h"

const std::string& ETHShaders::DefaultVS()
{
	return ETHGlobal::defaultVS_cg;
}

static const std::string emptyStr;

const std::string& ETHShaders::DefaultPS()
{
	return ETHGlobal::defaultPS_cg;
}

// ambient pass
const std::string& ETHShaders::Ambient_VS_Hor()
{
	return ETHGlobal::hAmbientVS_cg;
}

const std::string& ETHShaders::Highlight_PS()
{
	return ETHGlobal::highlightPS_cg;
}

const std::string& ETHShaders::SolidColor_PS()
{
	return ETHGlobal::solidPS_cg;
}

// particles
const std::string& ETHShaders::Particle_VS()
{
	return ETHGlobal::particleVS_cg;
}

