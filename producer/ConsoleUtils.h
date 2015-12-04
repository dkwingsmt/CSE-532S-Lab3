#pragma once
#include <thread>
#include <string>
#define PLAY_COMMAND "start"
#define STOP_COMMAND "stop"

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
	bool shutdown;
	
	static ConsoleUtils *sharedInstance;

	ConsoleUtils(void) : shutdown(false){}

	void start();
	void doMenu();
	pair<ConsoleCommand, int> parseCommand(string command);
	void processCommand(ConsoleCommand command, int director, int play);
	void signalInterrupt(int sig);

public:

	void activate();

	static ConsoleUtils* getInstance();
	static void tearDown();
};

