#ifndef _PHI_BUFFER_LOCK_H_
#define _PHI_BUFFER_LOCK_H_

#include "rendering.h"

#include <GL\glew.h>
#include <vector>

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
    RENDERING_API bufferLockManager(bool _cpuUpdates);
    RENDERING_API ~bufferLockManager();
    RENDERING_API void waitForLockedRange(size_t _lockBeginBytes, size_t _lockLength);
    RENDERING_API void lockRange(size_t _lockBeginBytes, size_t _lockLength);
};

#endif _PHI_BUFFER_LOCK_H_