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

#include "RecoverableResourceManager.h"

namespace gs2d {

void RecoverableResourceManager::InsertRecoverableResource(RecoverableResource* resource)
{
	m_resources.push_back(resource);
}

bool RecoverableResourceManager::RemoveRecoverableResource(const RecoverableResource* resource)
{
	for (std::list<RecoverableResource*>::iterator iter = m_resources.begin(); iter != m_resources.end();)
	{
		if ((*iter) == resource)
			iter = m_resources.erase(iter);
		else
			++iter;
	}
	return true;
}

void RecoverableResourceManager::RecoverAll()
{
	for (std::list<RecoverableResource*>::iterator iter = m_resources.begin(); iter != m_resources.end(); ++iter)
	{
		(*iter)->Recover();
	}
}

} // namespace gs2d
