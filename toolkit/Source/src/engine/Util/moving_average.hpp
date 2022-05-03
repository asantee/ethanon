#pragma once
#include <assert.h>
#include <vector>

template <typename T = float>
class MovingAverage
{
	std::vector<T> m_values;
	int m_window_size;
	int m_current_index = -1;
	T m_sum = 0;
	T m_average = 0;

public:
	MovingAverage(int window = 5);
	void update(T value);
	T get_average();
	T get_value();
};

template <typename T>
MovingAverage<T>::MovingAverage(int window) : m_window_size(window)
{
	assert(m_window_size > 0);
}

template <typename T>
void MovingAverage<T>::update(T value)
{
	if(++m_current_index >= m_window_size)
		m_current_index = 0;

	if((int)m_values.size() < m_window_size)
	{
		m_values.push_back(value);
		m_sum += value;
	}
	else
	{
		m_sum -= m_values[m_current_index];
		m_values[m_current_index] = value;
		m_sum += value;
	}
	m_average = m_sum / m_values.size();
}

template <typename T>
T MovingAverage<T>::get_average()
{
	return m_average;
}

template <typename T>
T MovingAverage<T>::get_value()
{
	return m_values[m_current_index];
}
