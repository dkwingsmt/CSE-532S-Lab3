#include <sstream>
#include "Script.h"
#include "common.h"

using namespace std;

const string Script::PREFIX_SCENE = "[scene]";

size_t Script::_readScript(string &scriptFileName) {
    ifstream scriptFile(scriptFileName);
    if (!scriptFile) {
        throw invalid_argument(scriptFileName);
    }

    string line;
    string scene_title;
    string dir = dirnameOf(scriptFileName);
    size_t biggestPairFrags = 0;
    size_t sizeLastFrag = 0;
    while (getline(scriptFile, line)) {
        if (line[0] == '[') {
            if (line.substr(0, PREFIX_SCENE.size()) != PREFIX_SCENE) {
                throw invalid_argument(line);
            }
            scene_title = trim(line.substr(PREFIX_SCENE.size()));
        }
        else {
            // Read fragment file
            _scriptConfig.push_back({scene_title, {}});
            string fragFileName = dir + trim(line);
            ifstream fragFile(fragFileName);
            if (!fragFile) {
                cout << "Can't open character file " << fragFileName << endl;
                continue;
            }
            _readFragConfig(fragFile, _scriptConfig.back().chars, dir);

            // Refresh scene title
            scene_title = "";

            // Calculate pair-of-frags
            size_t sizeThisFrag = _scriptConfig.back().chars.size();
            biggestPairFrags = max(biggestPairFrags, sizeLastFrag+sizeThisFrag);
            sizeLastFrag = sizeThisFrag;
        }
    }

    return biggestPairFrags;
}

bool Script::_readFragConfig(ifstream &fragFile, 
            std::list<tCharConfig> &chars, string dir) {
    list<Player> players;
    // Read play file
    string line;
    while (getline(fragFile, line))
    {
        line = trim(line);
        if (line.empty())
            continue;
        string characterName;
        string inputFileName;
        if (istringstream(line) >> characterName >> inputFileName)
        {
            chars.push_back({characterName, dir+inputFileName});
        }
    }
    return true;
}

void Script::_recruit(size_t numPlayers) {
    _play = make_shared<Play>(_scriptConfig);
    _players.clear();
    for (size_t i = 0; i < numPlayers; i++) {
        _players.push_back(make_shared<Player>(_play.get(), this));
    }
}

bool Script::_electDirector() {
    // Wait until there's no director
    while (_hasDirector) {
        this_thread::yield();
    }
    // Wait until there's an idle
    
    Player *leader = _registrar.getIdle();
    if (leader == NULL) {
        return false;
    }
    _hasDirector = true;
    
    leader->assignLeader(_play->getNextTask());

    return !_play->distributeEnded();
}

bool Script::cue(tFollowerTask task) {
    Player *follower = _registrar.getIdle();
    if (!follower) {
        return false;
    }
    follower->assignFollower(task);
    return true;
}

