#ifndef __SIGNALHANDLER__
#define __SIGNALHANDLER__

#include "ace/Reactor.h"
#include "ace/Event_Handler.h"
#include <iostream>

 

class SignalHandler : public ACE_Event_Handler  
{  

public:   
  SignalHandler(){}  

  int handle_signal(int signum, siginfo_t*,ucontext_t*)  
  {
    switch(signum)
    {
		case SIGINT: std::cout<<"You pressed SIGINT(Ctrl+C)\n";
		//
		ACE_Reactor::instance()->end_event_loop();
		return 0;
    }
    return 0;
  }

};  
#endif


