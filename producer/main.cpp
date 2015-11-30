#include <iostream>
#include <string>
#include "ace\INET_Addr.h"
#include "common.h"
#include "Comms.h"
#include "DirectorRegister.h"
#include "ConsoleUtils.h"
#include "stdi.h"
#include "TestHandler.h"

using namespace std;

int checkArguments(int, char* []);

int main(int argc, char* argv[]) {

	int port_number = checkArguments(argc, argv);

	if(!Comms::init(ACE_INET_Addr(port_number, "0.0.0.0"))){
		cout << "Could not open on port " << port_number;
		return 99;
	} 

	TGuard applicationShutdownGuard([](){
		Comms::shutdown();
		DirectorRegister::tearDown();
		ConsoleUtils::tearDown();
		ACE_Reactor::instance()->close_singleton();
	});	

	cout << "Waiting on " << port_number << endl;

	//CODE HERE
	int index = 0;
	TestHandler t1([&index](){

		DirectorRegister::getInstance()->beginPlay(1,index++ % 2);

	}, 7, 40);

	ACE_Reactor::instance()->run_reactor_event_loop();	

	return 0;
}

int checkArguments(int argc, char *argv[]) {

	if(argc != 2) {
		cerr << "usage: " << argv[0] << "[port]" << endl;
		terminate();
	}

	try {
		return stoi(string(argv[1]));
	} catch(...) {
		cerr << "Port number has to be a number" << endl;
		terminate();
	}

}