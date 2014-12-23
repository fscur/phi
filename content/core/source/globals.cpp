#include "globals.h"

#include <iostream>
#include <sstream>

namespace phi
{
    bool globals::contains(const std::string &s, std::string value)
    {
        return !(s.find(value) == std::string::npos);
    };

    std::vector<std::string> &globals::split(const std::string &s, char delim, std::vector<std::string> &elems) 
    {
        std::stringstream ss(s);
        std::string item;

        while (getline(ss, item, delim))
            elems.push_back(item);

        return elems;
    }

    std::vector<std::string> globals::split(const std::string &s, char delim) 
    {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }

    void globals::toArray(std::vector<unsigned int> values, unsigned int* &intArray)
    {
        int i = 0;

        for (std::vector<unsigned int>::iterator iterator = values.begin(); iterator != values.end(); ++iterator)
            intArray[i++] = *iterator;
    }
}