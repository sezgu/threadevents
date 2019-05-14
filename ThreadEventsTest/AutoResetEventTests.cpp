#include "pch.h"
#include "ThreadEvents.h"

using namespace std::literals::chrono_literals;
using namespace threading;

TEST(AutoResetEventTests, SingleThreadSet) 
{
	AutoResetEvent myEvent{false};

	bool result = myEvent.wait(10ms);
	ASSERT_FALSE(result);

	myEvent.set();

	result = myEvent.wait(10ms);
	ASSERT_TRUE(result);

	result = myEvent.wait(10ms);
	ASSERT_FALSE(result);

	myEvent.set();
	myEvent.wait();
}

TEST(AutoResetEventTests, MultiThreadSingleSet)
{
	AutoResetEvent myEvent{ false };

	std::optional<bool> firstThreadResult = std::nullopt;
	std::optional<bool> secondThreadResult = std::nullopt;

	auto firstThread = std::thread([&myEvent, &firstThreadResult]()
	{
		std::this_thread::sleep_for(10ms);
		if (myEvent.wait(100ms))
		{
			firstThreadResult = true;
		}
		std::cout << "First thread exiting.\r\n";
	});

	auto secondThread = std::thread([&myEvent, &secondThreadResult]()
	{
		if (myEvent.wait(100ms))
		{
			secondThreadResult = true;
		}
		std::cout << "Second thread exiting.\r\n";
	});

	std::this_thread::sleep_for(10ms);

	ASSERT_EQ(std::nullopt, firstThreadResult);
	ASSERT_EQ(std::nullopt, secondThreadResult);

	myEvent.set();

	firstThread.join();
	secondThread.join();

	ASSERT_FALSE(firstThreadResult.has_value() && secondThreadResult.has_value());
	ASSERT_TRUE(firstThreadResult.has_value() || secondThreadResult.has_value());

	ASSERT_TRUE(firstThreadResult.has_value() ? firstThreadResult.value() : true);
	ASSERT_TRUE(secondThreadResult.has_value() ? secondThreadResult.value() : true);

}

TEST(AutoResetEventTests, MultiThreadMultiSet)
{
	AutoResetEvent myEvent{ false };

	std::optional<bool> firstThreadResult = std::nullopt;
	std::optional<bool> secondThreadResult = std::nullopt;

	auto firstThread = std::thread([myEvent, &firstThreadResult]()
	{
		if (myEvent.wait(std::chrono::duration<long, std::ratio<1, 1000>>{100}))
		{
			firstThreadResult = true;
		}
		std::cout << "First thread exiting.\r\n";
	});
	
	auto secondThread = std::thread([myEvent, &secondThreadResult]()
	{
		if (myEvent.wait(100ms))
		{
			secondThreadResult = true;
		}
		std::cout << "Second thread exiting.\r\n";
	});

	std::this_thread::sleep_for(10ms);

	ASSERT_EQ(std::nullopt, firstThreadResult);
	ASSERT_EQ(std::nullopt, secondThreadResult);

	myEvent.set();
	myEvent.set();
	
	firstThread.join();
	secondThread.join();

	ASSERT_EQ(true, firstThreadResult.has_value());
	ASSERT_EQ(true, secondThreadResult.has_value());

	ASSERT_EQ(true, firstThreadResult.value());
	ASSERT_EQ(true, secondThreadResult.value());

	

}

TEST(AutoResetEventTests, MultiThreadNoTimeout)
{
	AutoResetEvent myEvent{ false };

	std::optional<bool> firstJoined = std::nullopt;
	std::optional<bool> secondJoined = std::nullopt;

	auto firstThread = std::thread([&myEvent, &firstJoined]()
	{
		myEvent.wait();
		firstJoined = true;
		std::cout << "First thread exiting.\r\n";
	});

	auto secondThread = std::thread([&myEvent, &secondJoined]()
	{
		myEvent.wait();
		secondJoined = true;
		std::cout << "Second thread exiting.\r\n";
	});

	myEvent.set();

	std::this_thread::sleep_for(10ms);

	std::thread* ongoingThread = nullptr;
	if (!firstJoined.has_value())
		ongoingThread = &firstThread;
	else if (!secondJoined.has_value())
		ongoingThread = &secondThread;

	ASSERT_FALSE(nullptr == ongoingThread);

	std::this_thread::sleep_for(100ms);
	ASSERT_FALSE(firstJoined.has_value() && secondJoined.has_value());

	myEvent.set();
	std::this_thread::sleep_for(10ms);

	ASSERT_EQ(true, firstJoined.has_value());
	ASSERT_EQ(true, secondJoined.has_value());

	firstThread.join();
	secondThread.join();

}

