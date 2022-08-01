#include "TimedCall.h"

TimedCall::TimedCall(std::function<void(void)> f, uint32_t time) :func(f), time_(time)
{
}

void TimedCall::Update()
{
	if (finishedFlag_)
	{
		return;
	}
	time_--;
	if (time_ <= 0)
	{
		finishedFlag_ = true;
		//コールバック関数呼び出し
		func();
	}
}