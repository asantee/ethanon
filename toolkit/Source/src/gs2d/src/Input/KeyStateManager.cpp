#include "KeyStateManager.h"

namespace gs2d {

KeyStateManager::KeyStateManager() :
	m_pressingElapsedFrameCount(0),
	m_currentState(GSKS_UP)
{
}

void KeyStateManager::Update(const bool isPressed)
{
	if (isPressed)
	{
		m_pressingElapsedFrameCount++;
	}
	else if (m_pressingElapsedFrameCount > 0)
	{
		m_pressingElapsedFrameCount = 0;
		m_currentState = GSKS_RELEASE;
		return;
	}
	else if (m_pressingElapsedFrameCount == 0)
	{
		m_currentState = GSKS_UP;
		return;
	}

	if (m_pressingElapsedFrameCount == 1)
	{
		m_currentState = GSKS_HIT;
		return;
	}
	m_currentState = GSKS_DOWN;
}

GS_KEY_STATE KeyStateManager::GetCurrentState() const
{
	return m_currentState;
}

} // namespace gs2d
