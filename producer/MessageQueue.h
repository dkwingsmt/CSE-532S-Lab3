#pragma once
#include <queue>
#include <mutex>
#include "stdi.h"
#include "ServerMessage.h"

using namespace std;

class MessageQueue
{
private:
	mutex queue_lock;
	queue<ServerMessage> messages;

public:
	MessageQueue(){}
	MessageQueue(const MessageQueue&);
	MessageQueue operator=(const MessageQueue&);

	void push_message(ServerMessage&& message);

	bool pop_message(ServerMessage& message);

};

