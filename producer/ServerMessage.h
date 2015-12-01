#pragma once
#include <string>
using namespace std;
enum ServerMessageType {
	START = 0,
	STOP,
	POISON
};

struct ServerMessage {
	ServerMessageType type;	
	int playId;
	string playName;

	ServerMessage() {}
	ServerMessage(ServerMessageType type) : type(type) {}
	ServerMessage(ServerMessageType type, int playId) : type(type), playId(playId) {}

};