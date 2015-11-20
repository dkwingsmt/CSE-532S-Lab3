#pragma once
#include "ace\Svc_Handler.h"
#include "ace\SOCK_Stream.h"
#include "MessageQueue.h"
#include "stdi.h"
#include "Serializers.h"

typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;

class ClientHandler : public super
{
	MessageQueue messageQueue;
	DefaultSerializer serializer;

	void shutdown();
	int processMessage(const Message& message);
	int processClientMessage(const ClientMessage& message);
	ClientMessage getClientMessage();

public:
	ClientHandler(void);
	~ClientHandler(void);

	virtual int open(void *acceptor_or_connector = 0);
	virtual int handle_input(ACE_HANDLE);
	virtual int handle_output(ACE_HANDLE);

	void postMessage(Message message);
};

