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
	static const std::string& Highlight_PS();
	static const std::string& SolidColor_PS();

	// particles
	static const std::string& Particle_VS();

	// pixel light
	static const std::string& PL_PS_Hor_Diff();
	static const std::string& PL_PS_Hor_Spec();

	static const std::string& PL_VS_Hor_Light();
};

#endif
