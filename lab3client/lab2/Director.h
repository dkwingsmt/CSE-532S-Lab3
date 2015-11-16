#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include "Script.h"

class Director {
private:
    Script _script;
public:
    Director(std::string scriptFileName, size_t numberOfPlayers = 0, bool bOverride = false) : 
        _script(this, scriptFileName, numberOfPlayers, bOverride)
    {
    }

	~Director() {
        // Work threads of players are joined by ~Player
	}

    // Called by the now-director Player
    void cue(size_t fragId, tCharConfig charConfig) {
        _script.cue(fragId, charConfig);
    }

    // Must call Director::ended() after this function completes!
	bool actEnded() { return _script.actEnded(); }

    void declareIdle(Player *me) {
        return _script.declareIdle(me);
    }

    bool electDirector() {
        return _script.electDirector();
    }

	void resign() {
        _script.resign();
	}
    
};

#endif
