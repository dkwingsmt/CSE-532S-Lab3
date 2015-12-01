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
		char msg_buffer[MAX_BUFFER_SIZE];  
       // memset(msg_buffer,0,BUF_LEN);
 
		if (  peer().recv(msg_buffer,MAX_BUFFER_SIZE )>0 )
		{

			//cout << string(msg_buffer) <<endl;
			processMessage(msg_buffer);
 
		}
		else
		{
			cout<<"no data received"<<endl;
		};  
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("%s/n"),msg_buffer)); 
        return 0;  
    }  

	int handle_output(ACE_HANDLE)
	{
		return 0;
	}

	//Sends available scripts as a menu to Producer. 
	void sendScriptFile()
	{
		string bufferStr = to_string(REGISTER) ;
		vector<std::string> scripts = myDirector->getScriptsFileName();
		for (auto script : scripts)
		{
			int pos = script.find_last_of("\\");
			bufferStr += ("#"+script.substr(pos+1) );
		}
		peer().send(bufferStr.c_str(), bufferStr.length());
	}
	 
 
	MessageHandler (){};

	MessageHandler(Director* director)
	{
		//myMessage = make_shared<message>(message());
		myDirector =  director;
		//Everytime the act ends, it sends a feedback to the producer that this director is available. 
		//This line causes a crash on producer side. 
		myDirector->onActEnd([this]{ sendFeedBack(AVAILABLE); });
	};

	~MessageHandler()
	{
		 		cout << "Killed ClientHandler instance: " << this << endl << endl;
	}

	void sendFeedBack(ClientMessageType msgType)
	{
		string str = to_string(msgType);
		peer().send(str.c_str(),str.length() );
		return;
	}
 
	void processMessage( char* msg_buffer)
	{
 
		string  msg_str = string(msg_buffer);
		istringstream iss(msg_str);
		std::istream_iterator<std::string> beg(iss), end;
		vector<string> messageVec(beg, end);
		int msgType = stoi(msg_str.substr(0, msg_str.find_first_of("#")));

		if (msgType == START)
		{
			//TODO: Start to play the correspondent play, which is indicated in 
			//the next string in messageVec
			string scriptNum = msg_str.substr( msg_str.find_first_of("#")+1, 1);
			cout << scriptNum <<endl;
			cout<<msgType<<" We got a start here" <<endl;
			myDirector->selectScript(stoi(scriptNum));
			//sendScriptFileAndStatus();
		}
		else if (msgType == STOP)
		{
			//TODO: Stop the current play and send feedback to producer
			cout<<msgType<<" We got a stop here" <<endl;
			myDirector->stopNowScript();
			//sendScriptFileAndStatus();
		}
		else if (msgType== POISON)
		{
			//TODO: Quit the director program safely, and send feedback to producer.
			sendFeedBack(NULL_MSG); 
			//Remove the handler and then end event loop.
			//Not sure if this is correct way to do it. 
			ACE_Reactor::instance()->remove_handler(this, ACE_Event_Handler::READ_MASK); 
			ACE_Reactor::instance()->end_event_loop();

		}
		return;
	}

 
private:
	Director* myDirector;
	mutex myMutex;

	static const int MAX_BUFFER_SIZE = 3;


};

 


#endif