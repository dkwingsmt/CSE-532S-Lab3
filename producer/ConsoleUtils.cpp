#include "ConsoleUtils.h"
#include <iostream>
#include "DirectorRegister.h"
#include <regex>
ConsoleUtils* ConsoleUtils::sharedInstance;

void ConsoleUtils::activate() {
	internalThread = move(thread(&ConsoleUtils::start, this));
}

void ConsoleUtils::start() {
		
	bool shutdown = false;

	do {
		doMenu(shutdown);
	} while(!shutdown);

}

void ConsoleUtils::doMenu(bool &shutdown) {
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
			cout << consoleItemIndex << ". Director #" << currentDirector << "'s " << playName << (directorIdToPlayNumberBusy[currentDirector] == currentPlayNumber ? "(busy)" : "") << endl;
			currentPlayNumber++;
		});

	});

	cout << "Please type a command (play, stop, exit) followed by an option (if required)" << endl;

	string currentCommand;
	cin >> currentCommand;

	auto commandOptionPair = parseCommand(currentCommand);

	if(commandOptionPair.first == C_INVALID) {
		cout << "Command not recognized, please try again" << endl;
		return;
	}

	int director =-1, playNumber = -1;
	if(commandOptionPair.second > 0) {
		auto directorPlayNumberIt = consoleNumberToDirectorIdAndPlayNumber.find(commandOptionPair.second);
		if(directorPlayNumberIt == consoleNumberToDirectorIdAndPlayNumber.end()) {
			cout << "Invalid option " << commandOptionPair.second << ", please try again" << endl;
			return;
		}
		director = directorPlayNumberIt->second.first;
		playNumber = directorPlayNumberIt->second.second;
	}

	processCommand(commandOptionPair.first, director, playNumber, shutdown);

}

void ConsoleUtils::processCommand(ConsoleCommand command, int  director, int play, bool &shutdown) {
	switch (command)
	{
	case C_PLAY:
		if(director<0 || play<0) {
			cout << "Play requires an option" << endl;
			return;
		}
		DirectorRegister::getInstance()->beginPlay(director, play);
		break;
	case C_STOP:
		if(director<0 || play<0) {
			cout << "Stop requires an option" << endl;
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
	regex commandRegex("\\s*(PLAY|STOP|EXIT)\\s*(\\d+)?", regex_constants::ECMAScript | regex_constants::icase);
	smatch match;
	if(regex_match(command, match, commandRegex) && match.size() > 0) {
		ConsoleCommand targetCommand = C_EXIT;
		if(match.str(1).compare(PLAY_COMMAND) == 0 || match.str(1).compare(PLAY_COMMAND_L) == 0)
			targetCommand = ConsoleCommand::C_PLAY;	
		else if (match.str(1).compare(STOP_COMMAND) == 0 || match.str(1).compare(STOP_COMMAND_L) == 0);
			targetCommand = ConsoleCommand::C_STOP;

		return pair<ConsoleCommand, int>(targetCommand, targetCommand != C_EXIT ? stoi(match.str(2)) : -1);

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
	if(sharedInstance != nullptr) delete sharedInstance;
}