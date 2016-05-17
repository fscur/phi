#include <precompiled.h>
#include "watcher.h"

namespace phi {

	void watchDir(std::string path, phi::blockingQueue<watcherMessage>* queue, phi::watchCallback callback) {
		char buf[256 * (sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH * sizeof(WCHAR))] = { 0 };
		DWORD bytesReturned = 0;
		BOOL result = FALSE;
		FILE_NOTIFY_INFORMATION *fni = NULL;

		HANDLE hDir = CreateFile(path.c_str(),
			FILE_LIST_DIRECTORY | STANDARD_RIGHTS_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL);

		if (!hDir || hDir == INVALID_HANDLE_VALUE)
		{
			queue->push(phi::watcherMessage(callback, "CreateFile failed"));
		}

		while (true)
		{
			result = ReadDirectoryChangesW(hDir,
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
				std::string filenameStr;
				wchar_t filename[MAX_PATH];
				for (fni = (FILE_NOTIFY_INFORMATION*)buf; fni; )
				{
					if (fni->FileNameLength)
					{
						wcsncpy_s(filename, MAX_PATH, fni->FileName, fni->FileNameLength / 2);
						filename[fni->FileNameLength / 2] = 0;
						filenameWStr = filename;
						filenameStr = std::string(filenameWStr.begin(), filenameWStr.end());
						queue->push(phi::watcherMessage(callback, filenameStr));
					}
					else
					{
						queue->push(phi::watcherMessage(callback, "undefined action"));
					}

					if (fni->NextEntryOffset)
					{
						char *p = (char*)fni;
						fni = (FILE_NOTIFY_INFORMATION*)(p + fni->NextEntryOffset);
					}
					else
					{
						fni = NULL;
					}
				}
			}
			else
			{
				queue->push(phi::watcherMessage(callback, "ReadDirectoryChangesW failed"));
			}
		}

		CloseHandle(hDir);
	}

	void watcher::watchDirAsync(std::string path, phi::blockingQueue<watcherMessage>* queue, phi::watchCallback callback) {
		std::thread(watchDir, path, queue, callback).detach();
	}
}