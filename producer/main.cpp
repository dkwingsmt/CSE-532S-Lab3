#include <iostream>
#include <string>
#include "ace\INET_Addr.h"
#include "common.h"
#include "Comms.h"
#include "DirectorRegister.h"
#include "ConsoleUtils.h"
#include "stdi.h"
#include "TestHandler.h"
#include "ConsoleLocker.h"
#include "ExitSignalHandler.h"
#include "ErrorCodes.h"
using namespace std;

#define ARGNUM         2
#define ARGID_CMD      0
#define ARGID_PORT     1

int checkArguments(int, char* [], int&);

int main(int argc, char* argv[]) {

	int commandLineErrors = 0;
	int port_number = checkArguments(argc, argv, commandLineErrors);
	if(port_number < 0)	return commandLineErrors;


	if(!Comms::init(ACE_INET_Addr(port_number, "0.0.0.0"))){
		cout << "Could not open on port " << port_number;
		return E_PORT_NOT_AVAILABLE;
	} 


	TGuard applicationShutdownGuard([](){
		Comms::shutdown();
		DirectorRegister::tearDown();
		ConsoleUtils::tearDown();

		//Console Locker must be destroyed last
		ConsoleLocker::tearDown();
	});	

	cout << "Waiting on " << port_number << endl;

	ConsoleUtils::getInstance()->activate();

	ExitSignalHandler sigHand;
	ACE_Reactor::instance()->register_handler(SIGINT, &sigHand);

	ACE_Reactor::instance()->run_reactor_event_loop();	
	ACE_Reactor::instance()->close_singleton();

	return 0;
}

int checkArguments(int argc, char *argv[], int &errCode) {
	
	if(argc != ARGNUM) {
		cerr << "Usage: " << argv[ARGID_CMD] << "[port]" << endl;
		errCode = E_ILLEGAL_COMMAND_LINE;
		return -1;
	}

	try {
		return stoi(string(argv[ARGID_PORT]));
	} catch(...) {
		cerr << "Port number has to be a number" << endl;
		errCode = E_PORT_NOT_NUMBER;
		return -1;
	}

}
