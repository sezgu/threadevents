#include "pch.h"
#include "Signal.h"


using namespace std::literals::chrono_literals;
using namespace threading;

TEST(SignalTests, SingleThreadTrivialType)
{
	Signal<int> signal{};

	int value{ -1 };

	bool result = signal.wait(10ms,value);
	ASSERT_FALSE(result);
	ASSERT_EQ(-1, value);

	signal.set(5);

	result = signal.wait(10000ms,value);
	ASSERT_TRUE(result);
	ASSERT_EQ(5, value);

	value = -1;
	result = signal.wait(10ms,value);
	ASSERT_FALSE(result);
	ASSERT_EQ(-1, value);

	signal.set(6);
	ASSERT_EQ(6,signal.wait());
}

TEST(SignalTests, SingleThreadNonCopyableType)
{
	Signal<std::unique_ptr<int>> signal{};

	std::unique_ptr<int> value{ std::make_unique<int>(-1) };

	bool result = signal.wait(10ms, value);
	ASSERT_FALSE(result);
	ASSERT_EQ(-1, *value);

	signal.set(std::make_unique<int>(5));

	result = signal.wait(10000ms, value);
	ASSERT_TRUE(result);
	ASSERT_EQ(5, *value);

	*value = -1;
	result = signal.wait(10ms, value);
	ASSERT_FALSE(result);
	ASSERT_EQ(-1, *value);

	signal.set(std::make_unique<int>(6));
	ASSERT_EQ(6, *signal.wait());

}