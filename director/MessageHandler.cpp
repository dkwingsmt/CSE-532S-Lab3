#include "common.h"
#include "MessageHandler.h"
using namespace std;
 
 
int MessageHandler::open(void *p)
{
	ACE_DEBUG((LM_DEBUG,ACE_TEXT("connection established/n")));  
    ACE_Reactor::instance()->register_handler(this,ACE_Event_Handler::READ_MASK);  
	ACE_Reactor::instance()->register_handler(SIGINT, new SignalHandler);
	return 0;  
}

int MessageHandler::handle_input(ACE_HANDLE)  
{  
	char msg_buffer[MAX_BUFFER_SIZE];  
	if (  peer().recv(msg_buffer,MAX_BUFFER_SIZE )>0 )
		processMessage(msg_buffer);
	else
	{
		myDirector->stopNowScript();
		ACE_Reactor::instance()->remove_handler(this, ACE_Event_Handler::READ_MASK); 
		ACE_Reactor::instance()->end_event_loop();
	}

    return 0;  
}  
 

//Sends available scripts as a menu to Producer, and "#" is the delimiter
void MessageHandler::sendScriptFile()
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
	 
 

MessageHandler::MessageHandler(Director* director)
{
	myDirector =  director;
	//Everytime the act ends, it sends a feedback to the producer that this director is available. 
	myDirector->onActEnd([this]{ sendFeedBack(AVAILABLE); });
};

 

void MessageHandler::sendFeedBack(ClientMessageType msgType)
{
	string str = to_string(msgType);
	if(currentPlayId == myDirector->getCurrentPlayId()) {
		peer().send(str.c_str(),str.length() );
		currentPlayId = -1;
	}

	myDirector->resetPlayId();

	return;
}
 
void MessageHandler::processMessage( char* msg_buffer)
{
 
	string  msg_str = string(msg_buffer);
	int msgType = stoi(msg_str.substr(0, msg_str.find_first_of("#")));

	if (msgType == START)
	{
		//TODO: Start to play the correspondent play, which is indicated in 
		string scriptNum = msg_str.substr( msg_str.find_first_of("#")+1);
		currentPlayId = stoi(scriptNum);
		myDirector->selectScript(currentPlayId);

	}
	else if (msgType == STOP)
	{
		//TODO: Stop the current play and send feedback to producer
		myDirector->stopNowScript();
	}
	else if (msgType== POISON)
	{
		//TODO: Quit the director program safely, and send feedback to producer.
		//Remove the handler and then end event loop. 
		myDirector->stopNowScript();
		ACE_Reactor::instance()->remove_handler(this, ACE_Event_Handler::READ_MASK); 
		ACE_Reactor::instance()->end_event_loop();

	}
	return;
}