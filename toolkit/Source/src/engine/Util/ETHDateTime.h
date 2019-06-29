#ifndef ETH_DATE_TIME_H_
#define ETH_DATE_TIME_H_

class ETHDateTime
{
public:
	void AddRef();
	void Release();

	ETHDateTime();
	void Update();

	unsigned int GetDay() const;
	unsigned int GetMonth() const;
	unsigned int GetYear() const;
	unsigned int GetHours() const;
	unsigned int GetMinutes() const;
	unsigned int GetSeconds() const;

private:
	unsigned int m_day;
	unsigned int m_month;
	unsigned int m_year;
	unsigned int m_hours;
	unsigned int m_minutes;
	unsigned int m_seconds;
	int m_ref;
};

#endif