#include <precompiled.h>
#include "fileReader.h"

namespace phi
{
	string fileReader::readFile(const string& filePath)
	{
		std::ifstream file(filePath);
		std::string str;
		std::string fileContent;
		while (std::getline(file, str))
		{
			fileContent += str;
			fileContent.push_back('\n');
		}

		return fileContent;
	}
}