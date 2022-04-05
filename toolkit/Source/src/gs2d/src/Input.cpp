#include "Input.h"

namespace gs2d {

int Input::GetKeyStateMultiplicationValue(const GS_KEY_STATE state)
{
	switch (state)
	{
		case GSKS_RELEASE: return 1;
		case GSKS_HIT:     return 2;
		case GSKS_DOWN:    return 3;
		default: return 0;
	}
}

GS_KEY_STATE Input::SumKeyStates(const GS_KEY_STATE a, const GS_KEY_STATE b)
{
	return (GetKeyStateMultiplicationValue(a) > GetKeyStateMultiplicationValue(b)) ? a : b;
}

} //namespace gs2d
