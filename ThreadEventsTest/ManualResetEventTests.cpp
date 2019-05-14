#include "pch.h"
#include "ThreadEvents.h"

using namespace std::literals::chrono_literals;
using namespace threading;

TEST(ManualResetEventTests, SingleThreadSet)
{
	ManualResetEvent myEvent{ true };

	bool result = myEvent.wait(10ms);
	ASSERT_TRUE(result);

	myEvent.wait();
	myEvent.reset();

	result = myEvent.wait(10ms);
	ASSERT_FALSE(result);

	myEvent.set();
	myEvent.wait();
}

TEST(ManualResetEventTests, MultiThreadSingleSet)
{
	ManualResetEvent myEvent{ false };

	std::optional<bool> firstThreadResult = std::nullopt;
	std::optional<bool> secondThreadResult = std::nullopt;

	auto firstThread = std::thread([&myEvent, &firstThreadResult]()
	{
		std::this_thread::sleep_for(10ms);
		myEvent.wait();
		firstThreadResult = true;
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

	ASSERT_EQ(std::nullopt, firstThreadResult);
	ASSERT_EQ(std::nullopt, secondThreadResult);

	myEvent.set();

	firstThread.join();
	secondThread.join();

	ASSERT_TRUE(firstThreadResult.has_value() && secondThreadResult.has_value());

	ASSERT_TRUE(firstThreadResult.value());
	ASSERT_TRUE(secondThreadResult.value());

}

TEST(ManualResetEventTests, MultiThreadSetReset)
{
	ManualResetEvent myEvent{ true };

	std::optional<bool> firstThreadResult = std::nullopt;
	auto firstThread = std::thread([myEvent, &firstThreadResult]()
	{
		if (myEvent.wait(std::chrono::duration<long, std::ratio<1, 1000>>{100}))
		{
			firstThreadResult = true;
		}
		std::cout << "First thread exiting.\r\n";
	});
	
	std::optional<bool> secondThreadResult = std::nullopt;
	auto secondThread = std::thread([myEvent, &secondThreadResult]()
	{
		std::this_thread::sleep_for(100ms);
		myEvent.wait();
		secondThreadResult = true;
		std::cout << "Second thread exiting.\r\n";
	});

	std::this_thread::sleep_for(10ms);

	ASSERT_EQ(std::nullopt, secondThreadResult);

	myEvent.set();
	firstThread.join();
	myEvent.reset();
	
	ASSERT_EQ(true, firstThreadResult.has_value());
	ASSERT_EQ(true, firstThreadResult.value());

	std::this_thread::sleep_for(100ms);

	ASSERT_EQ(std::nullopt, secondThreadResult);

	myEvent.set();
	secondThread.join();

	ASSERT_EQ(true, secondThreadResult.has_value());
	ASSERT_EQ(true, secondThreadResult.value());



}


