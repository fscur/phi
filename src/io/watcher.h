#pragma once

#include <phi.h>
#ifdef _WIN32
#include<Windows.h>
#endif
#include <core/blockingQueue.h>
#include "ioApi.h"

namespace phi
{
    typedef	std::function<void(const string&)> watchCallback;

    struct watcherMessage
    {
        watcherMessage(phi::watchCallback cb, string fileChanged) :
            callback(cb), fileChanged(fileChanged) {}

        phi::watchCallback callback;
        string fileChanged;
    };

    class watcher
    {
    private:
        std::atomic<bool> _programRunning;
        string _path;
        phi::blockingQueue<watcherMessage>* _queue;
        phi::watchCallback _callback;
        std::thread _watcherThread;
#ifdef _WIN32
        HANDLE _hDir;
#endif
        void watchDir();
    public:
        IO_API watcher(const string& path, phi::blockingQueue<phi::watcherMessage>* queue, phi::watchCallback callback) :
            _programRunning(true), _path(path), _queue(queue), _callback(callback) {}
        IO_API void startWatch();
        IO_API void endWatch();
    };
}