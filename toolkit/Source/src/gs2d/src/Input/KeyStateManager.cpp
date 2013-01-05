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
