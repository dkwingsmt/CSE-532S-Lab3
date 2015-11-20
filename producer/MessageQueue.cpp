#include "MessageQueue.h"
#include "stdi.h"

void MessageQueue::push_message(Message&& message) {
	lock_guard<mutex> guard(queue_lock);	
	messages.push(move(message));
}

bool MessageQueue::pop_message(Message& message) {
	lock_guard<mutex> guard(queue_lock);
	if(messages.empty())
		return false;

	message = move(messages.front());
	messages.pop();
	return true;
}