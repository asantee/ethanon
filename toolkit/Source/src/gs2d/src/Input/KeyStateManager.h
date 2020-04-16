#ifndef GS2D_KEY_STATE_MANAGER_H_
#define GS2D_KEY_STATE_MANAGER_H_

namespace gs2d {

enum GS_KEY_STATE
{
	GSKS_UP = 0,
	GSKS_HIT = 1,
	GSKS_DOWN = 2,
	GSKS_RELEASE = 3
};

class KeyStateManager
{
	int m_pressingElapsedFrameCount;
	GS_KEY_STATE m_currentState;

public:
	KeyStateManager();
	void Update(const bool isPressed);
	GS_KEY_STATE GetCurrentState() const;
};

} // namespace gs2d

#endif
