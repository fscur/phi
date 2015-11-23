#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "phi/demo/form.h"

#include <string>

class application
{
public: 
	static std::string path;
	static std::string exePath;
public:
    application();
    ~application(void);

    void run(form* form);
};

#endif