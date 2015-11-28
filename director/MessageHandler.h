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
#include "Director.h"
#include "SignalHandler.h"
using namespace std;

#ifndef __MESSAGEHANDLER__
#define __MESSAGEHANDLER__

#define BUF_LEN 60 
#define STOP "stop"
#define QUIT "quit"
#define PLAY "play"
#define WAITIGN "waiting"
 


struct message
{
	string action;
	message(){};
	message(char* msg_buffer)
	{
		action = string(msg_buffer);
	};

};



class MessageHandler 
	:public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;
public:
	 
	int open(void *p)
	{
		ACE_DEBUG((LM_DEBUG,ACE_TEXT("connection established/n")));  
        ACE_Reactor::instance()->register_handler(this,ACE_Event_Handler::READ_MASK);  
		ACE_Reactor::instance()->register_handler(SIGINT, new SignalHandler);
		return 0;  
	}

    int handle_input(ACE_HANDLE)  
	{  
		cout << "inside handle_input"<<endl;
		char msg_buffer[100];  
		cout << "inside handle_input 2"<<endl;
       // memset(msg_buffer,0,BUF_LEN);  
		if (  peer().recv(msg_buffer,MAX_BUFFER_SIZE )>0 )
		{
			cout << string(msg_buffer) <<endl;

		}
		else
		{
			cout<<"no data received"<<endl;
		};  
		cout << "inside handle_input"<<endl;
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("%s/n"),msg_buffer)); 
		//Parse input as message
		processMessage(msg_buffer);

        return 0;  
    }  

	int handle_output(ACE_HANDLE)
	{
		return 0;
	}


	void sendScriptFiles(vector<string>& scriptFiles)
	{
		const int buffer_size = 256;
		for(auto script: scriptFiles)
		{
			peer().send_n( script.c_str(), buffer_size);
		}
	}

 
	MessageHandler (){};

	MessageHandler(Director* director)
	{
		myMessage = make_shared<message>(message());
		myDirector =  director;
	};

	~MessageHandler()
	{
		cout << "Killed ClientHandler instance: " << this << endl << endl;
	}

 
	void processMessage( const char* msg_buffer)
	{
		myMessage->action = string(msg_buffer);

		if (myMessage->action == PLAY)
		{
			myDirector->selectScript(0);
		}
		else if (myMessage->action == STOP)
		{
			myDirector->stopNowScript();
			peer().send_n("Play stopped", 20);
		}
		else if (myMessage->action == QUIT)
		{
			return ;
		}
	}

 
private:
	Director* myDirector;
	shared_ptr<message> myMessage;
	mutex myMutex;
	static const int MAX_BUFFER_SIZE = 100;


};

 


#endif