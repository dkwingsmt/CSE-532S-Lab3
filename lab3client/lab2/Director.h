#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include "Script.h"

class Director {
private:
    Script *_nowScript;
    std::vector<std::string> _scriptsFileName;
    size_t _numOfPlayers;
public:
    Director(std::vector<std::string> scriptsFileName, size_t numberOfPlayers = 0) : 
        _nowScript(NULL), _scriptsFileName(scriptsFileName), _numOfPlayers(numberOfPlayers)
    {
    }

    ~Director() {
        stopNowScript();
    }

    bool selectScript(size_t id) {
        if (id >= _scriptsFileName.size()) {
            return false;
        }
        stopNowScript();
        _nowScript = new Script(this, _scriptsFileName[id], _numOfPlayers, false);
        return _nowScript;
    }

    void stopNowScript() {
        if (_nowScript) {
            delete _nowScript;
        }
        _nowScript = NULL;
    }

    // Called by the now-director Player
    // Return true if continue
    bool cue(size_t fragId, tCharConfig charConfig) {
        return _nowScript->cue(fragId, charConfig);
    }

    // Must call Director::ended() after this function completes!
    bool actEnded() { return _nowScript->actEnded(); }

    void declareIdle(Player *me) {
        return _nowScript->declareIdle(me);
    }

    bool electDirector() {
        return _nowScript->electDirector();
    }

    void resign() {
        _nowScript->resign();
    }
    
};

#endif
