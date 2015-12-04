#include "DirectorRegister.h"
#include "ConsoleLocker.h"
#include <iostream>
once_flag DirectorRegister::initFlag;
DirectorRegister* DirectorRegister::sharedInstance;

void DirectorRegister::init() {
	if(sharedInstance == nullptr) sharedInstance = new DirectorRegister();
}

DirectorRegister* DirectorRegister::getInstance() {
	call_once(initFlag, &DirectorRegister::init);
	return sharedInstance;
}

void DirectorRegister::tearDown() {
	if(sharedInstance != nullptr) delete sharedInstance;
}

bool DirectorRegister::getPlayDetails(map<int, vector<string>> &directorIdToPlays, map<int, int> &directorToPlayNumberBusy) {
	lock_guard<mutex> guard(registerLock);
	if(exitInitiated) return false;
	directorIdToPlays = this->directorIdToPlays;
	directorToPlayNumberBusy = this->directorToPlayNumberBusy;

	return true;
}

void DirectorRegister::beginPlay(int directorId, int playNumber) {
	if(exitInitiated) return;

	lock_guard<mutex> guard(registerLock);	
	ClientHandler *target = directorIdentifierToHandler[directorId];
	target->postMessage(ServerMessage(START, playNumber));	
	directorToPlayNumberBusy[directorId] = playNumber;
}

bool DirectorRegister::stopDirector(int directorId, int playNum) {
	lock_guard<mutex> guard(registerLock);	
	if (directorToPlayNumberBusy[directorId] != playNum)
	{
		return false;
	}
	ClientHandler *target = directorIdentifierToHandler[directorId];
	target->postMessage(ServerMessage(STOP));
	directorToPlayNumberBusy[directorId] = -1;
	return true;
}

void DirectorRegister::freeDirector(int directorId) {
	lock_guard<mutex> guard(registerLock);
	directorToPlayNumberBusy[directorId] = -1;
	CL_OUT(std::cout << std::endl << "[NOTIFICATION] Director #" << directorId << " is now free (press enter to refresh)" << std::endl << std::endl);
}

int DirectorRegister::addDirector(ClientHandler* handler, vector<string> playList) {
	if(exitInitiated) return -1;

	lock_guard<mutex> guard(registerLock);	
	int currentId = ++idCounter;

	directorIdentifierToHandler[currentId] = handler;
	directorToPlayNumberBusy[currentId] = -1;
	directorIdToPlays[currentId] = playList;

	CL_OUT(cout << endl << "[NOTIFICATION] New Director (#" << currentId << ") Available (press enter to refresh)" << endl << endl);

	return currentId;
}

bool DirectorRegister::removeDirector(int directorId) {
	lock_guard<mutex> guard(registerLock);
	directorIdentifierToHandler.erase(directorId);
	directorIdToPlays.erase(directorId);
	directorToPlayNumberBusy.erase(directorId);
	bool lastDirector = --idCounter == 0;

	CL_OUT(cout << endl << "[NOTIFICATION] Director #" << directorId << " has disconnected (press enter to refresh)" << endl << endl);

	if(lastDirector && exitInitiated) 
		ACE_Reactor::instance()->end_reactor_event_loop();		

	return lastDirector;
	
}

void DirectorRegister::exit() {
	lock_guard<mutex> guard(registerLock);				

	if(exitInitiated) return;
	exitInitiated = true;

	if(idCounter == 0) {
		ACE_Reactor::instance()->end_reactor_event_loop();
		return;
	}

	for_each(directorIdentifierToHandler.begin(), directorIdentifierToHandler.end(), [](pair<int, ClientHandler*> p){
		p.second->postMessage(ServerMessage(POISON));
	});
}
