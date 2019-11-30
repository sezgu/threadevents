#include "pch.h"
#include "ThreadEvents.h"
#include <atomic>
#include <mutex>

using namespace std::literals::chrono_literals;

using namespace threading;

struct AutoResetEvent::_EventData
{
	bool Value{false};
	int WaitCounter{0};
	std::mutex Mutex{};
	std::mutex SetMutex{};
	std::condition_variable Signal{};
	std::condition_variable WaitSignal{};
};

void AutoResetEvent::wait() const
{
	std::unique_lock<std::mutex> lock{ m_data->Mutex };

	++m_data->WaitCounter;
	m_data->Signal.wait(lock, [&]() {return m_data->Value; });
	m_data->Value = false;
	--m_data->WaitCounter;
	m_data->WaitSignal.notify_one();
}

bool AutoResetEvent::_wait(std::chrono::milliseconds duration) const
{
	std::unique_lock<std::mutex> lock{ m_data->Mutex };
	
	++m_data->WaitCounter;
	auto result = m_data->Signal.wait_for(lock, duration, [&]() { return m_data->Value; });
	m_data->Value = false;
	--m_data->WaitCounter;
	m_data->WaitSignal.notify_one();
	return result;
}

bool AutoResetEvent::wait(unsigned milliseconds) const
{
	return wait(std::chrono::milliseconds(milliseconds));
}

void AutoResetEvent::set()
{
	std::lock_guard<std::mutex> set_lock{ m_data->SetMutex };
	std::unique_lock<std::mutex> lock{ m_data->Mutex };

	auto initial = m_data->WaitCounter;
	m_data->Value = true;
	m_data->Signal.notify_one();
	if (initial > 0)
	{
		m_data->WaitSignal.wait(lock, [&]() {return m_data->WaitCounter < initial; });
	}
}

AutoResetEvent::AutoResetEvent(bool initialState)
	: m_data(std::make_shared<_EventData>())
{
	m_data->Value = initialState;
}




