#include "ClientHandler.h"
#include <iostream>
#include "stdi.h"
#include "DirectorRegistry.h"
using namespace std;

ClientHandler::ClientHandler(void)
{
}


ClientHandler::~ClientHandler(void)
{
}

int ClientHandler::open(void *acceptor_or_connector) {
	if(super::open(acceptor_or_connector) == -1) {
		cout << "Error" << endl;
		return -1;
	}
	cout << "Connection established" << endl;
	return 0;
}

int ClientHandler::handle_output(ACE_HANDLE) {

	Message message;
	if(!messageQueue.pop_message(message))
		return 0;

	return processMessage(message);

}

int ClientHandler::handle_input(ACE_HANDLE) {
	ClientMessage message = getClientMessage();
	return processClientMessage(message);
}

ClientMessage ClientHandler::getClientMessage() {
	char buf[2048];
	int bytesReceived;

	ClientMessage m;

	if( (bytesReceived = this->peer_.recv(buf, sizeof(buf) - 1)) < 1) {
		m.type = NULL_MSG;
	} else {
		m = serializer.currentSerializer()->inflate(string(buf));
	}

	return move(m);
}

int ClientHandler::processClientMessage(const ClientMessage& message) {
	switch(message.type) {
	case NULL_MSG:
		shutdown();
		return -1;

	case REGISTER:
		GlobalRegistry::instance()->add_director(this, message.playList);
		break;

	case AVAILABLE:
		GlobalRegistry::instance()->make_free(this);
	}

	return 0;
}

int ClientHandler::processMessage(const Message& message) {

	string serializedMessage = move(serializer.currentSerializer()->serialize(message));
	peer().send(serializedMessage.c_str(), serializedMessage.length());

	if(message.type == POISON) {
		shutdown();
		return -1;
	}

	return 0;
}

void ClientHandler::shutdown() {
	
	//TODO: Any shutdown stuff that's required

}

void ClientHandler::postMessage(Message message) {
	messageQueue.push_message(move(message));
}