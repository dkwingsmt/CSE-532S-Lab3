#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <algorithm>
#include <iterator>
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
#define START "start"
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

	//Sends available scripts as a menu to Producer. 
	void sendScriptFileAndStatus()
	{
		string bufferStr = "AvailableScripts";
		vector<std::string> scripts = myDirector->getScriptsAndStatus();
		bufferStr += accumulate(scripts.begin() , scripts.end(), string(""));
		peer().send(bufferStr.c_str(), bufferStr.length());
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

 
	void processMessage( char* msg_buffer)
	{
 
		string  str = string(msg_buffer);
		istringstream iss(str);
		std::istream_iterator<std::string> beg(iss), end;
		vector<string> messageVec(beg, end);
		if (messageVec[0] == START)
		{
			//TODO: Start to play the correspondent play, which is indicated in 
			//the next string in messageVec
			myDirector->selectScript(stoi(messageVec[1]));
			sendScriptFileAndStatus();
		}
		else if (messageVec[0] == STOP)
		{
			//TODO: Stop the current play and send feedback to producer
			myDirector->stopNowScript();
			sendScriptFileAndStatus();
		}
		else if (messageVec[0] == QUIT)
		{
			//TODO: Quit the director program safely, and send feedback to producer.
		}
		return;
	}

 
private:
	Director* myDirector;
	shared_ptr<message> myMessage;
	mutex myMutex;

	static const int MAX_BUFFER_SIZE = 100;


};

 


#endif