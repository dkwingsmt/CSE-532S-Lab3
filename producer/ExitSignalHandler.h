#pragma once
#include "ace\Event_Handler.h"
class ExitSignalHandler :
	public ACE_Event_Handler
{
public:
	virtual int handle_signal(int signum, siginfo_t*, ucontext_t *);
};

