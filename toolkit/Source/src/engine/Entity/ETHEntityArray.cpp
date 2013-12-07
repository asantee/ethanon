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

#include "ETHEntityArray.h"
#include <set>

ETHEntityArray::ETHEntityArray()
{
	m_ref = 1;
}

ETHEntityArray::~ETHEntityArray()
{
	clear();
}

void ETHEntityArray::AddRef()
{
	m_ref++;
}

void ETHEntityArray::Release()
{
	if (--m_ref == 0)
	{
		delete this;
	}
}

void ETHEntityArray::push_back(ETHEntityRawPtr p)
{
	p->AddRef();
	m_vector.push_back(p);
}

unsigned int ETHEntityArray::size() const
{
	return static_cast<unsigned int>(m_vector.size());
}

ETHEntityRawPtr &ETHEntityArray::operator[] (const unsigned int index)
{
	return m_vector[index];
}

const ETHEntityRawPtr &ETHEntityArray::operator[] (const unsigned int index) const
{
	return m_vector[index];
}

ETHEntityRawPtr &ETHEntityArray::operator[] (const unsigned long index)
{
	return (*this)[static_cast<unsigned int>(index)];
}

const ETHEntityRawPtr &ETHEntityArray::operator[] (const unsigned long index) const
{
	return (*this)[static_cast<unsigned int>(index)];
}

ETHEntityArray &ETHEntityArray::operator+=(const ETHEntityArray &other)
{
	const unsigned int nSize = other.size();
	for (unsigned int t = 0; t < nSize; t++)
	{
		push_back(other[t]);
	}
	return *this;
}

void ETHEntityArray::clear()
{
	const unsigned int nSize = size();
	if (nSize)
	{
		for (unsigned int t = 0; t < nSize; t++)
		{
			if (m_vector[t])
			{
				m_vector[t]->Release();
			}
		}
	}
	m_vector.clear();
}

ETHEntityArray &ETHEntityArray::operator=(const ETHEntityArray &other)
{
	clear();
	m_vector.resize(other.size());
	for (unsigned int t = 0; t < other.size(); t++)
	{
		m_vector[t] = other[t];
	}
	return *this;
}

void ETHEntityArray::unique()
{
	removeDeadEntities();
	std::set<ETHEntityRawPtr> entitySet;
	for (std::vector<ETHEntityRawPtr>::iterator iter = m_vector.begin(); iter != m_vector.end(); ++iter)
	{
		entitySet.insert(*iter);
	}
	clear();
	for (std::set<ETHEntityRawPtr>::iterator iter = entitySet.begin(); iter != entitySet.end(); ++iter)
	{
		push_back(*iter);
	}
}

void ETHEntityArray::removeDeadEntities()
{
	for (std::vector<ETHEntityRawPtr>::iterator iter = m_vector.begin();
		 iter != m_vector.end();)
	{
		if (!((*iter)->IsAlive()))
		{
			(*iter)->Release();
			iter = m_vector.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}