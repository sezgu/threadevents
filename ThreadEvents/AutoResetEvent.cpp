#include "pch.h"
#include "ThreadEvents.h"
#include <atomic>
#include <mutex>

using namespace std::literals::chrono_literals;

using namespace threading;

struct AutoResetEvent::_EventData
{
	std::mutex m_signalMutex;
	std::atomic<bool> m_signal;
	std::condition_variable m_cv;

	std::mutex m_waitMutex;
	std::atomic<unsigned> m_waitCounter;
	std::condition_variable m_waitCv;
	void wait_for_count(unsigned count, std::unique_lock<std::mutex>& lock);
	
};

void AutoResetEvent::_EventData::wait_for_count(unsigned count, std::unique_lock<std::mutex>& lock)
{
	bool result = m_waitCounter == count;
	while (!result)
	{
		m_waitCv.wait(lock);
		result = m_waitCounter == count;
	}
}


void AutoResetEvent::wait() const
{
	std::unique_lock<std::mutex> lock{ m_data->m_signalMutex };
	m_data->m_waitCounter++;

	bool result = m_data->m_signal.exchange(false);
	while (!result)
	{
		m_data->m_cv.wait(lock); 
		result = m_data->m_signal.exchange(false);
	}
	m_data->m_waitCounter--;
	m_data->m_waitCv.notify_all();
}

bool AutoResetEvent::_wait(std::chrono::milliseconds duration) const
{
	std::unique_lock<std::mutex> lock{ m_data->m_signalMutex };
	m_data->m_waitCounter++;

	auto timeout = std::chrono::steady_clock::now() + duration;
	bool result = m_data->m_signal.exchange(false);
	while (!result)
	{
		if (m_data->m_cv.wait_until(lock, timeout) == std::cv_status::timeout)
			break;
		else
		{
			result = m_data->m_signal.exchange(false);
		}
	}
	m_data->m_waitCounter--;
	m_data->m_waitCv.notify_all();
	return result;
	
}

bool AutoResetEvent::wait(unsigned milliseconds) const
{
	return wait(std::chrono::milliseconds(milliseconds));
}

void AutoResetEvent::set()
{
	std::unique_lock<std::mutex> waitlock{ m_data->m_waitMutex };
	std::unique_lock<std::mutex> lock{ m_data->m_signalMutex };
	unsigned initial = m_data->m_waitCounter;
	m_data->m_signal = true;
	m_data->m_cv.notify_one();

	if (initial > 0)
	{
		m_data->wait_for_count(initial - 1,lock);
	}
	
}

AutoResetEvent::AutoResetEvent(bool initialState)
	: m_data(std::make_shared<AutoResetEvent::_EventData>())
{
	m_data->m_signal = initialState;
	m_data->m_waitCounter = 0;
}




