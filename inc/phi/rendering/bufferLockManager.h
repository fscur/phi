#ifndef _PHI_BUFFER_LOCK_H_
#define _PHI_BUFFER_LOCK_H_

#include <vector>
#include <GL\glew.h>

struct bufferRange
{
    unsigned int startOffset;
    unsigned int length;

    bool Overlaps(const bufferRange& _rhs) const {
        return startOffset < (_rhs.startOffset + _rhs.length)
            && _rhs.startOffset < (startOffset + length);
    }
};

struct bufferLock
{
    bufferRange range;
    GLsync syncObject;
};

class bufferLockManager
{
private:
    std::vector<bufferLock> _bufferLocks;
    bool _cpuUpdates; // Whether it's the CPU (true) that updates, or the GPU (false)
private:
    void wait(GLsync* _syncObject);
    void cleanup(bufferLock* _bufferLock);
public:
    bufferLockManager(bool _cpuUpdates);
    ~bufferLockManager();
    void waitForLockedRange(size_t _lockBeginBytes, size_t _lockLength);
    void lockRange(size_t _lockBeginBytes, size_t _lockLength);
};

#endif _PHI_BUFFER_LOCK_H_