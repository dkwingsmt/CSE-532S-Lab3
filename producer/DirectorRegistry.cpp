#include "DirectorRegistry.h"


void DirectorRegistry::add_director(ClientHandler *handler, vector<string> plays) {
	lock_guard<mutex> guard(dataStructureLock);
	pair<ClientHandler *, bool> p;
	p.first = handler;
	p.second = true;
	for(auto it : plays)
		this->playNameToClientHandlerAvailabilityMap[it] = p;
}

void DirectorRegistry::remove_director(ClientHandler *handler) {
	lock_guard<mutex> guard(dataStructureLock);
	/*remove_if(this->playNameToClientHandlerAvailabilityMap.begin(), this->playNameToClientHandlerAvailabilityMap.end(), [handler](pair<const string, pair<ClientHandler*, bool>> p) {
		return p.second.first == handler;
	});*/
}

void DirectorRegistry::make_busy(string playName) {
	lock_guard<mutex> guard(dataStructureLock);	
	playNameToClientHandlerAvailabilityMap[playName].second = false;
}

void DirectorRegistry::make_free(ClientHandler *handler) {
	lock_guard<mutex> guard(dataStructureLock);
	
	for(auto it = this->playNameToClientHandlerAvailabilityMap.begin();it != this->playNameToClientHandlerAvailabilityMap.end(); ++it) {
		if(it->second.first == handler) {
			it->second.second = true;
			break;
		}
	}
		
}

void DirectorRegistry::interrupt_director_reciting_play(string playName) {
	lock_guard<mutex> guard(dataStructureLock);
	Message m;
	m.type = STOP;
	this->playNameToClientHandlerAvailabilityMap[playName].first->postMessage(m);
	this->playNameToClientHandlerAvailabilityMap[playName].second = true;
}

void DirectorRegistry::quit() {
	lock_guard<mutex> guard(dataStructureLock);
	Message m;	
	m.type = POISON;
	for_each(this->playNameToClientHandlerAvailabilityMap.begin(), this->playNameToClientHandlerAvailabilityMap.end(), [&m](pair<string, pair<ClientHandler*, bool>> p) {
		p.second.first->postMessage(m);
	});
}

vector<pair<string, bool>> DirectorRegistry::get_plays() {
	lock_guard<mutex> guard(dataStructureLock);
	vector<pair<string, bool>> temp;

	for_each(this->playNameToClientHandlerAvailabilityMap.begin(), this->playNameToClientHandlerAvailabilityMap.end(), [&temp](pair<string, pair<ClientHandler*, bool>> p){
		pair<string, bool> t;
		t.first = p.first;
		t.second = p.second.second;
		temp.push_back(move(t));
	});

	return move(temp);
}

DirectorRegistry *GlobalRegistry::registry;
mutex GlobalRegistry::instantiationLock;
atomic_bool GlobalRegistry::shutdownRequested;
