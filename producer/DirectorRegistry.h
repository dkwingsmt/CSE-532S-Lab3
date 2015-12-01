#pragma once
#include <map>
#include <string>
#include <mutex>
#include <atomic>
#include "ClientHandler.h"

using namespace std;
class DirectorRegistry
{
	mutex dataStructureLock;
	map<string, pair<ClientHandler *, bool>> playNameToClientHandlerAvailabilityMap;
	
public:
	DirectorRegistry() {};
	DirectorRegistry(const DirectorRegistry&);
	DirectorRegistry operator=(const DirectorRegistry&);

	void make_busy(string);
	void interrupt_director_reciting_play(string);
	void quit();
	void make_free(ClientHandler *);
	void add_director(ClientHandler *, vector<string>);
	void remove_director(ClientHandler *);
	vector<pair<string, bool>> get_plays();

};

class GlobalRegistry {
	static DirectorRegistry *registry;
	static mutex instantiationLock;
	static atomic_bool shutdownRequested;

public:
	static DirectorRegistry* instance() {
		lock_guard<mutex> guard(instantiationLock);
		if(registry == nullptr)
			registry = new DirectorRegistry();

		return registry;
	}

	static void setShutdown() {
		shutdownRequested = true;		
	}

	static bool isShutdownRequested() {
		return shutdownRequested;
	}

	//TODO: Fix memory leak - method never gets called
	static void teardown() {
		delete registry;
	}
};
