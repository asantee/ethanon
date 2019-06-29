#ifndef GS2D_RECOVERABLE_RESOURCE_H_
#define GS2D_RECOVERABLE_RESOURCE_H_

namespace gs2d {

/**
 * \brief Resource object that may be restored after device loss
*/
class RecoverableResource
{
public:
	virtual void Recover() = 0;
};

} // namespace gs2d

#endif
