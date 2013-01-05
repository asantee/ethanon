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

#include "ETHDateTime.h"

#define _SCL_SECURE_NO_WARNINGS
#pragma warning( push )
#pragma warning( disable : 4127 )
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#pragma warning( pop )
#undef _SCL_SECURE_NO_WARNINGS

ETHDateTime::ETHDateTime()
{
	m_ref = 1;
	Update();
}

void ETHDateTime::AddRef()
{
	m_ref++;
}

void ETHDateTime::Release()
{
	if (--m_ref == 0)
	{
		delete this;
	}
}

void ETHDateTime::Update()
{
	boost::posix_time::ptime local_time = boost::posix_time::second_clock::local_time();
	boost::gregorian::date d = local_time.date();

	m_day = d.day();
	m_month = d.month();
	m_year = d.year();

	const boost::posix_time::time_duration& duration = local_time.time_of_day();
	m_hours   = duration.hours();
	m_minutes = duration.minutes();
	m_seconds = duration.seconds();
}

unsigned int ETHDateTime::GetDay() const
{
	return m_day;
}

unsigned int ETHDateTime::GetMonth() const
{
	return m_month;
}

unsigned int ETHDateTime::GetYear() const
{
	return m_year;
}

unsigned int ETHDateTime::GetHours() const
{
	return m_hours;
}

unsigned int ETHDateTime::GetMinutes() const
{
	return m_minutes;
}

unsigned int ETHDateTime::GetSeconds() const
{
	return m_seconds;
}
