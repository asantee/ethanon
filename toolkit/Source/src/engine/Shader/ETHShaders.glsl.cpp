#include "ETHShaders.h"

#include "../../shaders/shaders.h"

const std::string& ETHShaders::DefaultVS()
{
	return ETHGlobal::GLSL_default_vs;
}

const std::string& ETHShaders::DefaultPS()
{
	return ETHGlobal::GLSL_default_ps;
}

// ambient pass
const std::string& ETHShaders::Ambient_VS_Hor()
{
	return ETHGlobal::GLSL_hAmbient_vs;
}

const std::string& ETHShaders::Highlight_PS()
{
	return ETHGlobal::GLSL_highlight_ps;
}

const std::string& ETHShaders::SolidColor_PS()
{
	return ETHGlobal::GLSL_solid_ps;
}

// particles
const std::string& ETHShaders::Particle_VS()
{
	return ETHGlobal::GLSL_particle_vs;
}
