#include "TestHandler.h"
#include "ace\Reactor.h"
TestHandler::TestHandler(function<void(void)> f, int initialDelay, int interval) {
	timeoutFunction = move(f);
	ACE_Reactor::instance()->schedule_timer(this, 0, ACE_Time_Value(initialDelay), ACE_Time_Value(interval));
}