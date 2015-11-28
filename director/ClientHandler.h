#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "ace\Connector.h"  
#include "ace\Reactor.h" 
#include "ace\Svc_Handler.h"
#include "ace\SOCK_Stream.h"
#include "ace\SOCK_Connector.h" 

#include "ace\SOCK_Acceptor.h"  
#include "ace\Synch.h"  
#include "ace\Acceptor.h" 

using namespace std;

class Agent_svc_handler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>

{

	public:

	Agent_svc_handler()
	{
		data= new char[DATA_SIZE];
	}

	int handle_input(ACE_HANDLE)
	{
		peer().recv(data, DATA_SIZE);
		ACE_DEBUG((LM_DEBUG, "%s", data));
		return 0;
	}

private:
	char* data;
	static const int DATA_SIZE = 64;

};