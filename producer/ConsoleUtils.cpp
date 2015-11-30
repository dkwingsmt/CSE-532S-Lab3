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
			
	});


	/*map<int, vector<string>> directorIdToPlayListMap;
	map<int, int> directorIdToBusyStatus;
	DirectorRegister::getInstance()->getPlayDetails(directorIdToPlayListMap, directorIdToBusyStatus);

	map<int, pair<int, int>> consoleNumberToDirectorIdAndPlayNumberMap;
	int consoleItemIndex = 0;
	
	for_each(directorIdToPlayListMap.begin(), directorIdToPlayListMap.end(), [&consoleItemIndex, &consoleNumberToDirectorIdAndPlayNumberMap, &directorIdToBusyStatus]
	(pair<int, vector<string>> p){

		int currentDirectorId = p.first;

		cout << "Director #" << currentDirectorId << (directorIdToBusyStatus[p.first] ? "\t(BUSY)" : "") << endl;			
		cout << "--------" << endl;

		int currentPlayIndex = 0;
		for_each(p.second.begin(), p.second.end(), [&currentDirectorId, &consoleItemIndex, &currentPlayIndex, &consoleNumberToDirectorIdAndPlayNumberMap](string playName){
			consoleNumberToDirectorIdAndPlayNumberMap[++consoleItemIndex] = pair<int,int>(currentDirectorId, currentPlayIndex);
			cout << consoleItemIndex << ". " << playName << endl;
		});

		cout << endl;

	});*/
}

pair<ConsoleCommand, int> parseCommand(string command) {
	regex commandRegex("\\s*(PLAY|STOP|EXIT)\\s*(\\d+)?", regex_constants::ECMAScript | regex_constants::icase);
	smatch match;
	if(regex_match(command, match, commandRegex) && match.size() > 0) {
		ConsoleCommand targetCommand = C_EXIT;
		if(match.str(1).compare(PLAY_COMMAND) == 0) 
			targetCommand = ConsoleCommand::C_PLAY;	
		else if (match.str(1).compare(STOP_COMMAND) == 0)		
			targetCommand = ConsoleCommand::C_STOP;

		return pair<ConsoleCommand, int>(targetCommand, match.size() > 1 ? stoi(match.str(2)) : -1);

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