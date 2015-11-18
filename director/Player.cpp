#include <iostream>
#include "common.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include "Director.h"
#include "Player.h"

using namespace std;

void Player::_read() {
    ifstream charFile(_task.followerTask.charFileName);
    if (!charFile) {
        cout << "unable to open file" 
            << _task.followerTask.charFileName << endl;
        return;
    }

    string line;
    _lines.clear();
    while (getline(charFile, line)) {
        istringstream iss(line);
        size_t lineNumber;
        string text;

        if (iss >> lineNumber) {
            if (getline(iss, text)) {
                trim(text);
                if (!text.empty()) {
					PlayLine playline;
					playline.order = lineNumber;
					playline.character = _task.followerTask.charName;
					playline.text = text;
                    _lines.push_back(playline);
				}
            }
        }
    }
}

void Player::_act() {
    sort(_lines.begin(), _lines.end());
    for(vector<PlayLine>::const_iterator playIterator = _lines.begin(); 
                playIterator != _lines.end(); /* Blank */) {
        if (!_play->recite(playIterator, _task.followerTask.fragId)) {
            return;
        }
    }
}

void Player::_start() {
    while (!_play->actEnded()) {
        _hasTask = false;
        _script->declareIdle(this);
        {
            unique_lock<mutex> lk(_idleMutex); 
            while (!_hasTask && !_play->actEnded()) {
                _idleCv.wait(lk);
            }
            if (_play->actEnded()) {
                return;
            }
        }
        if (_task.isLeader) {
            _doLeader();
        }
        else {
            _doFollower();
        }
    }
}

void Player::_assignFollowerSync(tFollowerTask task) {
    _task.followerTask = task;
    _task.isLeader = false;
}

void Player::assignFollower(tFollowerTask task) {
    {
        lock_guard<mutex> lk(_idleMutex);
        _assignFollowerSync(task);
        _hasTask = true;
    }
    _idleCv.notify_one();
}

void Player::assignLeader(tLeaderTask task) {
    {
        lock_guard<mutex> lk(_idleMutex);
        _task.leaderTask = task;
        _task.isLeader = true;
        _hasTask = true;
    }
    _idleCv.notify_one();
}

void Player::_doLeader() {
    size_t fragId = _task.leaderTask.fragId;
    auto chars = _task.leaderTask.chars;
    auto myChar = chars.begin();
    auto newChar = myChar;
    ++newChar;
    for(; newChar != chars.end(); newChar++) {
		tFollowerTask task;
		task.fragId = fragId;
		task.charName = newChar->first;
		task.charFileName = newChar->second;
        if (!_script->cue(task)){
            return;
        }
    }
    tFollowerTask task;
	task.fragId = fragId;
	task.charName = myChar->first;
	task.charFileName = myChar->second;
    _assignFollowerSync(task);
    _script->resign();
    _doFollower();
}
