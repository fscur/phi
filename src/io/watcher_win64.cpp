#include <precompiled.h>

#include "watcher.h"
#include <diagnostics\windowsProcLoader.h>

namespace phi
{
    void watcher::watchDir()
    {
        char buf[256 * (sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH * sizeof(WCHAR))] = { 0 };
        DWORD bytesReturned = 0;
        BOOL result = FALSE;
        FILE_NOTIFY_INFORMATION *fni = NULL;

        _hDir = CreateFile(_path.c_str(),
            FILE_LIST_DIRECTORY | STANDARD_RIGHTS_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL);

        if (!_hDir || _hDir == INVALID_HANDLE_VALUE)
        {
            _queue->push(watcherMessage(_callback, "CreateFile failed"));
            return;
        }

        while (_programRunning)
        {
            result = ReadDirectoryChangesW(_hDir,
                buf,
                sizeof(buf) / sizeof(*buf),
                TRUE,
                FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_CREATION,
                &bytesReturned,
                NULL,
                NULL);

            if (result && bytesReturned)
            {
                std::wstring filenameWStr;
                string filenameStr;
                wchar_t filename[MAX_PATH];
                for (fni = (FILE_NOTIFY_INFORMATION*)buf; fni; )
                {
                    if (fni->FileNameLength)
                    {
                        wcsncpy_s(filename, MAX_PATH, fni->FileName, fni->FileNameLength / 2);
                        filename[fni->FileNameLength / 2] = 0;
                        filenameWStr = filename;
                        filenameStr = string(filenameWStr.begin(), filenameWStr.end());
                        _queue->push(watcherMessage(_callback, filenameStr));
                    }
                    else
                    {
                        _queue->push(watcherMessage(_callback, "undefined action"));
                    }

                    if (fni->NextEntryOffset)
                    {
                        char *p = (char*)fni;
                        fni = (FILE_NOTIFY_INFORMATION*)(p + fni->NextEntryOffset);
                        safeDelete(p);
                    }
                    else
                    {
                        fni = NULL;
                    }
                }
            }
            else
            {
                _queue->push(watcherMessage(_callback, "ReadDirectoryChangesW failed"));
            }
        }

        CloseHandle(_hDir);
    }

    void watcher::startWatch()
    {
        _watcherThread = std::thread(&watcher::watchDir, this);
    }

    void watcher::endWatch()
    {
        if (_programRunning)
        {
            _programRunning = false;

            if (windowsProcLoader::cancelIoEx(_hDir, NULL))
            {
                _watcherThread.join();
            }
            else
            {
                debug("<watcher> couldnt close watch handle: " + GetLastError());
            }
        }
    }
}