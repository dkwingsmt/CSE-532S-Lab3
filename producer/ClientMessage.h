#pragma once
enum ClientMessageType {
	REGISTER = 0,
	AVAILABLE,
	NULL_MSG
};

struct ClientMessage {
	ClientMessageType type;	
	vector<string> playList;

	ClientMessage() {};
	ClientMessage(ClientMessageType type) : type(type) {}
	ClientMessage(ClientMessageType type, vector<string> playList) : type(type), playList(move(playList)) {}
};

