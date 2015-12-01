#pragma once
#include "stdi.h"
#include <string>
#include "MessageQueue.h"
#include "ClientMessage.h"

using namespace std;
class ISerializer
{
public:
	virtual string serialize(const ServerMessage &message) = 0;
	virtual string serialize(const ClientMessage &message) = 0;
	virtual ClientMessage inflateClient(const string clientMessage) = 0;
	virtual ServerMessage inflateServer(const string serverMessage) = 0;
};