#include "common.h"
#include "ace\Connector.h"  
#include "ace\Reactor.h" 
#include "ace\Svc_Handler.h"
#include "ace\SOCK_Stream.h"
#include "ace\SOCK_Connector.h" 
#include "ace\SOCK_Acceptor.h"  
#include "ace\Synch.h"  
#include "ace\Acceptor.h" 
#include "Director.h"
#include "SignalHandler.h"
using namespace std;

#ifndef __MESSAGEHANDLER__
#define __MESSAGEHANDLER__

enum ClientMessageType {
	REGISTER = 0,
	AVAILABLE,
	NULL_MSG
};

enum ServerMessageType {
	START = 0,
	STOP,
	POISON
};

class MessageHandler  :public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{ 
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;
	size_t currentPlayId;
public: 
	int open(void *p);
    int handle_input(ACE_HANDLE);
 

	//Sends available scripts as a menu to Producer. 
	void sendScriptFile();
	MessageHandler (){};
	MessageHandler(Director* director);

	void sendFeedBack(ClientMessageType msgType);
	void processMessage( char* msg_buffer);

private:
	Director* myDirector;
	static const int MAX_BUFFER_SIZE = 1024;
};

 


#endif