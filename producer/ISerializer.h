#pragma once
#include "stdi.h"
#include <string>
#include "MessageQueue.h"
#include "ClientMessage.h"

using namespace std;
class ISerializer
{
public:
	virtual string serialize(const Message &message) = 0;
	virtual ClientMessage inflate(const string clientMessage) = 0;		
};