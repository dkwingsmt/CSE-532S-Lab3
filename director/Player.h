#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>

#include "common.h"
#include "Play.h"

class Script;

class Player {

private:
    Play *_play;
    Script *_script;
    std::thread _workThread;
    std::vector<PlayLine> _lines;

    bool _hasTask;
    tPlayerTask _task;
    std::mutex _idleMutex;
    std::condition_variable _idleCv;

    void _read();
    void _act();
    void _start();

    void _assignFollowerSync(tFollowerTask);

    void _doLeader();
    void _doFollower() {
        _read();
        _play->enter(_task.followerTask.fragId);
        _act();
        _play->exit();
    }

    void _join() {
        _idleCv.notify_all();
        if (_workThread.joinable())
            _workThread.join();
    }

public:
    Player(Play *play, Script *script) :
        _play(play), 
        _script(script), 
        _hasTask(false)
    {
        _workThread = std::thread([this] { _start(); });
    }

    ~Player() {
        _join();
    }

    void assignFollower(tFollowerTask);
    void assignLeader(tLeaderTask);

};

class PlayerRegistrar {
private:
    std::atomic<Player*> _idler;
    bool _ended;
public:
    PlayerRegistrar() : _idler(nullptr), _ended(false) {}

    void declareIdle(Player *me) {
        Player *empty = nullptr;
        while (!_ended && !std::atomic_compare_exchange_strong(&_idler, &empty, me)) {
            empty = nullptr;
            std::this_thread::yield();
        }
    }

    Player *getIdle() {
        Player *leader = nullptr;
        while (!_ended && !(leader = (Player*)std::atomic_exchange(&_idler, nullptr))) {
            std::this_thread::yield();
        }
        return leader;
    }

    void shutdown() {
        _ended = true;
    }
};


#endif
