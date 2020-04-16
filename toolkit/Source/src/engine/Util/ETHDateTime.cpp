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

int64_t ETHDateTime::GetDay() const
{
	return m_day;
}

int64_t ETHDateTime::GetMonth() const
{
	return m_month;
}

int64_t ETHDateTime::GetYear() const
{
	return m_year;
}

int64_t ETHDateTime::GetHours() const
{
	return m_hours;
}

int64_t ETHDateTime::GetMinutes() const
{
	return m_minutes;
}

int64_t ETHDateTime::GetSeconds() const
{
	return m_seconds;
}
