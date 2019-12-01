#pragma once
#include "ThreadEvents.h"
#include <type_traits>

namespace threading
{
	template<typename T, typename = typename std::enable_if<!std::is_reference_v<T>>::type>
	struct Signal
	{
		template<typename _Rep, typename _Period>
		bool wait(std::chrono::duration<_Rep, _Period> duration, T& outValue) const
		{
			if (m_event.wait(duration))
			{
				outValue = std::move(m_value);
				return true;
			}
			return false;
		}
		T wait() const
		{
			m_event.wait();
			return std::move(m_value);
		}
		bool wait(unsigned milliseconds, T& outValue) const
		{
			if (m_event.wait(milliseconds))
			{
				outValue = std::move(m_value);
				return true;
			}
			return false;
		}
		void set(T&& value)
		{
			m_value = std::move(value);
			m_event.set();
		}

	private:
		AutoResetEvent m_event{false};
		mutable T m_value{};
	};



	



}
