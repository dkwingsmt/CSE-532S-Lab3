#pragma once
#include <mutex>
#define CL_OUT(x) ConsoleLocker::getInstance()->execute([&](){x;})
using namespace std;
class ConsoleLocker
{
	//recursive_mutex is required as opposed to just a mutex because outside code
	//might deadlock the application be calling execute over and over
	recursive_mutex commonLock;
	static ConsoleLocker *sharedInstance;	
	static once_flag instanceInitialized;

	static void init();

	ConsoleLocker(){}

public:
	static ConsoleLocker *getInstance();
	static void tearDown();

	void execute(function<void(void)> consoleFunction);

};

