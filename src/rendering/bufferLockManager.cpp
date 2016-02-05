#include <phi\rendering\bufferLockManager.h>

const GLuint64 ONE_SECOND_IN_NANOSECONDS = 1000000000;

bufferLockManager::bufferLockManager(bool _cpuUpdates)
: _cpuUpdates(_cpuUpdates)
{

}

bufferLockManager::~bufferLockManager()
{
    for (auto it = _bufferLocks.begin(); it != _bufferLocks.end(); ++it) {
        cleanup(&*it);
    }

    _bufferLocks.clear();
}

void bufferLockManager::waitForLockedRange(size_t lockBeginBytes, size_t lockLength)
{
    bufferRange testRange = { lockBeginBytes, lockLength };
    std::vector<bufferLock> swapLocks;
    for (auto it = _bufferLocks.begin(); it != _bufferLocks.end(); ++it)
    {
        if (testRange.Overlaps(it->range)) 
        {
            wait(&it->syncObject);
            cleanup(&*it);
        }
        else
        {
            swapLocks.push_back(*it);
        }
    }

    _bufferLocks.swap(swapLocks);
}

void bufferLockManager::lockRange(size_t lockBeginBytes, size_t lockLength)
{
    bufferRange newRange = { lockBeginBytes, lockLength };
    GLsync syncName = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    bufferLock newLock = { newRange, syncName };

    _bufferLocks.push_back(newLock);
}

void bufferLockManager::wait(GLsync* syncObject)
{
    if  (_cpuUpdates) {
        GLbitfield waitFlags = 0;
        GLuint64 waitDuration = 0;
        while (1) {
            GLenum waitRet = glClientWaitSync(*syncObject, waitFlags, waitDuration);
            if (waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED) {
                return;
            }

            if (waitRet == GL_WAIT_FAILED) {
                //assert(!"Not sure what to do here. Probably raise an exception or something.");
                return;
            }

            // After the first time, need to start flushing, and wait for a looong time.
            waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
            waitDuration = ONE_SECOND_IN_NANOSECONDS;
        }
    } else {
        glWaitSync(*syncObject, 0, GL_TIMEOUT_IGNORED);
    }
}

void bufferLockManager::cleanup(bufferLock* bufferLock)
{
    glDeleteSync(bufferLock->syncObject);
}