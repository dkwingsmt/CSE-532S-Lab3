#include "DirectorRegister.h"
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

void DirectorRegister::getPlayDetails(map<int, vector<string>> &directorIdToPlays, map<int, int> &directorToPlayNumberBusy) {
	lock_guard<mutex> guard(registerLock);
	directorIdToPlays = this->directorIdToPlays;
	directorToPlayNumberBusy = this->directorToPlayNumberBusy;
}

void DirectorRegister::beginPlay(int directorId, int playNumber) {
	if(exitInitiated) return;

	lock_guard<mutex> guard(registerLock);	
	ClientHandler *target = directorIdentifierToHandler[directorId];
	target->postMessage(ServerMessage(START, playNumber));	
	directorToPlayNumberBusy[directorId] = playNumber;
}

void DirectorRegister::stopDirector(int directorId) {
	lock_guard<mutex> guard(registerLock);	
	ClientHandler *target = directorIdentifierToHandler[directorId];
	target->postMessage(ServerMessage(STOP));
	directorToPlayNumberBusy[directorId] = -1;
}

void DirectorRegister::freeDirector(int directorId) {
	lock_guard<mutex> guard(registerLock);
	directorToPlayNumberBusy[directorId] = -1;
	std::cout << "Director #" << directorId << " is now free" << std::endl;
}

int DirectorRegister::addDirector(ClientHandler* handler, vector<string> playList) {
	if(exitInitiated) return -1;

	lock_guard<mutex> guard(registerLock);	
	int currentId = ++idCounter;

	directorIdentifierToHandler[currentId] = handler;
	directorToPlayNumberBusy[currentId] = -1;
	directorIdToPlays[currentId] = playList;

	return currentId;
}

bool DirectorRegister::removeDirector(int directorId) {
	lock_guard<mutex> guard(registerLock);
	directorIdentifierToHandler.erase(directorId);
	directorIdToPlays.erase(directorId);
	directorToPlayNumberBusy.erase(directorId);
	bool lastDirector = --idCounter == 0;

	if(lastDirector && exitInitiated) 
		ACE_Reactor::instance()->end_reactor_event_loop();		

	return lastDirector;
	
}

void DirectorRegister::exit() {
	lock_guard<mutex> guard(registerLock);				
	exitInitiated = true;
	for_each(directorIdentifierToHandler.begin(), directorIdentifierToHandler.end(), [](pair<int, ClientHandler*> p){
		p.second->postMessage(ServerMessage(POISON));
	});
}
