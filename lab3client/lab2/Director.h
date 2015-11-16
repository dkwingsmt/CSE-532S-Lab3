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
        _nowScript = new Script(_scriptsFileName[id], _numOfPlayers);
        _nowScript->start();
        return _nowScript;
    }

    void stopNowScript() {
        if (_nowScript) {
            delete _nowScript;
        }
        _nowScript = NULL;
    }
};

#endif
