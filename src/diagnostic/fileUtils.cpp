#include <precompiled.h>
#include "fileUtils.h"

void fileUtils::getDirectoryFromPath(const char* fullPath, char* directory)
{
    char aux[FILENAME_MAX] = { 0 };
    auto directoryCharacterCounter = 0;
    auto auxCounter = 0;
    auto pathSize = strlen(fullPath);
    auto pathSeparator = L'\\';

    for (size_t i = 0; i < pathSize; i++)
    {
        if (fullPath[i] != pathSeparator)
        {
            if (auxCounter < FILENAME_MAX)
                aux[auxCounter++] = fullPath[i];
            else
                break;
        }
        else
        {
            for (int j = 0; j < auxCounter; j++)
            {
                directory[directoryCharacterCounter++] = aux[j];
                aux[j] = 0;
            }

            directory[directoryCharacterCounter++] = fullPath[i];
            auxCounter = 0;
        }
    }
}
