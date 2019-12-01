#include "pch.h"
#include "ThreadEvents.h"
#include <mutex>

using namespace threading;

struct ManualResetEvent::_EventData
{
	bool Value{false};
	std::mutex Mutex{};
	std::condition_variable Signal{};
};


void ManualResetEvent::wait() const
{
	std::unique_lock<std::mutex> lock{ m_data->Mutex };

	m_data->Signal.wait(lock, [&]() {return m_data->Value; });
}

bool ManualResetEvent::_wait(std::chrono::milliseconds duration) const
{
	std::unique_lock<std::mutex> lock{ m_data->Mutex };
	
	return m_data->Signal.wait_for(lock, duration, [&]() {return m_data->Value; });
}

bool ManualResetEvent::wait(unsigned milliseconds) const
{
	return wait(std::chrono::milliseconds(milliseconds));
}

void ManualResetEvent::set()
{
	std::unique_lock<std::mutex> lock{ m_data->Mutex };
	m_data->Value = true;
	m_data->Signal.notify_all();

}

void ManualResetEvent::reset()
{
	std::unique_lock<std::mutex> lock{ m_data->Mutex };
	m_data->Value = false;
}

ManualResetEvent::ManualResetEvent(bool initialState)
	: m_data(std::make_shared<ManualResetEvent::_EventData>())
{
	m_data->Value = initialState;
}
