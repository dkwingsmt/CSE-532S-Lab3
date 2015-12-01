#pragma once
#include "ace\INET_Addr.h"
#include "ace\Acceptor.h"
#include "ace\SOCK_Acceptor.h"
#include "ClientHandler.h"
#include "stdi.h"
typedef ACE_Acceptor<ClientHandler, ACE_SOCK_ACCEPTOR> ConnectionHandler;

class Comms
{
private:
	static ConnectionHandler *cHand;

public:
	static bool init(ACE_INET_Addr address);
	static void shutdown();
};

