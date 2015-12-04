#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include "Script.h"

class Director {
private:
    Script *_nowScript;
    std::vector<std::string> _scriptsFileName;
    size_t _numOfPlayers;
	size_t currentPlayId;
    std::function<void(void)> _onActEnd;

    void _executeOnActEnd() {
        _onActEnd();
    }
public:
    Director(std::vector<std::string> scriptsFileName, size_t numberOfPlayers = 0) : 
        _nowScript(NULL), _scriptsFileName(scriptsFileName), _numOfPlayers(numberOfPlayers),
        _onActEnd([]{})
    {
    }

    ~Director() {
        stopNowScript();
    }

	size_t getCurrentPlayId() {
		return currentPlayId;
	}

	void resetPlayId() {
		currentPlayId = -1;
	}

    bool selectScript(size_t id) {
        if (id >= _scriptsFileName.size()) {
            return false;
        }
        stopNowScript();
		currentPlayId = id;
        _nowScript = new Script(_scriptsFileName[id], _numOfPlayers, [this]{ _executeOnActEnd(); });
        _nowScript->start();
        return _nowScript != NULL;
    }

    void stopNowScript() {
        if (_nowScript) {
            delete _nowScript;
        }
        _nowScript = NULL;
    }

    void onActEnd(std::function<void(void)> f) {
        _onActEnd = f;
    }

    bool live() {
        if (_nowScript && _nowScript->live()) {
            return true;
        }
        else {
            stopNowScript();
            return false;
        }
    }

	std::vector<std::string> getScriptsFileName()
	{
		return _scriptsFileName;
	}
	
};

#endif
