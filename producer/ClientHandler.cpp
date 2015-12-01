#include "ClientHandler.h"
#include <iostream>
#include "stdi.h"
#include "DirectorRegister.h"
#include "Comms.h"
using namespace std;

int ClientHandler::open(void *acceptor_or_connector) {
	if(reactor() && reactor()->register_handler(this, ACE_Event_Handler::READ_MASK | ACE_Event_Handler::WRITE_MASK) == -1) {
		cout << "Error in connection" << endl;
		return -1;
	}

	cout << "Connection established" << endl;
	return 0;
}

int ClientHandler::handle_output(ACE_HANDLE) {

	ServerMessage message;
	if(!messageQueue.pop_message(message))
		return 0;

	return processMessage(message);

}

int ClientHandler::handle_input(ACE_HANDLE) {
	ClientMessage message = getClientMessage();
	return processClientMessage(message);
}

ClientMessage ClientHandler::getClientMessage() {
	char buf[MAX_BUFFER_SIZE];
	int bytesReceived;

	ClientMessage m;

	if( (bytesReceived = this->peer_.recv(buf, sizeof(buf) - 1)) < 1) {
		m.type = NULL_MSG;
	} else {
		m = serializer.currentSerializer()->inflateClient(string(buf));
	}

	return move(m);
}

int ClientHandler::processClientMessage(const ClientMessage& message) {
	switch(message.type) {
	case NULL_MSG:
		shutdown();
		return -1;

	case REGISTER:
		currentDirectorId = DirectorRegister::getInstance()->addDirector(this, message.playList);
		break;

	case AVAILABLE:
		DirectorRegister::getInstance()->freeDirector(currentDirectorId);
	}

	return 0;
}

int ClientHandler::processMessage(const ServerMessage& message) {

	string serializedMessage = move(serializer.currentSerializer()->serialize(message));
	cout << serializedMessage << endl;
	cout << serializedMessage.c_str() << endl;
	peer().send(serializedMessage.c_str(), serializedMessage.length());

	if(message.type == POISON) {
		shutdown();
		return -1;
	}

	return 1;
}

void ClientHandler::shutdown() {
	DirectorRegister::getInstance()->removeDirector(currentDirectorId);
}

void ClientHandler::postMessage(ServerMessage message) {
	messageQueue.push_message(move(message));
	reactor()->notify(this, ACE_Event_Handler::WRITE_MASK);
}