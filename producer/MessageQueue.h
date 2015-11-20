#pragma once
#include <queue>
#include <mutex>
#include "stdi.h"

using namespace std;
enum MessageType {
	POISON = 0,
	START,
	STOP,
};

struct Message {
	MessageType type;
	int extra;
};

class MessageQueue
{
private:
	mutex queue_lock;
	queue<Message> messages;

public:
	MessageQueue(){}
	MessageQueue(const MessageQueue&);
	MessageQueue operator=(const MessageQueue&);

	void push_message(Message&& message);

	bool pop_message(Message& message);

};

