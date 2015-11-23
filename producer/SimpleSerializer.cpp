#include <regex>
#include "SimpleSerializer.h"
using namespace std;

string SimpleSerializer::serialize(const ServerMessage &message) {
	return message.type + "#" + message.playName;
}

string SimpleSerializer::serialize(const ClientMessage &message) {

}

ClientMessage SimpleSerializer::inflateClient(const string clientMessage){

}

ServerMessage SimpleSerializer::inflateServer(const string serverMessage) {
	regex serverMessageRegex("(\\d+)#([A-Za-z0-9-_\\s+\\t+]+)");	
	smatch match;	
	
}