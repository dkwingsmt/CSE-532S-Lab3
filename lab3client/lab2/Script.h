#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <string>
#include <vector>
#include <fstream>
#include <list>
#include <utility>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <stdexcept>
#include <atomic>

#include "common.h"
#include "Player.h"
#include "Play.h"

class Director;

class Script {
private:
    bool _ended;

    const static std::string PREFIX_SCENE;
    std::vector<tFragConfig> _scriptConfig;
    std::shared_ptr<Play> _play;
    std::list<std::shared_ptr<Player>> _players;

    std::atomic<bool> _hasDirector;
    PlayerRegistrar _registrar;

    std::thread _electionThread;

    // Returns biggestPairFrags
    size_t _readScript(std::string &scriptFileName);
    bool _readFragConfig(std::ifstream &, 
                std::list<tCharConfig> &, 
                std::string);
    void _recruit(size_t numPlayers);

    void _electionThreadFunc() {
        while (!_ended && _electDirector()) {
        }
    }

    // Return true if continue
    bool _electDirector();

public:
    Script(std::string scriptFileName, size_t numberOfPlayers=0) : 
        _ended(false), _hasDirector(false)
    {
        size_t biggestPairFrags = _readScript(scriptFileName);
        _recruit(std::max(biggestPairFrags, numberOfPlayers));
    }

    ~Script() {
        while (!_play->actEnded()) {
            std::this_thread::yield();
        }

        _ended = true;
        // Work threads of players are joined by ~Player
        _registrar.shutdown();
        if (_electionThread.joinable()) {
            _electionThread.join();
        }
    }

    void start() {
        _electionThread = std::thread([this] { _electionThreadFunc(); });
    }

    // Called by the now-director Player
    // Return true if continue
    bool cue(tFollowerTask task);

    void declareIdle(Player *me) {
        return _registrar.declareIdle(me);
    }

    void resign() {
        _hasDirector = false;
    }
    
};


#endif
