#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>

#include "common.h"
#include "Play.h"

class Director;

class Player {

private:
    Play *_play;
    Director *_director;
    std::thread _workThread;
    std::vector<PlayLine> _lines;

    bool _hasTask;
    tPlayerTask _task;
    std::mutex _idleMutex;
    std::condition_variable _idleCv;

    std::atomic<bool> _ended;

    void _read();
    void _act();
    void _start();

    void _assignFollowerSync(tFollowerTask);

    void _doLeader();
    void _doFollower() {
        _enter();
        _exit();
    }

    // On stage
    void _enter() {
        _read();
        _play->enter(_task.followerTask.fragId);
        _act();
    }

    // Off stage
    void _exit() {
        _play->exit();
    }


    void _join() {
        {
            std::lock_guard<std::mutex> lk(_idleMutex);
            _ended = true;
        }

        _idleCv.notify_one();
        if (_workThread.joinable())
            _workThread.join();
    }

public:
    Player(Play *play, Director *director) :
        _play(play), 
        _director(director), 
        _hasTask(false),
        _ended(false)
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
    PlayerRegistrar() : _idler(NULL), _ended(false) {}

    void declareIdle(Player *me) {
        Player *empty = NULL;
        while (!std::atomic_compare_exchange_strong(&_idler, &empty, me) && !_ended) {
            empty = NULL;
            std::this_thread::yield();
        }
    }

    Player *getIdle() {
        Player *leader = NULL;
        while (!(leader = std::atomic_exchange<Player*>(&_idler, NULL)) && !_ended) {
            std::this_thread::yield();
        }
        return leader;
    }

    void shutdown() {
        _ended = true;
    }
};


#endif
