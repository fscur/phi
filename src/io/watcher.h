#pragma once
#include <phi.h>
#include "ioApi.h"
#include <core/blockingQueue.h>

namespace phi {

	typedef std::function<void(std::string)> watchCallback;

	struct watcherMessage {
		watcherMessage(phi::watchCallback cb, std::string fileChanged) : callback(cb), fileChanged(fileChanged) {}
		phi::watchCallback callback;
		std::string fileChanged;
	};

	class watcher {
	public:
		IO_API static void watchDirAsync(std::string path, phi::blockingQueue<watcherMessage>* queue, phi::watchCallback callback);
	};
}