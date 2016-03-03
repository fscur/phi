#pragma once
#include <phi.h>
#include "form.h"

class application
{
public: 
	static phi::string path;
	static phi::string exePath;
public:
    application();
    ~application(void);

    void run(form* form);
};