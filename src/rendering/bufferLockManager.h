#pragma once
#include <phi.h>
#include "renderingApi.h"

namespace phi
{
    struct bufferRange
    {
        size_t startOffset;
        size_t length;

        bool Overlaps(const bufferRange& _rhs) const 
        {
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
        vector<bufferLock> _bufferLocks;
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
}