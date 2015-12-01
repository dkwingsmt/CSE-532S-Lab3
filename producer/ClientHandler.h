#pragma once
#include "ace\Svc_Handler.h"
#include "ace\SOCK_Stream.h"
#include "MessageQueue.h"
#include "stdi.h"
#include "Serializers.h"
#include "ServerMessage.h"
#include "AspectConfig.h"
#define MAX_BUFFER_SIZE 2048

typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;

class ClientHandler : public super
{
	int currentDirectorId;
	MessageQueue messageQueue;
	DefaultSerializer serializer;

	void shutdown();
	int processMessage(const ServerMessage& message);
	int processClientMessage(const ClientMessage& message);
	ClientMessage getClientMessage();

public:
	virtual int open(void *acceptor_or_connector);
	virtual int handle_input(ACE_HANDLE);
	virtual int handle_output(ACE_HANDLE);

	void postMessage(ServerMessage message);
};

