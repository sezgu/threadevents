#pragma once
#include <chrono>
#include <memory>

namespace threading
{
	struct AutoResetEvent
	{
		explicit AutoResetEvent(bool initialState);

		template<typename _Rep, typename _Period>
		bool wait(std::chrono::duration<_Rep, _Period> duration) const
		{
			return _wait(std::chrono::duration_cast<std::chrono::milliseconds, _Rep, _Period>(duration));
		}
		void wait() const;
		bool wait(unsigned milliseconds) const;
		void set();

	private:
		struct _EventData;
		bool _wait(std::chrono::milliseconds duration) const;
		std::shared_ptr<_EventData> m_data;


	};

	struct ManualResetEvent
	{
		explicit ManualResetEvent(bool initialState);

		template<typename _Rep, typename _Period>
		bool wait(std::chrono::duration<_Rep, _Period> duration) const
		{
			return _wait(std::chrono::duration_cast<std::chrono::milliseconds, _Rep, _Period>(duration));
		}
		void wait() const;
		bool wait(unsigned milliseconds) const;
		void set();
		void reset();
	private:
		struct _EventData;
		bool _wait(std::chrono::milliseconds duration) const;
		std::shared_ptr<_EventData> m_data;


	};

}





