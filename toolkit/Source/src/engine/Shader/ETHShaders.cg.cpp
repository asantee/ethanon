#include "ETHShaders.h"

#include "../../shaders/shaders.h"

const std::string& ETHShaders::DefaultVS()
{
	return ETHGlobal::Cg_defaultVS_cg;
}

static const std::string emptyStr;

const std::string& ETHShaders::DefaultPS()
{
	return ETHGlobal::Cg_defaultPS_cg;
}

// ambient pass
const std::string& ETHShaders::Ambient_VS_Hor()
{
	return ETHGlobal::Cg_hAmbientVS_cg;
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

