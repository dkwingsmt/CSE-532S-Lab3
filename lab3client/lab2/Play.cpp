#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>
#include "common.h"
#include "Play.h"

using namespace std;

bool Play::recite(vector<PlayLine>::const_iterator &line,
            size_t fragId) {
    int reciteOrderCmp = 0;
    {
        unique_lock<mutex> ul(_reciteMutex);
        _reciteCv.wait(ul, [&reciteOrderCmp, this, fragId, &line]{ 
                    if (_ended)
                        return true;
                    reciteOrderCmp = _cmpFragLine(fragId, line->order,
                        _sceneFragCounter, _lineCounter);
                    return reciteOrderCmp <= 0; });
        if (_ended) {
            return false;
        }
        if (reciteOrderCmp < 0) {
            cerr << "Error: Asked to recite an earlier line [" 
                << fragId << ", " << line->order
                << "] but now at [" << _sceneFragCounter << ", "
                << _lineCounter << "]." << endl;
        }
        else {
            if (_currentPlayer != line->character) {
                cout << "\n" << line->character << "." << endl;
                _currentPlayer = line->character;
            }
            cout << line->text << endl;
            _lineCounter++;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    _reciteCv.notify_all();
    ++line;
    return true;
}

void Play::enter(size_t fragId) {
    unique_lock<mutex> ul(_reciteMutex);
    if (fragId < _sceneFragCounter) {
        ostringstream oss;
        oss << "Trying to enter an earlier fragment " <<
            fragId << " while currently at fragment " << 
            _sceneFragCounter << ".";
        throw invalid_argument(oss.str());
    }
    if (fragId > _sceneFragCounter) {
        _reciteCv.wait(ul, [&]{ 
                    return _ended || fragId == _sceneFragCounter; });
    }
    if (_ended) {
        return;
    }
    ++_onStage;
}

void Play::exit() {
    {
        lock_guard<mutex> lk(_reciteMutex);
        --_onStage;
        if (_onStage == 0) {
            if (_ended) {
                return;
            }
            _sceneFragCounter++;
            if (_sceneFragCounter != _sceneConfig.size() &&
                        _sceneConfig[_sceneFragCounter].title.size()) {
                cout << "\n\nScene ["
                    << _sceneConfig[_sceneFragCounter].title
                    << "]" << std::endl;
            }
            _lineCounter = 1;
        }
    }
    _reciteCv.notify_all();
}

tLeaderTask Play::getNextTask() {
    size_t nowFragId = _sceneFragDistributed++;
    return {nowFragId, _sceneConfig[nowFragId].chars};
}
