#pragma once
#include <map>
#include <mutex>
#include <atomic>
#include "ClientHandler.h"
using namespace std;
class DirectorRegister
{
	map<int, ClientHandler*> directorIdentifierToHandler;
	map<int, int> directorToPlayNumberBusy;
	map<int, vector<string>> directorIdToPlays;

	mutex registerLock;

	int idCounter;

	atomic_bool exitInitiated;

	static once_flag initFlag;
	static DirectorRegister* sharedInstance;
	static void init();

	DirectorRegister() : idCounter(0) { exitInitiated = false; }

public:

	DirectorRegister(const DirectorRegister&);
	DirectorRegister operator=(const DirectorRegister&);

	int addDirector(ClientHandler*, vector<string>);
	bool removeDirector(int directorId);

	bool getPlayDetails(map<int, vector<string>> &directorIdToPlays, map<int, int> &directorToPlayNumberBusy);
	void beginPlay(int directorId, int playNumber);
	bool stopDirector(int directorId, int playNum);
	void freeDirector(int directorId);
	void freeDirector(ClientHandler*);
	void exit();

	static DirectorRegister* getInstance();
	static void tearDown();

};

