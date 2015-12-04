#include "ConsoleUtils.h"
#include <iostream>
#include "DirectorRegister.h"
#include "ConsoleLocker.h"
#include <regex>
#include <sstream>
#define CONSOLE_INPUT_SIZE 256

ConsoleUtils* ConsoleUtils::sharedInstance;

void ConsoleUtils::activate() {
	internalThread = move(thread(&ConsoleUtils::start, this));
}

void ConsoleUtils::start() {
	do {
		doMenu();
		CL_OUT(cout << endl << endl);
	} while(!shutdown);
	CL_OUT(cout << "Exiting, please wait..." << endl);
}

void ConsoleUtils::doMenu() {
	map<int, vector<string>> directorIdToPlayListMap;
	map<int, int> directorIdToPlayNumberBusy;
	DirectorRegister::getInstance()->getPlayDetails(directorIdToPlayListMap, directorIdToPlayNumberBusy);
	
	map<int, pair<int, int>> consoleNumberToDirectorIdAndPlayNumber;

	int consoleItemIndex = 0;
	for_each(directorIdToPlayListMap.begin(), directorIdToPlayListMap.end(), [&consoleItemIndex, &consoleNumberToDirectorIdAndPlayNumber, &directorIdToPlayNumberBusy]
	(pair<int, vector<string>> p){
		int currentDirector = p.first;		
		int currentPlayNumber = 0;

		for_each(p.second.begin(), p.second.end(), [&currentDirector, &currentPlayNumber, &consoleItemIndex, &consoleNumberToDirectorIdAndPlayNumber, &directorIdToPlayNumberBusy](string playName){
			consoleNumberToDirectorIdAndPlayNumber[++consoleItemIndex] = pair<int, int>(currentDirector, currentPlayNumber);
			CL_OUT(cout << consoleItemIndex << ". Director #" << currentDirector << "'s " << playName << (directorIdToPlayNumberBusy[currentDirector] == currentPlayNumber ? "(busy)" : "") << endl);
			currentPlayNumber++;
		});

	});

	CL_OUT(cout << "Please type a command (start, stop, exit) followed by an option (if required)" << endl);

	char inputBuffer[CONSOLE_INPUT_SIZE];
	cin.getline(inputBuffer, CONSOLE_INPUT_SIZE);

	string currentCommand(inputBuffer);

	//Might trigger if SIGINT is initiated
	if(cin.fail()) { 
		shutdown = true;
		return; 
	}

	if(currentCommand.empty()) {
		CL_OUT(cout << "Refreshing..." << endl);
		return;
	}

	auto commandOptionPair = parseCommand(currentCommand);

	if(commandOptionPair.first == C_INVALID) {
		CL_OUT(cout << "Command not recognized, please try again" << endl);
		return;
	}

	int director =-1, playNumber = -1;
	if(commandOptionPair.second > 0) {
		auto directorPlayNumberIt = consoleNumberToDirectorIdAndPlayNumber.find(commandOptionPair.second);
		if(directorPlayNumberIt == consoleNumberToDirectorIdAndPlayNumber.end()) {
			CL_OUT(cout << "Invalid option " << commandOptionPair.second << ", please try again" << endl);
			return;
		}
		director = directorPlayNumberIt->second.first;
		playNumber = directorPlayNumberIt->second.second;
	}

	processCommand(commandOptionPair.first, director, playNumber);

}

void ConsoleUtils::processCommand(ConsoleCommand command, int  director, int play) {
	switch (command)
	{
	case C_PLAY:
		if(director<0 || play<0) {
			CL_OUT(cout << "Play requires an option" << endl);
			return;
		}
		DirectorRegister::getInstance()->beginPlay(director, play);
		break;
	case C_STOP:
		if(director<0 || play<0) {
			CL_OUT(cout << "Stop requires an option" << endl);
			return;
		}
		DirectorRegister::getInstance()->stopDirector(director);
		break;
	case C_EXIT:
		shutdown = true;
		DirectorRegister::getInstance()->exit();
		break;
	}
}

pair<ConsoleCommand, int> ConsoleUtils::parseCommand(string command) {
	regex commandRegex("\\s*(START|STOP|EXIT)(\\s+(\\d+))?", regex_constants::ECMAScript | regex_constants::icase);
	smatch match;
	if(regex_match(command, match, commandRegex) && match.size() > 0) {
		ConsoleCommand targetCommand = C_EXIT;
		string command = match.str(1);
		stringstream ss;
		for(char & c : command)
		{
			ss<<(char)tolower(c);
		}

		if(ss.str().compare(PLAY_COMMAND) == 0)
			targetCommand = ConsoleCommand::C_PLAY;	
		else if (ss.str().compare(STOP_COMMAND) == 0 )
			targetCommand = ConsoleCommand::C_STOP;

		return pair<ConsoleCommand, int>(targetCommand, targetCommand != C_EXIT && !match.str(2).empty()? stoi(match.str(2)) : -1);

	} else {
		return pair<ConsoleCommand, int>(C_INVALID, -1);
	}

}

ConsoleUtils* ConsoleUtils::getInstance() {
	if(sharedInstance == nullptr)
		sharedInstance = new ConsoleUtils();

	return sharedInstance;
}

void ConsoleUtils::tearDown() {
	if(sharedInstance->internalThread.joinable()) 
		sharedInstance->internalThread.join();

	if(sharedInstance != nullptr) 
		delete sharedInstance;
}