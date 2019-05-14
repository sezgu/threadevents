#include "pch.h"
#include "ThreadEvents.h"
#include <atomic>
#include <mutex>

using namespace threading;

struct ManualResetEvent::_EventData
{
	std::atomic<bool> m_signal;
	std::mutex m_signalMutex;
	std::condition_variable m_cv;
};


void ManualResetEvent::wait() const
{
	std::unique_lock<std::mutex> lock{ m_data->m_signalMutex };

	bool result = m_data->m_signal;
	while (!result)
	{
		m_data->m_cv.wait(lock);
		result = m_data->m_signal;
	}
}

bool ManualResetEvent::_wait(std::chrono::milliseconds duration) const
{
	std::unique_lock<std::mutex> lock{ m_data->m_signalMutex };

	auto timeout = std::chrono::steady_clock::now() + duration;
	bool result = m_data->m_signal;
	while (!result)
	{
		if (m_data->m_cv.wait_until(lock, timeout) == std::cv_status::timeout)
			break;
		else
		{
			result = m_data->m_signal;
		}
	}
	return result;

}

bool ManualResetEvent::wait(unsigned milliseconds) const
{
	return wait(std::chrono::milliseconds(milliseconds));
}

void ManualResetEvent::set()
{
	std::unique_lock<std::mutex> lock{ m_data->m_signalMutex };
	m_data->m_signal = true;
	m_data->m_cv.notify_all();

}

void ManualResetEvent::reset()
{
	std::unique_lock<std::mutex> lock{ m_data->m_signalMutex };
	m_data->m_signal = false;
}

ManualResetEvent::ManualResetEvent(bool initialState)
	: m_data(std::make_shared<ManualResetEvent::_EventData>())
{
	m_data->m_signal = initialState;
}
