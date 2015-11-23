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
	string playName;
};