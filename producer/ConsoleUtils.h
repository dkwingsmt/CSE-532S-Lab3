#pragma once
#include <thread>
#include <string>
#define PLAY_COMMAND "PLAY"
#define STOP_COMMAND "STOP"
using namespace std;

enum ConsoleCommand {
	C_PLAY = 0,
	C_STOP,
	C_EXIT,
	C_INVALID
};

class ConsoleUtils
{
	thread internalThread;
	
	static ConsoleUtils *sharedInstance;

	ConsoleUtils(void){}

	void start();
	void doMenu(bool &);
	pair<ConsoleCommand, int> parseCommand(string command);

public:

	void activate();

	static ConsoleUtils* getInstance();
	static void tearDown();
};

