#ifndef KEY_PROVIDER_H_
#define KEY_PROVIDER_H_

#include <string>

namespace Platform {

class KeyProvider
{
public:
	static std::string ProvideKey();
};

} // namespace Platform

#endif
