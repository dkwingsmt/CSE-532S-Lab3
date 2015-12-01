#pragma once
#include "ace\Event_handler.h"
#include <functional>
using namespace std;
class TestHandler :
	public ACE_Event_Handler
{
	function<void(void)> timeoutFunction;

public:
	TestHandler(function<void(void)> f, int initialDelay, int interval);

	virtual int handle_timeout(const ACE_Time_Value&, const void*) { timeoutFunction(); return 1; }
};

