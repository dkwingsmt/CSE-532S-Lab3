#pragma once
#include "iserializer.h"
class SimpleSerializer :
	public ISerializer
{
public:
	virtual string serialize(const Message &message);
	virtual ClientMessage inflate(const string clientMessage);
};

