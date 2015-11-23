#pragma once
#include "iserializer.h"
class SimpleSerializer :
	public ISerializer
{
public:
	virtual string serialize(const ServerMessage &message);
	virtual string serialize(const ClientMessage &message);
	virtual ClientMessage inflateClient(const string clientMessage);
	virtual ServerMessage inflateServer(const string serverMessage);
};

