#include <regex>
#include <sstream>
#include "SimpleSerializer.h"
using namespace std;
#define DELIMITER "#"

string SimpleSerializer::serialize(const ServerMessage &message) {
	stringstream ss;
	ss << message.type << "#" << message.playId;
	return ss.str();
}

string SimpleSerializer::serialize(const ClientMessage &message) {
	stringstream ss;	
	ss << message.type;
	if(message.type == REGISTER) {
		for_each(message.playList.begin(), message.playList.end(), [&ss](string playName){
			ss  << "#" << playName;
		});
	}

	return ss.str();
}

ClientMessage SimpleSerializer::inflateClient(const string clientMessage){
	regex clientMessageRegex("(\\d+)([#A-Za-z-_ 0-9.]+)?");
	smatch match;
	if(regex_search(clientMessage, match, clientMessageRegex) && match.size() > 0){
		ClientMessageType currentMessageType = static_cast<ClientMessageType>(stoi(match.str(1)));	
		if(currentMessageType == AVAILABLE){
			return ClientMessage(currentMessageType);
		} else {
			vector<string> playList;
			string playListString = match.str(2);	
			int lastBegin = 1;
			for(unsigned int i=1; i < playListString.size(); ++i) {
				if(playListString.compare(i, 1, DELIMITER) == 0) {
					playList.push_back(playListString.substr(lastBegin, i - lastBegin));
					lastBegin = i+1;
				}
			}
			playList.push_back(playListString.substr(lastBegin));

			return ClientMessage(currentMessageType, move(playList));
		}
	} else {
		throw exception("Invalid message received from client !");
	}
}

ServerMessage SimpleSerializer::inflateServer(const string serverMessage) {
	regex serverMessageRegex("(\\d+)(#(\\d+))?");	
	smatch match;	
	if(regex_search(serverMessage, match, serverMessageRegex) && match.size() > 0) {
		return match.size() == 3 ? 
			ServerMessage(static_cast<ServerMessageType>(stoi(match.str(1))), stoi(match.str(3))) : 
			ServerMessage(static_cast<ServerMessageType>(stoi(match.str(1))));
	} else {
		throw std::exception("Invalid message received from server !");
	}

}