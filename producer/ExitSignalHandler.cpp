#include "ExitSignalHandler.h"
#include "DirectorRegister.h"
#include "ConsoleLocker.h"
#include <iostream>
using namespace std;

int ExitSignalHandler::handle_signal(int signum, siginfo_t* = 0, ucontext_t* = 0) {
	if(signum == SIGINT) {
		DirectorRegister::getInstance()->exit();	
		return -1;
	}

	return 0;
}
