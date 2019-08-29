#ifndef ETH_DATE_TIME_H_
#define ETH_DATE_TIME_H_
#include <cstdint>

class ETHDateTime
{
public:
	void AddRef();
	void Release();

	ETHDateTime();
	void Update();

	int64_t GetDay() const;
	int64_t GetMonth() const;
	int64_t GetYear() const;
	int64_t GetHours() const;
	int64_t GetMinutes() const;
	int64_t GetSeconds() const;

private:
	int64_t m_day;
	int64_t m_month;
	int64_t m_year;
	int64_t m_hours;
	int64_t m_minutes;
	int64_t m_seconds;
	int32_t m_ref;
};

#endif
