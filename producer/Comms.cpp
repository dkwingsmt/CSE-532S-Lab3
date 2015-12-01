#include "Comms.h"
#include "ace\Reactor.h"
#include "stdi.h"
bool Comms::init(ACE_INET_Addr address) {
	cHand = new ConnectionHandler();	

	if(cHand->open(address) == -1)
		return false;

	ACE_Reactor::instance()->register_handler(cHand, ACE_Event_Handler::READ_MASK);

	return true;
}

void Comms::shutdown() {
	if(cHand)
		delete cHand;
}

ConnectionHandler *Comms::cHand;